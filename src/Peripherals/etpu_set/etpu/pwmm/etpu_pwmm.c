/*******************************************************************************
 *
 *   (c) Copyright 2004-2016 Freescale Semiconductor, Inc.
 *   Copyright 2017 - 2018 NXP
 *   NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
 *   in accordance with the applicable license terms. By expressly accepting
 *   such terms or by downloading, installing, activating and/or otherwise using
 *   the software, you are agreeing that you have read, and that you agree to
 *   comply with and are bound by, such license terms. If you do not agree to
 *   be bound by the applicable license terms, then you may not retain,
 *   install, activate or otherwise use the software.
****************************************************************************//*!
*
* @file    etpu_pwmm.c
*
* @author  Marketa Venclikova [nxa17216]
* 
* @version 1.2
* 
* @date    14-February-2018
*
* @brief   This file contains API for using the eTPU function
*          Pulse Width Modulation for Motor Control (PWMM).
* 
****************************************************************************//*!
*
* @mainpage
*
* The eTPU PWMM APIs @ref etpu_pwmm.c/.h includes API functions for eTPU
* function Pulse Width modulation for Motor Control.
* 
*******************************************************************************/
/*******************************************************************************
*                                        INCLUDE FILES
*******************************************************************************/

#include "etpu_pwmm.h"    /* private header file */
#include "../_utils/etpu_util.h"    /* utility routines for working with the eTPU */

/*******************************************************************************
*                                      GLOBAL VARIABLES
*******************************************************************************/
extern uint32_t fs_etpu_data_ram_start;
extern uint32_t fs_etpu_data_ram_ext;

/*******************************************************************************
*                                       GLOBAL FUNCTIONS
*******************************************************************************/
/*******************************************************************************
* FUNCTION: fs_etpu_pwmm_init
****************************************************************************//*!
* @brief   This function initializes eTPU channels to run PWMM function.
*
* @note    The following actions are performed in order:
*          -# Use user-defined CPBA or allocate new eTPU DATA RAM
*          -# Write chan config registers and FM bits
*          -# Write channel parameters
*          -# Write HSR
*          -# Set channel priority
*
* @param   *p_pwmm_instance - This is a pointer to the instance structure 
*            @ref pwmm_instance_t.
* @param   *p_pwmm_config - This is a pointer to the structure of configuration
*            parameters @ref pwmm_config_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_MALLOC - eTPU DATA RAM memory allocation error
*          - @ref FS_ETPU_ERROR_NONE - No error
*
* @warning This function does not configure the pins, only the eTPU channels.
*******************************************************************************/
uint32_t fs_etpu_pwmm_init(
  struct pwmm_instance_t *p_pwmm_instance,
  struct pwmm_config_t   *p_pwmm_config)
{
  uint8_t  chan_num_master;
  uint8_t  chan_num_phase_a;
  uint8_t  chan_num_phase_b;
  uint8_t  chan_num_phase_c;
  uint8_t  priority;
  uint32_t *cpba;
  uint32_t cr;

  chan_num_master  = p_pwmm_instance->chan_num_master;   
  chan_num_phase_a = p_pwmm_instance->chan_num_phase_a;
  chan_num_phase_b = p_pwmm_instance->chan_num_phase_b;
  chan_num_phase_c = p_pwmm_instance->chan_num_phase_c;
  priority         = p_pwmm_instance->priority;                
  cpba             = p_pwmm_instance->cpba;                       
  
  /* Disable channel priorities */
  fs_etpu_enable(chan_num_master, FS_ETPU_PRIORITY_DISABLE);
  
  fs_etpu_enable(chan_num_phase_a, FS_ETPU_PRIORITY_DISABLE); 
  fs_etpu_enable(chan_num_phase_b, FS_ETPU_PRIORITY_DISABLE); 
  fs_etpu_enable(chan_num_phase_c, FS_ETPU_PRIORITY_DISABLE); 
  
  if(p_pwmm_instance->phase_type == FS_ETPU_PWMM_FM0_COMPLEMENTARY_PAIRS)
  {
    fs_etpu_enable(chan_num_phase_a + 1, FS_ETPU_PRIORITY_DISABLE); 
    fs_etpu_enable(chan_num_phase_b + 1, FS_ETPU_PRIORITY_DISABLE); 
    fs_etpu_enable(chan_num_phase_c + 1, FS_ETPU_PRIORITY_DISABLE); 
  }

  /* Use user-defined CPBA or allocate new eTPU DATA RAM */
  if(cpba == 0)
  {
    cpba = fs_etpu_malloc(FS_ETPU_PWMM_NUM_PARMS);
    if(cpba == 0)
    {
      return(FS_ETPU_ERROR_MALLOC);
    }
    else
    {
      p_pwmm_instance->cpba = cpba;
    }
  }
  
  /* Write chan config registers and FM bits */
  cr = (FS_ETPU_PWMM_TABLE_SELECT << 24) +
       (FS_ETPU_PWMM_FUNCTION_NUMBER << 16) +
       (((uint32_t)cpba - fs_etpu_data_ram_start) >> 3);
  eTPU->CHAN[chan_num_master].CR.R = cr; 
  eTPU->CHAN[chan_num_master].SCR.R = (uint32_t)p_pwmm_instance->phase_type + p_pwmm_config->update_option;
  
  eTPU->CHAN[chan_num_phase_c].CR.R = cr; 
  eTPU->CHAN[chan_num_phase_b].CR.R = cr; 
  eTPU->CHAN[chan_num_phase_a].CR.R = cr; 
  
  if(p_pwmm_instance->phase_type == FS_ETPU_PWMM_FM0_COMPLEMENTARY_PAIRS)
  {
    eTPU->CHAN[chan_num_phase_c + 1].CR.R = cr; 
    eTPU->CHAN[chan_num_phase_b + 1].CR.R = cr; 
    eTPU->CHAN[chan_num_phase_a + 1].CR.R = cr; 
  }
  
  /* min_pw value must be non-zero */
  if(p_pwmm_config->min_pulse_width == 0)
  {
    p_pwmm_config->min_pulse_width = 1;
  }
  /* Write channel parameters */
  *(cpba + ((FS_ETPU_PWMM_OFFSET_INPUTS_PERIOD      - 1)>>2)) = p_pwmm_config->period;
  *(cpba + ((FS_ETPU_PWMM_OFFSET_INPUTS_DEAD_TIME   - 1)>>2)) = p_pwmm_config->dead_time;
  *(cpba + ((FS_ETPU_PWMM_OFFSET_INPUTS_MIN_PW      - 1)>>2)) = p_pwmm_config->min_pulse_width;
  *(cpba + ((FS_ETPU_PWMM_OFFSET_INPUTS_UPDATE_TIME - 1)>>2)) = p_pwmm_config->update_time;
  *(cpba + ((FS_ETPU_PWMM_OFFSET_START_OFFSET       - 1)>>2)) = p_pwmm_instance->start_offset;
  *(cpba + ((FS_ETPU_PWMM_OFFSET_INPUTS_A           - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PWMM_OFFSET_INPUTS_B           - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PWMM_OFFSET_INPUTS_C           - 1)>>2)) = 0;

  *((uint8_t*)cpba + FS_ETPU_PWMM_OFFSET_CHAN_MASTER        ) = chan_num_master;
  *((uint8_t*)cpba + FS_ETPU_PWMM_OFFSET_POLARITY           ) = p_pwmm_instance->polarity;
  *((uint8_t*)cpba + FS_ETPU_PWMM_OFFSET_INPUTS_MODE        ) = p_pwmm_config->mode;
  *((uint8_t*)cpba + FS_ETPU_PWMM_OFFSET_INPUTS_MODULATION  ) = p_pwmm_config->modulation;
  *((uint8_t*)cpba + FS_ETPU_PWMM_OFFSET_PHASEA_CHAN_NUM    ) = chan_num_phase_a;
  *((uint8_t*)cpba + FS_ETPU_PWMM_OFFSET_PHASEB_CHAN_NUM    ) = chan_num_phase_b;
  *((uint8_t*)cpba + FS_ETPU_PWMM_OFFSET_PHASEC_CHAN_NUM    ) = chan_num_phase_c;
  
  /* Write HSR */  
  eTPU->CHAN[chan_num_master].HSRR.R = FS_ETPU_PWMM_HSR_INIT;
  
  /* Set channel priority */
  fs_etpu_enable(chan_num_master, priority);

  return(FS_ETPU_ERROR_NONE);
}

/*******************************************************************************
* FUNCTION: fs_etpu_pwmm_config
****************************************************************************//*!
* @brief   This function changes the PWMM configuration.
*
* @note    The following actions are performed in order:
*          -# Write configuration parameter values to eTPU DATA RAM
*          -# Write channel FM bits
*
* @param   *p_pwmm_instance - This is a pointer to the instance structure 
*            @ref pwmm_instance_t.
* @param   *p_pwmm_config - This is a pointer to the structure of configuration
*            parameters @ref pwmm_config_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_NONE - No error
*
*******************************************************************************/
uint32_t fs_etpu_pwmm_config(
  struct pwmm_instance_t *p_pwmm_instance,
  struct pwmm_config_t   *p_pwmm_config)
{
  uint32_t *cpba;
  uint32_t *cpbae;
  uint32_t  error = FS_ETPU_ERROR_NONE;

  cpba = p_pwmm_instance->cpba;                       
  cpbae = cpba + (0x4000 >> 2); /* sign-extended memory area */
  
  /* min_pw value must be non-zero */
  if(p_pwmm_config->min_pulse_width == 0)
  {
    p_pwmm_config->min_pulse_width = 1;
  }
  /* Write channel parameters - use cpbae to prevent from overwriting bits 31:24 */
  *(cpbae + ((FS_ETPU_PWMM_OFFSET_INPUTS_PERIOD      - 1)>>2)) = p_pwmm_config->period;
  *(cpbae + ((FS_ETPU_PWMM_OFFSET_INPUTS_DEAD_TIME   - 1)>>2)) = p_pwmm_config->dead_time;
  *(cpbae + ((FS_ETPU_PWMM_OFFSET_INPUTS_MIN_PW      - 1)>>2)) = p_pwmm_config->min_pulse_width;
  *(cpbae + ((FS_ETPU_PWMM_OFFSET_INPUTS_UPDATE_TIME - 1)>>2)) = p_pwmm_config->update_time;

  *((uint8_t*)cpba + FS_ETPU_PWMM_OFFSET_INPUTS_MODE         ) = p_pwmm_config->mode;
  *((uint8_t*)cpba + FS_ETPU_PWMM_OFFSET_INPUTS_MODULATION   ) = p_pwmm_config->modulation;

  /* Write channel FM bits */
  eTPU->CHAN[p_pwmm_instance->chan_num_master].SCR.B.FM1 = (uint32_t)p_pwmm_config->update_option >> 1;

  return(error);
}

/*******************************************************************************
* FUNCTION: fs_etpu_pwmm_set_inputs
****************************************************************************//*!
* @brief   This function writes input parameter values to the PWMM function.
*
* @note    The following actions are performed in order:
*          -# Write inputs parameter values to eTPU DATA RAM
*          -# Check there is no HSR pending.
*          -# Set HSR to make the PWMM function to use the new values
*
* @param   *p_pwmm_instance - This is a pointer to the instance structure 
*            @ref pwmm_instance_t.
* @param   *p_pwmm_inputs - This is a pointer to the structure of inputs
*            @ref pwmm_inputs_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_TIMING - There is pending HSR not serviced yet.
*              The function call needs to be done again at a later time.
*          - @ref FS_ETPU_ERROR_NONE - No error
*
* @warning This function writes raw input values. 
*******************************************************************************/
uint32_t fs_etpu_pwmm_set_inputs(
  struct pwmm_instance_t *p_pwmm_instance,
  struct pwmm_inputs_t   *p_pwmm_inputs)
{
  uint32_t *cpbae;
  uint32_t  error = FS_ETPU_ERROR_NONE;

  cpbae = p_pwmm_instance->cpba + (0x4000 >> 2); /* sign-extended memory area */
  
  /* Check there is no HSR pending */  
  if(eTPU->CHAN[p_pwmm_instance->chan_num_master].HSRR.R != 0)
  {
    error = FS_ETPU_ERROR_TIMING;
  }
  else
  {
    /* Write channel parameters */
    *(cpbae + ((FS_ETPU_PWMM_OFFSET_INPUTS_A - 1)>>2)) = p_pwmm_inputs->input_a;
    *(cpbae + ((FS_ETPU_PWMM_OFFSET_INPUTS_B - 1)>>2)) = p_pwmm_inputs->input_b;
    *(cpbae + ((FS_ETPU_PWMM_OFFSET_INPUTS_C - 1)>>2)) = p_pwmm_inputs->input_c;

    /* Write HSR */  
    eTPU->CHAN[p_pwmm_instance->chan_num_master].HSRR.R = FS_ETPU_PWMM_HSR_UPDATE;
  }  

  return(error);
}

/*******************************************************************************
* FUNCTION: fs_etpu_pwmm_disable
****************************************************************************//*!
* @brief   This function enables the PWM outputs, 
*          starting them from the next PWM period.
*
* @note    The following actions are performed in order:
*          -# Check there is no HSR pending.
*          -# Set HSR to enable the PWM outputs.
*
* @param   *p_pwmm_instance - This is a pointer to the instance structure 
*            @ref pwmm_instance_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_TIMING - There is pending HSR not serviced yet.
*              The function call needs to be done again at a later time.
*          - @ref FS_ETPU_ERROR_NONE - No error
*
*******************************************************************************/
uint32_t fs_etpu_pwmm_enable(
  struct pwmm_instance_t *p_pwmm_instance)
{
  uint32_t error = FS_ETPU_ERROR_NONE;

  /* Write HSR */
  if(eTPU->CHAN[p_pwmm_instance->chan_num_master].HSRR.R == 0)
  {
    eTPU->CHAN[p_pwmm_instance->chan_num_master].HSRR.R = FS_ETPU_PWMM_HSR_ENABLE;
  }
  else
  {
    error = FS_ETPU_ERROR_TIMING;
  }  

  return(error);
}

/*******************************************************************************
* FUNCTION: fs_etpu_pwmm_disable
****************************************************************************//*!
* @brief   This function disables the PWM outputs, 
*          setting them all to the inactive polarity.
*
* @note    The following actions are performed in order:
*          -# Check there is no HSR pending.
*          -# Set HSR to disable the PWM outputs.
*
* @param   *p_pwmm_instance - This is a pointer to the instance structure 
*            @ref pwmm_instance_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_TIMING - There is pending HSR not serviced yet.
*              The function call needs to be done again at a later time.
*          - @ref FS_ETPU_ERROR_NONE - No error
*
*******************************************************************************/
uint32_t fs_etpu_pwmm_disable(
  struct pwmm_instance_t *p_pwmm_instance)
{
  uint32_t error = FS_ETPU_ERROR_NONE;

  /* Write HSR */
  if(eTPU->CHAN[p_pwmm_instance->chan_num_master].HSRR.R == 0)
  {
    eTPU->CHAN[p_pwmm_instance->chan_num_master].HSRR.R = FS_ETPU_PWMM_HSR_DISABLE;
  }
  else
  {
    error = FS_ETPU_ERROR_TIMING;
  }  

  return(error);
}

/*******************************************************************************
* FUNCTION: fs_etpu_pwmm_get_states
****************************************************************************//*!
* @brief   This function reads state parameter values to the PWMM function.
*
* @note    The following actions are performed in order:
*          -# Read output parameter values from eTPU DATA RAM
*
* @param   *p_pwmm_instance - This is a pointer to the instance structure 
*            @ref pwmm_instance_t.
* @param   *p_pwmm_states - This is a pointer to the structure of states
*            @ref pwmm_states_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_NONE - No error
*
*******************************************************************************/
uint32_t fs_etpu_pwmm_get_states(
  struct pwmm_instance_t *p_pwmm_instance,
  struct pwmm_states_t   *p_pwmm_states)
{
  uint32_t *cpba;

  cpba = p_pwmm_instance->cpba;                       
  
  /* Read channel parameters */
  p_pwmm_states->sector = *((uint8_t*)cpba + FS_ETPU_PWMM_OFFSET_SECTOR);
  p_pwmm_states->state  = *((uint8_t*)cpba + FS_ETPU_PWMM_OFFSET_STATE);
  p_pwmm_states->update = *((uint8_t*)cpba + FS_ETPU_PWMM_OFFSET_UPDATE);

  p_pwmm_states->missing_update_cnt = *(cpba + ((FS_ETPU_PWMM_OFFSET_MISSING_UPDATE_CNT - 1)>>2)) & 0xFFFFFF;
  p_pwmm_states->duty_a             = *(cpba + ((FS_ETPU_PWMM_OFFSET_PHASEA_DUTY - 1)>>2)) & 0xFFFFFF;
  p_pwmm_states->duty_b             = *(cpba + ((FS_ETPU_PWMM_OFFSET_PHASEB_DUTY - 1)>>2)) & 0xFFFFFF;
  p_pwmm_states->duty_c             = *(cpba + ((FS_ETPU_PWMM_OFFSET_PHASEC_DUTY - 1)>>2)) & 0xFFFFFF;

  return(FS_ETPU_ERROR_NONE);
}

/*******************************************************************************
 *
 * REVISION HISTORY:
 *
 * FILE OWNER: Marketa Venclikova [b52107]
 *
 * Revision 1.2  2018/02/14  b52107 
 * - Added a condition ensuring that 0% or 100% duty cycle will not be generated
 * Revision 1.1  2018/01/08  b52107 
 * - Input a,b and c configuration removed from  fs_etpu_pwmm_config();
 * Revision 1.0  2012/05/17  r54529
 * Initial version of file.
 ******************************************************************************/
