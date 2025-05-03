/*******************************************************************************
*
*   (c) Copyright 2004-2016 Freescale Semiconductor, Inc.
*   Copyright 2017 - 2019 NXP
*   NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
*   in accordance with the applicable license terms. By expressly accepting
*   such terms or by downloading, installing, activating and/or otherwise using
*   the software, you are agreeing that you have read, and that you agree to
*   comply with and are bound by, such license terms. If you do not agree to
*   be bound by the applicable license terms, then you may not retain,
*   install, activate or otherwise use the software.
****************************************************************************//*!

* @file    etpu_resolver.c
*
* @author  Marketa Venclikova [nxa17216]
* 
* @version 1.75
* 
* @date    14-Nov-2019
*
* @brief   This file contains API for using the eTPU function
*          Resolver Interface (RESOLVER).
* 
****************************************************************************//*!
*
* @mainpage
*
* The eTPU RESOLVER APIs @ref etpu_resolver.c/.h includes API functions for eTPU
* function Resolver Interface.
* 
*******************************************************************************/
/*******************************************************************************
*                                        INCLUDE FILES
*******************************************************************************/
#include "etpu_resolver.h"    /* private header file */
#include "../_utils/etpu_util.h"        /* utility routines for working with the eTPU */

/*******************************************************************************
*                                      GLOBAL VARIABLES
*******************************************************************************/
extern uint32_t fs_etpu_data_ram_start;
extern uint32_t fs_etpu_data_ram_ext;

/*******************************************************************************
*                                       GLOBAL FUNCTIONS
*******************************************************************************/
/*******************************************************************************
* FUNCTION: fs_etpu_resolver_init
****************************************************************************//*!
* @brief   This function initializes eTPU channels to run RESOLVER function.
*
* @note    The following actions are performed in order:
*          -# Use user-defined CPBA or allocate new eTPU DATA RAM
*          -# Write chan config registers and FM bits
*          -# Write channel parameters
*          -# Write HSR
*          -# Set channel priority
*
* @param   *p_resolver_instance - This is a pointer to the instance structure 
*            @ref resolver_instance_t.
* @param   *p_resolver_config - This is a pointer to the structure of configuration
*            parameters @ref resolver_config_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_MALLOC - eTPU DATA RAM memory allocation error
*          - @ref FS_ETPU_ERROR_NONE - No error
*
* @warning This function does not configure the pins, only the eTPU channels.
*******************************************************************************/
uint32_t fs_etpu_resolver_init(
  struct resolver_instance_t *p_resolver_instance,
  struct resolver_config_t   *p_resolver_config)
{
  uint8_t  chan_num_exc;
  uint8_t  chan_num_diag;
  uint8_t  chan_num_sample;
  uint8_t  chan_num_ato;
  uint8_t  priority;
  uint32_t *cpba, *signals_pba;

  chan_num_exc  = p_resolver_instance->chan_num_exc;   
  chan_num_diag = p_resolver_instance->chan_num_diag;   
  chan_num_sample = p_resolver_instance->chan_num_sample;   
  chan_num_ato = p_resolver_instance->chan_num_ato;
  priority      = p_resolver_instance->priority;                
  cpba          = p_resolver_instance->cpba;
  signals_pba   = p_resolver_instance->signals_pba;                       

  /* Disable channel priorities */
  fs_etpu_enable(chan_num_exc, FS_ETPU_PRIORITY_DISABLE);
  fs_etpu_enable(chan_num_diag, FS_ETPU_PRIORITY_DISABLE);
  fs_etpu_enable(chan_num_sample, FS_ETPU_PRIORITY_DISABLE);
  
  /* Use user-defined CPBA or allocate new eTPU DATA RAM */
  if(cpba == 0)
  {
    cpba = fs_etpu_malloc(FS_ETPU_RESOLVER_NUM_PARMS);
    if(cpba == 0)
    {
      return(FS_ETPU_ERROR_MALLOC);
    }
    else
    {
      p_resolver_instance->cpba = cpba;
    }
  }

  /* Use user-defined signals_pba or allocate new eTPU DATA RAM */
  if(signals_pba == 0)
  {
    signals_pba = fs_etpu_malloc(3*32*sizeof(uint32_t));
    if(signals_pba == 0)
    {
      return(FS_ETPU_ERROR_MALLOC);
    }
    else
    {
      p_resolver_instance->signals_pba = signals_pba;
    }
  }
  
  if(p_resolver_config->options & FS_ETPU_RESOLVER_OPTIONS_RDC_CHECKER_ON)
  {
	  p_resolver_instance->checker_signals_pba = p_resolver_instance->cpba + (FS_ETPU_RESOLVER_OFFSET_RDC_CHECKER>>2);
  }


  /* Write chan config registers and FM bits */
  eTPU->CHAN[chan_num_exc].CR.R = (FS_ETPU_RESOLVER_TABLE_SELECT << 24) +
                                  (FS_ETPU_RESOLVER_FUNCTION_NUMBER << 16) +
                                  (((uint32_t)cpba - fs_etpu_data_ram_start) >> 3);
  eTPU->CHAN[chan_num_exc].SCR.R = FS_ETPU_RESOLVER_FM0_EXC;
  eTPU->CHAN[chan_num_ato].CR.R = (FS_ETPU_RESOLVER_TABLE_SELECT << 24) +
                                  (FS_ETPU_RESOLVER_FUNCTION_NUMBER << 16) +
                                  (((uint32_t)cpba - fs_etpu_data_ram_start) >> 3);
  eTPU->CHAN[chan_num_ato].SCR.R = 0;
  eTPU->CHAN[chan_num_diag].CR.R = (FS_ETPU_RESOLVER_TABLE_SELECT << 24) +
                                   (FS_ETPU_RESOLVER_FUNCTION_NUMBER << 16) +
                                   (((uint32_t)cpba - fs_etpu_data_ram_start) >> 3);
  eTPU->CHAN[chan_num_diag].SCR.R = FS_ETPU_RESOLVER_FM0_DIAG;
  eTPU->CHAN[chan_num_sample].CR.R = (FS_ETPU_RESOLVER_TABLE_SELECT << 24) +
                                  (FS_ETPU_RESOLVER_FUNCTION_NUMBER << 16) +
                                  (((uint32_t)cpba - fs_etpu_data_ram_start) >> 3);
  eTPU->CHAN[chan_num_sample].SCR.R = FS_ETPU_RESOLVER_FM1_TRIG_RISING;
  


  /* Write channel parameters */

  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_START_OFFSET         - 1)>>2)) = p_resolver_instance->start_offset;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_ADC_DELAY            - 1)>>2)) = p_resolver_instance->adc_delay;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_CURRENT_TIME         - 1)>>2)) = 0;  
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_SIGNALS              - 1)>>2)) = (uint32_t)signals_pba - fs_etpu_data_ram_start; 
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_EXC_FRAME_TIME       - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_EXC_PERIOD           - 1)>>2)) = p_resolver_config->excitation_period;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_EXC_PHASE_SHIFT      - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_EXC_ZERO_CROSS_ERR_SIN-1)>>2)) = 0; 
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_EXC_ZERO_CROSS_ERR_COS-1)>>2)) = 0; 
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_EXC_ZERO_CROSS_ERR   - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_EXC_PI_ACC           - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_EXC_PI_GAIN_P        - 1)>>2)) = p_resolver_config->exc_p_gain;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_EXC_PI_GAIN_I        - 1)>>2)) = p_resolver_config->exc_i_gain; 
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_ATO_PI_GAIN_P        - 1)>>2)) = p_resolver_config->ato_p_gain;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_ATO_PI_GAIN_I        - 1)>>2)) = p_resolver_config->ato_i_gain;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_ATO_Q_EWMA           - 1)>>2)) = p_resolver_config->q_ewma_speed;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_ATO_SIG_SIN          - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_ATO_SIG_COS          - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_ATO_PI_ACC           - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_ATO_ANGLE_ERROR      - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_ATO_ANGLE            - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_ATO_SPEED            - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_CALCULATED_ANGLE         - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_CALCULATED_SPEED         - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_CALCULATED_REV_COUNTER   - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_CALCULATED_SIN           - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_CALCULATED_COS           - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_CALCULATED_TIMESTAMP     - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_EXTRAPOLATED_ANGLE       - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_EXTRAPOLATED_SPEED       - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_EXTRAPOLATED_REV_COUNTER - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_EXTRAPOLATED_SIN         - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_EXTRAPOLATED_COS         - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_EXTRAPOLATED_TIMESTAMP   - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_AMPL_THRS          - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_MEAN_THRS          - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_VEC_MIN_THRS       - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_VEC_MAX_THRS       - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_AMPL_DIFF_THRS     - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_ATO_ANGLE_ERR_THRS - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_ATO_SPEED_THRS     - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_SIN_AMPL_1   - 1)>>2)) = 0; 
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_SIN_AMPL_2   - 1)>>2)) = 0; 
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_SIN_AMPL_MIN - 1)>>2)) = 0; 
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_SIN_AMPL_MAX - 1)>>2)) = 0; 
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_SIN_MEAN_MIN - 1)>>2)) = 0; 
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_SIN_MEAN_MAX - 1)>>2)) = 0; 
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_COS_AMPL_1   - 1)>>2)) = 0; 
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_COS_AMPL_2   - 1)>>2)) = 0; 
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_COS_AMPL_MIN - 1)>>2)) = 0; 
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_COS_AMPL_MAX - 1)>>2)) = 0; 
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_COS_MEAN_MIN - 1)>>2)) = 0; 
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_COS_MEAN_MAX - 1)>>2)) = 0; 
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_VEC_MIN      - 1)>>2)) = 0x7FFFFF;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_VEC_MAX      - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_AMPL_DIFF    - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_FLAGS_ACTUAL     - 1)>>2)) = 0;  
  *(cpba + ((FS_ETPU_RESOLVER_OFFSET_DIAG_FLAGS_CUMULATIVE - 1)>>2)) = 0;  

  *((uint8_t*)cpba + FS_ETPU_RESOLVER_OFFSET_DMA_CHAN             ) = p_resolver_instance->chan_num_dma;
  *((uint8_t*)cpba + FS_ETPU_RESOLVER_OFFSET_EXC_LINK_CHAN             ) = FS_ETPU_CHANNEL_TO_LINK(p_resolver_instance->chan_num_exc);
  *((uint8_t*)cpba + FS_ETPU_RESOLVER_OFFSET_SMPR_ID              ) = p_resolver_config->smpr_id;
  *((uint8_t*)cpba + FS_ETPU_RESOLVER_OFFSET_OPTIONS              ) = p_resolver_config->options;
  *((uint8_t*)cpba + FS_ETPU_RESOLVER_OFFSET_DIAG_LINK_CHAN       ) = FS_ETPU_CHANNEL_TO_LINK(chan_num_diag);
  *((uint8_t*)cpba + FS_ETPU_RESOLVER_OFFSET_UPD                  ) = 0;
  *((uint8_t*)cpba + FS_ETPU_RESOLVER_OFFSET_CALCULATED_SEMAPHORE  ) = FS_ETPU_RESOLVER_SEMAPHORE_AVAILABLE;
  *((uint8_t*)cpba + FS_ETPU_RESOLVER_OFFSET_EXTRAPOLATED_SEMAPHORE) = FS_ETPU_RESOLVER_SEMAPHORE_AVAILABLE;
  
  /* Write HSR */  
  eTPU->CHAN[chan_num_exc].HSRR.R = FS_ETPU_RESOLVER_HSR_INIT_EXC;
  eTPU->CHAN[chan_num_ato].HSRR.R = FS_ETPU_RESOLVER_HSR_INIT_ATO;
  eTPU->CHAN[chan_num_diag].HSRR.R = FS_ETPU_RESOLVER_HSR_INIT_DIAG;
  eTPU->CHAN[chan_num_sample].HSRR.R = FS_ETPU_RESOLVER_HSR_INIT_SAMPLE;
  
  /* Set channel priority */
  fs_etpu_enable(chan_num_exc, priority);
  fs_etpu_enable(chan_num_diag, priority);
  fs_etpu_enable(chan_num_sample, priority);
  fs_etpu_enable(chan_num_ato, priority);

  return(FS_ETPU_ERROR_NONE);
}

/*******************************************************************************
* FUNCTION: fs_etpu_resolver_config
****************************************************************************//*!
* @brief   This function changes the RESOLVER configuration.
*
* @note    The following actions are performed in order:
*          -# Write configuration parameter values to eTPU DATA RAM
*
* @param   *p_resolver_instance - This is a pointer to the instance structure 
*            @ref resolver_instance_t.
* @param   *p_resolver_config - This is a pointer to the structure of configuration
*            parameters @ref resolver_config_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_NONE - No error
*
*******************************************************************************/
uint32_t fs_etpu_resolver_config(
  struct resolver_instance_t *p_resolver_instance,
  struct resolver_config_t   *p_resolver_config)
{
  uint32_t *cpba;
  uint32_t *cpbae;

  cpba = p_resolver_instance->cpba;                       
  cpbae = cpba + (0x4000 >> 2); /* sign-extended memory area */
  
  /* Write channel parameters - use cpbae to prevent from overwriting bits 31:24 */
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_EXC_PERIOD           - 1)>>2)) = p_resolver_config->excitation_period;
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_EXC_PI_GAIN_P        - 1)>>2)) = p_resolver_config->exc_p_gain;
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_EXC_PI_GAIN_I        - 1)>>2)) = p_resolver_config->exc_i_gain; 
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_ATO_PI_GAIN_P        - 1)>>2)) = p_resolver_config->ato_p_gain;
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_ATO_PI_GAIN_I        - 1)>>2)) = p_resolver_config->ato_i_gain; 
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_ATO_Q_EWMA           - 1)>>2)) = p_resolver_config->q_ewma_speed;
  *((uint8_t*)cpba + FS_ETPU_RESOLVER_OFFSET_OPTIONS               ) = p_resolver_config->options;
  *((uint8_t*)cpba + FS_ETPU_RESOLVER_OFFSET_SMPR_ID               ) = p_resolver_config->smpr_id;

  return(FS_ETPU_ERROR_NONE);
}

/*******************************************************************************
* FUNCTION: fs_etpu_resolver_get_outputs_calculated
****************************************************************************//*!
* @brief   This function reads the motor angular position and speed, 
*          together with the motor angle sin and cos, as calculated by the last
*          update of the Angle Tracking Observer (ATO). The ATO outputs
*          corresponds to the middle of the last sin/cos signal half-period.  
*
* @note    The following actions are performed in order:
*          -# Read output parameter values from eTPU DATA RAM
*
* @param   *p_resolver_instance - This is a pointer to the instance structure 
*            @ref resolver_instance_t.
* @param   *p_resolver_outputs_calculated - This is a pointer to the structure 
*            of outputs @ref resolver_outputs_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_NONE - No error
*          - @ref FS_ETPU_ERROR_TIMING - Data just being updated, 
*                                        repeat function call again.
*
*******************************************************************************/
uint32_t fs_etpu_resolver_get_outputs_calculated(
  struct resolver_instance_t *p_resolver_instance,
  struct resolver_outputs_t  *p_resolver_outputs_calculated)
{
  uint32_t *cpba;
  uint32_t *cpbae;

  cpba = p_resolver_instance->cpba;                       
  cpbae = cpba + (0x4000 >> 2); /* sign-extended memory area */

  /* Check semaphore */
  if(*((uint8_t*)cpba + FS_ETPU_RESOLVER_OFFSET_CALCULATED_SEMAPHORE) == FS_ETPU_RESOLVER_SEMAPHORE_DO_NOT_READ)
  {
    return(FS_ETPU_ERROR_TIMING);
  }
  
  /* Read channel parameters */
  p_resolver_outputs_calculated->angle = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_CALCULATED_ANGLE - 1)>>2));
  p_resolver_outputs_calculated->speed = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_CALCULATED_SPEED - 1)>>2));
  p_resolver_outputs_calculated->rev_counter = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_CALCULATED_REV_COUNTER - 1)>>2));
  p_resolver_outputs_calculated->sin   = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_CALCULATED_SIN - 1)>>2));
  p_resolver_outputs_calculated->cos   = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_CALCULATED_COS - 1)>>2));
  p_resolver_outputs_calculated->timestamp = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_CALCULATED_TIMESTAMP - 1)>>2));
  
  return(FS_ETPU_ERROR_NONE);
}

/*******************************************************************************
* FUNCTION: fs_etpu_resolver_get_outputs_extrapolated
****************************************************************************//*!
* @brief   This function reads the motor angular position and speed, 
*          together with the motor angle sin and cos, extrapolated 
*          by the extrapolation time. 
*          The ATO outputs, which corresponds to the middle of the last sin/cos
*          signal half-period, are extrapolated so that the results corresponds
*          to a position defined by the extrapolation time.
*          The extrapolation can be used for example to align the motor position
*          reading with the phase currents reading.
*
* @note    The following actions are performed in order:
*          -# Read output parameter values from eTPU DATA RAM
*
* @param   *p_resolver_instance - This is a pointer to the instance structure 
*            @ref resolver_instance_t.
* @param   *p_resolver_outputs_extrapolated - This is a pointer to the structure 
*            of outputs @ref resolver_outputs_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_NONE - No error
*          - @ref FS_ETPU_ERROR_TIMING - Data just being updated, 
*                                        repeat function call again.
*
*******************************************************************************/
uint32_t fs_etpu_resolver_get_outputs_extrapolated(
  struct resolver_instance_t *p_resolver_instance,
  struct resolver_outputs_t  *p_resolver_outputs_extrapolated)
{
  uint32_t *cpba;
  uint32_t *cpbae;

  cpba = p_resolver_instance->cpba;                       
  cpbae = cpba + (0x4000 >> 2); /* sign-extended memory area */

  /* Check semaphore */
  if(*((uint8_t*)cpba + FS_ETPU_RESOLVER_OFFSET_EXTRAPOLATED_SEMAPHORE) == FS_ETPU_RESOLVER_SEMAPHORE_DO_NOT_READ)
  {
    return(FS_ETPU_ERROR_TIMING);
  }

  /* Read channel parameters */
  p_resolver_outputs_extrapolated->angle = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_EXTRAPOLATED_ANGLE - 1)>>2));
  p_resolver_outputs_extrapolated->speed = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_EXTRAPOLATED_SPEED - 1)>>2));
  p_resolver_outputs_extrapolated->rev_counter = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_EXTRAPOLATED_REV_COUNTER - 1)>>2));
  p_resolver_outputs_extrapolated->sin   = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_EXTRAPOLATED_SIN - 1)>>2));
  p_resolver_outputs_extrapolated->cos   = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_EXTRAPOLATED_COS - 1)>>2));
  p_resolver_outputs_extrapolated->timestamp = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_EXTRAPOLATED_TIMESTAMP - 1)>>2));
  
  return(FS_ETPU_ERROR_NONE);
}

/*******************************************************************************
* FUNCTION: fs_etpu_resolver_trans_outputs_el_to_mech
****************************************************************************//*!
*
*******************************************************************************/
uint32_t fs_etpu_resolver_trans_outputs_el_to_mech(
  int32_t pole_pairs, //resolver pole pairs
  struct resolver_outputs_t  *p_resolver_outputs_el,
  struct resolver_outputs_t  *p_resolver_outputs_mech)
{
  p_resolver_outputs_mech->angle = (p_resolver_outputs_el->angle + (fract24_t) 0x1000000 * (p_resolver_outputs_el->rev_counter % pole_pairs)) / pole_pairs;
  //p_resolver_outputs_mech->angle = (p_resolver_outputs_el->angle + (p_resolver_outputs_el->rev_counter % pole_pairs) * 0x1000000) / pole_pairs;
  p_resolver_outputs_mech->speed = p_resolver_outputs_el->speed / pole_pairs;
  p_resolver_outputs_mech->rev_counter = p_resolver_outputs_el->rev_counter / pole_pairs;
  
  return(FS_ETPU_ERROR_NONE);
}

/*******************************************************************************
* FUNCTION: fs_etpu_resolver_sample
****************************************************************************//*!
* @brief   This function triggers the extrapolation. As soon as the Sample
*          channel HSR register clears to zero, the extrapolated outputs 
*          are available.
*
* @note    The following actions are performed in order:
*          -# Write HSR
*
* @param   *p_resolver_instance - This is a pointer to the instance structure 
*            @ref resolver_instance_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_TIMING - Another HSR is being processed. 
*            Wait a couple of microseconds and call this function again.
*          - @ref FS_ETPU_ERROR_NONE - No error
*
*******************************************************************************/
uint32_t fs_etpu_resolver_sample(
  struct resolver_instance_t      *p_resolver_instance)
{
  uint8_t  chan_num_sample;

  chan_num_sample = p_resolver_instance->chan_num_sample;   

  /* Write HSR */
  if(eTPU->CHAN[chan_num_sample].HSRR.R != 0)
  {
    return(FS_ETPU_ERROR_TIMING);
  }
  else
  {
    eTPU->CHAN[chan_num_sample].HSRR.R = FS_ETPU_RESOLVER_HSR_SAMPLE;
  }  

  return(FS_ETPU_ERROR_NONE);
}

/*******************************************************************************
* FUNCTION: fs_etpu_resolver_get_diag_measures
****************************************************************************//*!
* @brief   This function reads the Resolver Diagnostic Measures. 
*
* @note    The following actions are performed in order:
*          -# Read output parameter values from eTPU DATA RAM
*
* @param   *p_resolver_instance - This is a pointer to the instance structure 
*            @ref resolver_instance_t.
* @param   *p_resolver_diag_measures - This is a pointer to the structure 
*            of measures @ref resolver_diag_measures_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_NONE - No error
*
*******************************************************************************/
uint32_t fs_etpu_resolver_get_diag_measures(
  struct resolver_instance_t      *p_resolver_instance,
  struct resolver_diag_measures_t *p_resolver_diag_measures)
{
  uint32_t *cpba;
  uint32_t *cpbae;

  cpba = p_resolver_instance->cpba;                       
  cpbae = cpba + (0x4000 >> 2); /* sign-extended memory area */

  /* Read channel parameters */
  p_resolver_diag_measures->sin_ampl_1   = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_SIN_AMPL_1 - 1)>>2));
  p_resolver_diag_measures->sin_ampl_2   = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_SIN_AMPL_2 - 1)>>2));
  p_resolver_diag_measures->sin_ampl_min = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_SIN_AMPL_MIN - 1)>>2));
  p_resolver_diag_measures->sin_ampl_max = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_SIN_AMPL_MAX - 1)>>2));
  p_resolver_diag_measures->sin_mean     = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_SIN_MEAN - 1)>>2));
  p_resolver_diag_measures->sin_mean_min = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_SIN_MEAN_MIN - 1)>>2));
  p_resolver_diag_measures->sin_mean_max = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_SIN_MEAN_MAX - 1)>>2));
  p_resolver_diag_measures->cos_ampl_1   = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_COS_AMPL_1 - 1)>>2));
  p_resolver_diag_measures->cos_ampl_2   = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_COS_AMPL_2 - 1)>>2));
  p_resolver_diag_measures->cos_ampl_min = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_COS_AMPL_MIN - 1)>>2));
  p_resolver_diag_measures->cos_ampl_max = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_COS_AMPL_MAX - 1)>>2));
  p_resolver_diag_measures->cos_mean     = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_COS_MEAN - 1)>>2));
  p_resolver_diag_measures->cos_mean_min = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_COS_MEAN_MIN - 1)>>2));
  p_resolver_diag_measures->cos_mean_max = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_COS_MEAN_MAX - 1)>>2));
  p_resolver_diag_measures->vec          = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_VEC - 1)>>2));
  p_resolver_diag_measures->vec_min      = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_VEC_MIN - 1)>>2));
  p_resolver_diag_measures->vec_max      = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_VEC_MAX - 1)>>2));
  p_resolver_diag_measures->ampl_diff    = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_MEASURES_AMPL_DIFF - 1)>>2));

  return(FS_ETPU_ERROR_NONE);
}

/*******************************************************************************
* FUNCTION: fs_etpu_resolver_get_diag_flags
****************************************************************************//*!
* @brief   This function reads the signals diagnostic flags.  
*
* @note    The following actions are performed in order:
*          -# Read output parameter values from eTPU DATA RAM
*
* @param   *p_resolver_instance - This is a pointer to the instance structure 
*            @ref resolver_instance_t.
* @param   *p_resolver_diag_flags - This is a pointer to the structure 
*            of diagnostic flags @ref resolver_diag_flags_t. 
*          The list of possible flags includes:
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_SIN_AMPL_MIN 
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_SIN_AMPL_MAX 
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_COS_AMPL_MIN 
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_COS_AMPL_MAX 
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_SIN_MEAN     
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_COS_MEAN     
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_VEC_MIN      
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_VEC_MAX      
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_AMPL_DIFF    
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_ATO_ANGLE_ERR
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_ATO_SPEED    
*                 
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_NONE - No error
*
*******************************************************************************/
uint32_t fs_etpu_resolver_get_diag_flags(
  struct resolver_instance_t   *p_resolver_instance,
  struct resolver_diag_flags_t *p_resolver_diag_flags)
{
  uint32_t *cpba;
  uint32_t *cpbae;

  cpba = p_resolver_instance->cpba;                       
  cpbae = cpba + (0x4000 >> 2); /* sign-extended memory area */

  /* Read channel parameters */
  p_resolver_diag_flags->actual     = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_FLAGS_ACTUAL - 1)>>2));
  p_resolver_diag_flags->cumulative = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_FLAGS_CUMULATIVE - 1)>>2));
  
  return(FS_ETPU_ERROR_NONE);
}

/*******************************************************************************
* FUNCTION: fs_etpu_resolver_decode_diag_flags_basic
****************************************************************************//*!
* @brief   This function decodes diagnostic flags into advanced flags.  
*
* @note    The following actions are performed in order:
*          -# Decode the advanced diagnostic flags.
*
* @param   *p_resolver_diag_flags - This is a pointer to the structure 
*            of diagnostic flags @ref resolver_diag_flags_t. 
*
* @return  A byte including decoded basic diagnostic flags.
*          The list of possible flags includes:
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_BASIC_LOS
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_BASIC_DOS
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_BASIC_LOT
*
*******************************************************************************/
uint8_t fs_etpu_resolver_decode_diag_flags_basic(
  struct resolver_diag_flags_t *p_resolver_diag_flags)
{
  uint8_t   ret;
  uint24_t  flags;
  
  ret = 0;
  flags = p_resolver_diag_flags->cumulative;
  
  if(flags & FS_ETPU_RESOLVER_DIAG_FLAG_BASIC_LOS_MASK) 
  {
    ret |= FS_ETPU_RESOLVER_DIAG_FLAG_BASIC_LOS;
  }
  if(flags & FS_ETPU_RESOLVER_DIAG_FLAG_BASIC_DOS_MASK) 
  {
    ret |= FS_ETPU_RESOLVER_DIAG_FLAG_BASIC_DOS;
  }
  if(flags & FS_ETPU_RESOLVER_DIAG_FLAG_BASIC_LOT_MASK) 
  {
    ret |= FS_ETPU_RESOLVER_DIAG_FLAG_BASIC_LOT;
  }
  
  return(ret);
}

/*******************************************************************************
* FUNCTION: fs_etpu_resolver_decode_diag_flags_advanced
****************************************************************************//*!
* @brief   This function decodes diagnostic flags into advanced flags.  
*
* @note    The following actions are performed in order:
*          -# Decode the advanced diagnostic flags.
*
* @param   *p_resolver_diag_flags - This is a pointer to the structure 
*            of diagnostic flags @ref resolver_diag_flags_t. 
*
* @return  A byte including decoded advanced diagnostic flags.
*          The list of possible flags includes:
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_ADV_SIN_DISCONNECT
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_ADV_SIN_SHORT
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_ADV_COS_DISCONNECT
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_ADV_COS_SHORT
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_ADV_EXC_DISCONNECT
*          - @ref FS_ETPU_RESOLVER_DIAG_FLAG_ADV_EXC_SHORT
*
*******************************************************************************/
uint8_t fs_etpu_resolver_decode_diag_flags_advanced(
  struct resolver_diag_flags_t *p_resolver_diag_flags)
{
  uint8_t   ret;
  uint24_t  flags;
  
  flags = p_resolver_diag_flags->cumulative;
  
  if((flags & FS_ETPU_RESOLVER_DIAG_FLAG_ADV_EXC_DISC_MASK) == FS_ETPU_RESOLVER_DIAG_FLAG_ADV_EXC_DISC_MASK)
  {
    ret = FS_ETPU_RESOLVER_DIAG_FLAG_ADV_EXC_DISCONNECT;
  }
  else if((flags & FS_ETPU_RESOLVER_DIAG_FLAG_ADV_SIN_DISC_MASK) == FS_ETPU_RESOLVER_DIAG_FLAG_ADV_SIN_DISC_MASK)
  {
    ret = FS_ETPU_RESOLVER_DIAG_FLAG_ADV_SIN_DISCONNECT;
  }
  else if((flags & FS_ETPU_RESOLVER_DIAG_FLAG_ADV_COS_DISC_MASK) == FS_ETPU_RESOLVER_DIAG_FLAG_ADV_COS_DISC_MASK) 
  {
    ret = FS_ETPU_RESOLVER_DIAG_FLAG_ADV_COS_DISCONNECT;
  }
  else if((flags & FS_ETPU_RESOLVER_DIAG_FLAG_ADV_EXC_SHORT_MASK) == FS_ETPU_RESOLVER_DIAG_FLAG_ADV_EXC_SHORT_MASK)
  {
    ret = FS_ETPU_RESOLVER_DIAG_FLAG_ADV_EXC_SHORT;
  }
  else if((flags & FS_ETPU_RESOLVER_DIAG_FLAG_ADV_SIN_SHORT_MASK) == FS_ETPU_RESOLVER_DIAG_FLAG_ADV_SIN_SHORT_MASK) 
  {
    ret = FS_ETPU_RESOLVER_DIAG_FLAG_ADV_SIN_SHORT;
  }
  else if((flags & FS_ETPU_RESOLVER_DIAG_FLAG_ADV_COS_SHORT_MASK) == FS_ETPU_RESOLVER_DIAG_FLAG_ADV_COS_SHORT_MASK) 
  {
    ret = FS_ETPU_RESOLVER_DIAG_FLAG_ADV_COS_SHORT;
  }
  else
  {
    ret = 0;
  }
  
  return(ret);
}

/*******************************************************************************
* FUNCTION: fs_etpu_resolver_diag_reset
****************************************************************************//*!
* @brief   This function resets Resolver diagnostic measures. 
*
* @note    The following actions are performed in order:
*          -# Write HSR
*
* @param   *p_resolver_instance - This is a pointer to the instance structure 
*            @ref resolver_instance_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_TIMING - Another HSR is being processed. 
*            Wait a couple of microseconds and call this function again.
*          - @ref FS_ETPU_ERROR_NONE - No error
*
*******************************************************************************/
uint32_t fs_etpu_resolver_diag_reset(
  struct resolver_instance_t      *p_resolver_instance)
{
  uint8_t  chan_num_diag;

  chan_num_diag = p_resolver_instance->chan_num_diag;   

  /* Write HSR */
  if(eTPU->CHAN[chan_num_diag].HSRR.R != 0)
  {
    return(FS_ETPU_ERROR_TIMING);
  }
  else
  {
    eTPU->CHAN[chan_num_diag].HSRR.R = FS_ETPU_RESOLVER_HSR_DIAG_RESET;
  }  

  return(FS_ETPU_ERROR_NONE);
}

/*******************************************************************************
* FUNCTION: fs_etpu_resolver_set_thresholds
****************************************************************************//*!
* @brief   This function sets Resolver diagnostic thresholds.
*          For a proper operation, this must be called when the following is
*          satisfied:
*          - there is no fault on the resolver signals
*          - at least one mechanical revolution was passed from initialization
*            of from the last call of @ref fs_etpu_resolver_diag_reset. 
*
* @note    The following actions are performed in order:
*          -# Calculate thresholds
*          -# Write channel parameters
*          The values of
*          - ato_angle_err_max_thrs, and
*          - ato_speed_max_thrs
*          are not modified. 
*          The other values are updated according to the signal measurement
*          acquired from the last fs_etpu_resolver_diag_reset() call. 
*
* @param   *p_resolver_instance - This is a pointer to the instance structure 
*            @ref resolver_instance_t.
* @param   *p_resolver_diag_measures - This is a pointer to the structure 
*            of diagnostic measures @ref resolver_diag_measures_t.
* @param   *p_resolver_thresholds - This is a pointer to the structure of 
*            threshold values @ref resolver_thresholds_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_NONE - No error
*
*******************************************************************************/
uint32_t fs_etpu_resolver_set_thresholds(
  struct resolver_instance_t   *p_resolver_instance,
  struct resolver_diag_thresholds_t *p_resolver_diag_thresholds)

{
  uint32_t *cpba;
  uint32_t *cpbae;

  cpba = p_resolver_instance->cpba;                       
  cpbae = cpba + (0x4000 >> 2); /* sign-extended memory area */

  /* Write channel parameters */
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_AMPL_THRS          - 1)>>2)) = p_resolver_diag_thresholds->ampl_thrs; 
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_LOW_AMPL_THRS      - 1)>>2)) = p_resolver_diag_thresholds->low_ampl_thrs;
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_MEAN_THRS          - 1)>>2)) = p_resolver_diag_thresholds->mean_thrs;
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_VEC_MIN_THRS       - 1)>>2)) = p_resolver_diag_thresholds->vec_min_thrs; 
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_VEC_MAX_THRS       - 1)>>2)) = p_resolver_diag_thresholds->vec_max_thrs; 
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_AMPL_DIFF_THRS     - 1)>>2)) = p_resolver_diag_thresholds->ampl_diff_thrs;
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_ATO_ANGLE_ERR_THRS - 1)>>2)) = p_resolver_diag_thresholds->ato_angle_err_thrs;
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_ATO_SPEED_THRS     - 1)>>2)) = p_resolver_diag_thresholds->ato_speed_thrs;

  return(FS_ETPU_ERROR_NONE);
}
/*******************************************************************************
* FUNCTION: fs_etpu_resolver_diag_calibrate
****************************************************************************//*!
* @brief   This function calculates and sets Resolver diagnostic thresholds
*          according to the available diagnostic measures.
*          For a proper operation, this must be called when the following is
*          satisfied:
*          - there is no fault on the resolver signals
*          - at least one mechanical revolution was passed from initialization
*            of from the last call of @ref fs_etpu_resolver_diag_reset. 
*          The values of
*          - ato_angle_err_max_thrs, and
*          - ato_speed_max_thrs
*          are not modified. 
*
* @note    The following actions are performed in order:
*          -# Calculate thresholds
*          -# Write channel parameters
*
* @param   *p_resolver_instance - This is a pointer to the instance structure 
*            @ref resolver_instance_t.
* @param   *p_resolver_diag_measures - This is a pointer to the structure 
*            of diagnostic measures @ref resolver_diag_measures_t.
* @param   *p_resolver_thresholds - This is a pointer to the structure of 
*            threshold values @ref resolver_thresholds_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_NONE - No error
*
*******************************************************************************/
uint32_t fs_etpu_resolver_diag_calibrate(
  struct resolver_instance_t        *p_resolver_instance,
  struct resolver_diag_measures_t   *p_resolver_diag_measures,
  struct resolver_diag_thresholds_t *p_resolver_diag_thresholds)
{
  uint32_t *cpba;
  uint32_t *cpbae;

  int32_t sin_ampl_min, sin_ampl_max;
  int32_t cos_ampl_min, cos_ampl_max;
  int32_t sin_mean_min, sin_mean_max;
  int32_t cos_mean_min, cos_mean_max;
  int32_t vec_min, vec_max, ampl_diff;
  int32_t tmp;

  cpba = p_resolver_instance->cpba;                       
  cpbae = cpba + (0x4000 >> 2); /* sign-extended memory area */

  /* Calculate thresholds */
  sin_ampl_min = p_resolver_diag_measures->sin_ampl_min; 
  sin_ampl_max = p_resolver_diag_measures->sin_ampl_max; 
  cos_ampl_min = p_resolver_diag_measures->cos_ampl_min; 
  cos_ampl_max = p_resolver_diag_measures->cos_ampl_max; 
  sin_mean_min = p_resolver_diag_measures->sin_mean_min; 
  sin_mean_max = p_resolver_diag_measures->sin_mean_max; 
  cos_mean_min = p_resolver_diag_measures->cos_mean_min; 
  cos_mean_max = p_resolver_diag_measures->cos_mean_max; 
  vec_min      = p_resolver_diag_measures->vec_min;
  vec_max      = p_resolver_diag_measures->vec_max;
  ampl_diff    = p_resolver_diag_measures->ampl_diff;

  tmp = -sin_ampl_min;
  tmp = -cos_ampl_min > tmp ? -cos_ampl_min : tmp;
  tmp = sin_ampl_max > tmp ? sin_ampl_max : tmp;;
  tmp = cos_ampl_max > tmp ? cos_ampl_max : tmp;
  p_resolver_diag_thresholds->ampl_thrs = tmp * 110/100;
  p_resolver_diag_thresholds->low_ampl_thrs = tmp * 25/100;
  tmp = -sin_mean_min;
  tmp = -cos_mean_min > tmp ? -cos_mean_min : tmp;
  tmp =  sin_mean_max > tmp ?  sin_mean_max : tmp;
  tmp =  cos_mean_max > tmp ?  cos_mean_max : tmp;
  p_resolver_diag_thresholds->mean_thrs = tmp * 150/100;
  p_resolver_diag_thresholds->vec_min_thrs = vec_min * 80/100;
  p_resolver_diag_thresholds->vec_max_thrs = vec_max * 120/100;
  p_resolver_diag_thresholds->ampl_diff_thrs  = ampl_diff * 150/100;

  /* Write channel parameters */
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_AMPL_THRS       - 1)>>2)) = p_resolver_diag_thresholds->ampl_thrs; 
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_LOW_AMPL_THRS   - 1)>>2)) = p_resolver_diag_thresholds->low_ampl_thrs;
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_MEAN_THRS       - 1)>>2)) = p_resolver_diag_thresholds->mean_thrs;
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_VEC_MIN_THRS    - 1)>>2)) = p_resolver_diag_thresholds->vec_min_thrs; 
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_VEC_MAX_THRS    - 1)>>2)) = p_resolver_diag_thresholds->vec_max_thrs; 
  *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_DIAG_THRESHOLDS_AMPL_DIFF_THRS  - 1)>>2)) = p_resolver_diag_thresholds->ampl_diff_thrs;

  return(FS_ETPU_ERROR_NONE);
}
/*******************************************************************************
* FUNCTION: fs_etpu_resolver_get_states
****************************************************************************//*!
* @brief   This function reads the Resolver eTPU function states. 
*
* @note    The following actions are performed in order:
*          -# Read output parameter values from eTPU DATA RAM
*
* @param   *p_resolver_instance - This is a pointer to the instance structure 
*            @ref resolver_instance_t.
* @param   *p_resolver_states - This is a pointer to the structure 
*            of state @ref resolver_states_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_NONE - No error
*
*******************************************************************************/
uint32_t fs_etpu_resolver_get_states(
  struct resolver_instance_t *p_resolver_instance,
  struct resolver_states_t   *p_resolver_states)
{
  uint32_t *cpba;
  uint32_t *cpbae;

  cpba = p_resolver_instance->cpba;                       
  cpbae = cpba + (0x4000 >> 2); /* sign-extended memory area */

  /* Read channel parameters */
  p_resolver_states->exc_phase_shift    = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_EXC_PHASE_SHIFT    - 1)>>2));
  p_resolver_states->exc_zero_cross_err = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_EXC_ZERO_CROSS_ERR - 1)>>2));
  p_resolver_states->exc_pi_acc         = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_EXC_PI_ACC         - 1)>>2));
  p_resolver_states->ato_sig_sin        = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_ATO_SIG_SIN        - 1)>>2));
  p_resolver_states->ato_sig_cos        = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_ATO_SIG_COS        - 1)>>2));
  p_resolver_states->ato_acc            = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_ATO_PI_ACC         - 1)>>2));
  p_resolver_states->ato_angle_error    = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_ATO_ANGLE_ERROR    - 1)>>2));
  p_resolver_states->ato_angle          = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_ATO_ANGLE          - 1)>>2));
  p_resolver_states->ato_speed          = *(cpbae + ((FS_ETPU_RESOLVER_OFFSET_ATO_SPEED          - 1)>>2));
  p_resolver_states->upd                = *((uint8_t*)cpba + FS_ETPU_RESOLVER_OFFSET_UPD);
  p_resolver_states->state              = *((uint8_t*)cpba + FS_ETPU_RESOLVER_OFFSET_STATE);
  
  return(FS_ETPU_ERROR_NONE);
}

/*******************************************************************************
 *
 * REVISION HISTORY:
 * FILE OWNER: Marketa Venclikova [nxa17216]
 * Revision 1.75 2020/01/17  nxa17216
 * Added smpr_id parameter into resolver_config_t structure to define semaphore used to lock data by HW semaphore during update to ensure coherence
 * Revision 1.74 2019/11/14  nxa17216
 * Added initialization of separate ATO channel. Added parameter chan_num_ato into resolver_instance_t.
 * Revision 1.73 2019/10/30  nxa17216
 * Added option FS_ETPU_RESOLVER_OPTIONS_RDC_CHECKER_ON to switch on logging data into structure for RDC checker processing.
 * Revision 1.72 2019/10/01  nxa17216
 * Added new parameters to resolver_instance structure:
 *  - dma_num_chan - number of eTPU channel to generate DMA request to transfer checker data from eTPU RAM to system RAM for RDC checker processing.
 *  - *checker_signals_pba - pointer to checker data structure in eTPU RAM.
 * Revision 1.71 2018/08/17  b52107 
 * Updated the condition to apply RESOLVER_EXC_Adapt() only when signal-related cumulative flags are equal to zero, regardless the startup flags.
 * Revision 1.7 2018/07/17  b52107
 * Fixed the condition in RESOLVER_State_Update() to prevent from getting freeze 
 * in state RESOLVER_STATE_EXC_GEN due to overflowing condition computation to proceed to state RESOLVER_STATE_EXC_ADAPT.
 * Revision 1.63 2018/02/01  b52107
 * Added Low_ampl_thrs threshold and FS_ETPU_RESOLVER_DIAG_FLAG_SIN_LOW_AMPL and FS_ETPU_RESOLVER_DIAG_FLAG_COS_LOW_AMPL flags.
 * Removed Vector_fitered value.
 * Revision 1.62 2017/08/17  r54529
 * Startup error flags added to LOS, DOS, LOT masks.
 * Revision 1.6 2017/04/28   r54529
 * State machine added, which enables diagnostics even after EXC and ATO settles.
 * Structure resolver_states_t updated.  
 * Revision 1.5 2017/04/07   r54529
 * q_ewma_speed added (Speed EWMA filter).
 * Revision 1.41 2017/04/03  r54529
 * Fix of diagnostics not executed.
 * Revision 1.4  2017/03/22  r54529
 * Sample channel added for triggering extrapolation.
 * Revision 1.3  2017/02/07  r54529
 * 32 samples per period.
 * Revision 1.2  2016/03/08  r54529
 * New improved diagnostics.
 * Revision 1.0  2012/05/17  r54529
 * Initial version of file.
 ******************************************************************************/
