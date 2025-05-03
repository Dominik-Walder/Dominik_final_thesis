/*******************************************************************************
*
*   (c) Copyright 2004-2012 Freescale Semiconductor, Inc.
*   Copyright 2017 NXP
*   NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
*   in accordance with the applicable license terms. By expressly accepting
*   such terms or by downloading, installing, activating and/or otherwise using
*   the software, you are agreeing that you have read, and that you agree to
*   comply with and are bound by, such license terms. If you do not agree to
*   be bound by the applicable license terms, then you may not retain,
*   install, activate or otherwise use the software.
****************************************************************************//*!
*
* @file    etpu_as.c
*
* @author  Marketa Venclikova [nxa17216]
* 
* @version 1.1
* 
* @date    6-September-2017
*
* @brief   This file contains API for using the eTPU function
*          Analog Sensing (AS).
* 
****************************************************************************//*!
*
* @mainpage
*
* The eTPU AS APIs @ref etpu_as.c/.h includes API functions for eTPU
* function Analog Sensing.
* 
*******************************************************************************/
/*******************************************************************************
* Includes
*******************************************************************************/
#include "etpu_as.h"      /* private header file */
#include "../_utils/etpu_util.h"    /* utility routines for working with the eTPU */
#include "../pwmm/etpu_pwmm.h"    /* access to PWMM parameter offsets */

/*******************************************************************************
* Global variables
*******************************************************************************/
extern uint32_t fs_etpu_data_ram_start;
extern uint32_t fs_etpu_data_ram_ext;

/*******************************************************************************
* FUNCTION: fs_etpu_as_init
****************************************************************************//*!
* @brief   This function initializes eTPU channels to run AS function.
*
* @note    The following actions are performed in order:
*          -# Use user-defined CPBA or allocate new eTPU DATA RAM
*          -# Write chan config registers and FM bits
*          -# Write channel parameters
*          -# Write HSR
*          -# Set channel priority
*
* @warning The PWMM channel eTPU DATA RAM allocation must be done 
*          before the AS channel initialization (using fs_etpu_as_init).
*          On AS initialization, pointers to PWMM parameters are created.
*          This is required only when mode is FS_ETPU_AS_MODE_PWMM_SLAVE,
*          FS_ETPU_AS_PHASE_CURRENTS_ON is set or any of
*          AS_CFIFO_FRAME/CENTER_PULSE_START/END is set. 
*
* @param   *p_as_instance - This is a pointer to the instance structure 
*            @ref as_instance_t.
* @param   *p_pwmm_instance - This is a pointer to the instance structure 
*            @ref pwmm_instance_t. It is used in case there is any interaction
*            between AS and PWMM eTPU functions - if running AS in synchronized
*            more or if reading PWMM sector for phase current processing.
* @param   *p_as_config - This is a pointer to the structure of configuration
*            parameters @ref as_config_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_MALLOC - eTPU DATA RAM memory allocation error
*          - @ref FS_ETPU_ERROR_NONE - No error
*
* @warning This function does not configure the pins, only the eTPU channels.
*******************************************************************************/
uint32_t fs_etpu_as_init(
  struct as_instance_t   *p_as_instance,
  struct pwmm_instance_t *p_pwmm_instance,
  struct as_config_t     *p_as_config)
{
  uint8_t  chan_num;
  uint8_t  priority;
  uint32_t *cpba;
  uint32_t *cpba_signals;
  uint32_t cpba_pwmm_local;
  uint8_t  signal_count;
  uint32_t cr;
  uint8_t  i;

  chan_num     = p_as_instance->chan_num;   
  priority     = p_as_instance->priority;                
  cpba         = p_as_instance->cpba;
  cpba_signals = p_as_instance->cpba_signals;
  signal_count = p_as_instance->signal_count;                       
  cpba_pwmm_local = (uint32_t)p_pwmm_instance->cpba - fs_etpu_data_ram_start;
  
  /* Use user-defined CPBA or allocate new eTPU DATA RAM */
  if(cpba == 0)
  {
    cpba = fs_etpu_malloc(FS_ETPU_AS_NUM_PARMS);
    if(cpba == 0)
    {
      return(FS_ETPU_ERROR_MALLOC);
    }
    else
    {
      p_as_instance->cpba = cpba;
    }
  }
  if(cpba_signals == 0)
  {
    cpba_signals = fs_etpu_malloc(FS_ETPU_AS_SIGNAL_STRUCT_SIZE*signal_count);
    if(cpba_signals == 0)
    {
      return(FS_ETPU_ERROR_MALLOC);
    }
    else
    {
      p_as_instance->cpba_signals = cpba_signals;
    }
  }
  
  /* Write chan config registers and FM bits */
  cr = (FS_ETPU_AS_TABLE_SELECT << 24) +
       (FS_ETPU_AS_FUNCTION_NUMBER << 16) +
       (((uint32_t)cpba - fs_etpu_data_ram_start) >> 3);
  eTPU->CHAN[chan_num].CR.R = cr; 
  eTPU->CHAN[chan_num].SCR.R = (uint32_t)p_as_config->pulse_selection;

  /* Write channel parameters */
  /* 24-bit */
  *(cpba + ((FS_ETPU_AS_OFFSET_START_OFFSET            - 1)>>2)) = p_as_config->start_offset;
  *(cpba + ((FS_ETPU_AS_OFFSET_PERIOD                  - 1)>>2)) = p_as_config->period;
  *(cpba + ((FS_ETPU_AS_OFFSET_PULSE_ADJUSTMENT_FRAME  - 1)>>2)) = p_as_config->pulse_adjustment_frame;
  *(cpba + ((FS_ETPU_AS_OFFSET_PULSE_ADJUSTMENT_CENTER - 1)>>2)) = p_as_config->pulse_adjustment_center;
  *(cpba + ((FS_ETPU_AS_OFFSET_PULSE_WIDTH             - 1)>>2)) = p_as_config->pulse_width;
  *(cpba + ((FS_ETPU_AS_OFFSET_FRAME_TIME              - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_AS_OFFSET_CENTER_TIME             - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_AS_OFFSET_PWMM_SECTOR             - 1)>>2)) = cpba_pwmm_local + FS_ETPU_PWMM_OFFSET_SECTOR;
  *(cpba + ((FS_ETPU_AS_OFFSET_PWMM_FRAME_TIME         - 1)>>2)) = cpba_pwmm_local + FS_ETPU_PWMM_OFFSET_FRAME_TIME;
  *(cpba + ((FS_ETPU_AS_OFFSET_PWMM_PERIOD             - 1)>>2)) = cpba_pwmm_local + FS_ETPU_PWMM_OFFSET_PERIOD;
  *(cpba + ((FS_ETPU_AS_OFFSET_RESULT_QUEUE            - 1)>>2)) = (uint32_t)p_as_config->p_result_queue;
  *(cpba + ((FS_ETPU_AS_OFFSET_COMMAND_QUEUE           - 1)>>2)) = (uint32_t)p_as_config->p_cfifo;

  /* 8-bit */
  *((uint8_t*)cpba + FS_ETPU_AS_OFFSET_OPTIONS_POLARITY      ) = p_as_instance->polarity;
  *((uint8_t*)cpba + FS_ETPU_AS_OFFSET_OPTIONS_LINK          ) = p_as_config->link_options;
  *((uint8_t*)cpba + FS_ETPU_AS_OFFSET_OPTIONS_IRQ           ) = p_as_config->irq_dma_options;
  *((uint8_t*)cpba + FS_ETPU_AS_OFFSET_OPTIONS_CFIFO         ) = p_as_config->cfifo_update;
  *((uint8_t*)cpba + FS_ETPU_AS_OFFSET_SIGNAL_MASK_FRAME     ) = p_as_config->signal_mask_frame;
  *((uint8_t*)cpba + FS_ETPU_AS_OFFSET_SIGNAL_MASK_CENTER    ) = p_as_config->signal_mask_center;
  *((uint8_t*)cpba + FS_ETPU_AS_OFFSET_SIGNAL_MASK_DC_OFFSET ) = 0;
  *((uint8_t*)cpba + FS_ETPU_AS_OFFSET_SECTOR                ) = 0;
  *((uint8_t*)cpba + FS_ETPU_AS_OFFSET_PHASE_CURRENT_A_IDX   ) = p_as_config->phase_current_idx_a;
  *((uint8_t*)cpba + FS_ETPU_AS_OFFSET_PHASE_CURRENT_B_IDX   ) = p_as_config->phase_current_idx_b;
  *((uint8_t*)cpba + FS_ETPU_AS_OFFSET_PHASE_CURRENT_C_IDX   ) = p_as_config->phase_current_idx_c;
  *((uint8_t*)cpba + FS_ETPU_AS_OFFSET_OPTIONS_PHASE_CURRENTS) = p_as_config->phase_current_processing; 
  *((uint8_t*)cpba + FS_ETPU_AS_OFFSET_SIGNAL_COUNT          ) = signal_count;

  /* 32-bit */
  *(cpba + (FS_ETPU_AS_OFFSET_LINK_CHANS_FRAME_START  >>2)) = p_as_config->link_chans_frame_start;
  *(cpba + (FS_ETPU_AS_OFFSET_LINK_CHANS_FRAME_END    >>2)) = p_as_config->link_chans_frame_end;
  *(cpba + (FS_ETPU_AS_OFFSET_LINK_CHANS_CENTER_START >>2)) = p_as_config->link_chans_center_start;
  *(cpba + (FS_ETPU_AS_OFFSET_LINK_CHANS_CENTER_END   >>2)) = p_as_config->link_chans_center_end;
  *(cpba + (FS_ETPU_AS_OFFSET_CMD_IA_ADC0             >>2)) = p_as_config->cmd_ia_adc_0;
  *(cpba + (FS_ETPU_AS_OFFSET_CMD_IA_ADC1             >>2)) = p_as_config->cmd_ia_adc_1;
  *(cpba + (FS_ETPU_AS_OFFSET_CMD_IB_ADC0             >>2)) = p_as_config->cmd_ib_adc_0;
  *(cpba + (FS_ETPU_AS_OFFSET_CMD_IB_ADC1             >>2)) = p_as_config->cmd_ib_adc_1;
  *(cpba + (FS_ETPU_AS_OFFSET_CMD_IC_ADC0             >>2)) = p_as_config->cmd_ic_adc_0;
  *(cpba + (FS_ETPU_AS_OFFSET_CMD_IC_ADC1             >>2)) = p_as_config->cmd_ic_adc_0;
  *(cpba + (FS_ETPU_AS_OFFSET_SIGNAL                  >>2)) = (uint32_t)cpba_signals - fs_etpu_data_ram_start;

  /* Write signal array parameters */
  for(i=0; i<signal_count; i++)
  {
    *(cpba_signals + ((FS_ETPU_AS_OFFSET_GAIN          - 1)>>2)) = p_as_config->p_signal_config[i].gain;
    *(cpba_signals + ((FS_ETPU_AS_OFFSET_DC_OFFSET     - 1)>>2)) = p_as_config->p_signal_config[i].dc_offset;
    *(cpba_signals + ((FS_ETPU_AS_OFFSET_FORGET_FACTOR - 1)>>2)) = p_as_config->p_signal_config[i].forget_factor;
    *(cpba_signals + ((FS_ETPU_AS_OFFSET_VALUE         - 1)>>2)) = 0;
    *((uint8_t*)cpba_signals + FS_ETPU_AS_OFFSET_QUEUE_OFFSET  ) = p_as_config->p_signal_config[i].queue_offset >> 2;
    cpba_signals += FS_ETPU_AS_SIGNAL_STRUCT_SIZE >> 2;
  }

  /* Write HSR */  
  eTPU->CHAN[chan_num].HSRR.R = p_as_instance->mode;
  
  /* Set channel priority */
  fs_etpu_enable(chan_num, priority);
  
  return(FS_ETPU_ERROR_NONE);
}

/*******************************************************************************
* FUNCTION: fs_etpu_as_config
****************************************************************************//*!
* @brief   This function changes the AS configuration.
*
* @note    The following actions are performed in order:
*          -# Write configuration parameter values to eTPU DATA RAM
*          -# Write AS channel FM bits
*
* @param   *p_as_instance - This is a pointer to the instance structure 
*            @ref as_instance_t.
* @param   *p_as_config - This is a pointer to the structure of configuration
*            parameters @ref as_config_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_NONE - No error
*
*******************************************************************************/
uint32_t fs_etpu_as_config(
  struct as_instance_t *p_as_instance,
  struct as_config_t   *p_as_config)
{
  uint32_t *cpba;
  uint32_t *cpbae;
  uint32_t *cpbae_signals;
  uint8_t  signal_count;
  uint8_t  i;

  cpba = p_as_instance->cpba;                       
  cpbae = cpba + (0x4000 >> 2); /* sign-extended memory area */
  cpbae_signals = p_as_instance->cpba_signals + (0x4000 >> 2); /* sign-extended memory area */
  signal_count = p_as_instance->signal_count;                       

  /* Write channel parameters */
  /* 24-bit - use cpbae to prevent from overwriting bits 31:24 */
  *(cpbae + ((FS_ETPU_AS_OFFSET_PERIOD                  - 1)>>2)) = p_as_config->period;
  *(cpbae + ((FS_ETPU_AS_OFFSET_PULSE_ADJUSTMENT_FRAME  - 1)>>2)) = p_as_config->pulse_adjustment_frame;
  *(cpbae + ((FS_ETPU_AS_OFFSET_PULSE_ADJUSTMENT_CENTER - 1)>>2)) = p_as_config->pulse_adjustment_center;
  *(cpbae + ((FS_ETPU_AS_OFFSET_PULSE_WIDTH             - 1)>>2)) = p_as_config->pulse_width;

  /* 8-bit */
  *((uint8_t*)cpba + FS_ETPU_AS_OFFSET_OPTIONS_LINK          ) = p_as_config->link_options;
  *((uint8_t*)cpba + FS_ETPU_AS_OFFSET_OPTIONS_IRQ           ) = p_as_config->irq_dma_options;
  *((uint8_t*)cpba + FS_ETPU_AS_OFFSET_OPTIONS_CFIFO         ) = p_as_config->cfifo_update;
  *((uint8_t*)cpba + FS_ETPU_AS_OFFSET_SIGNAL_MASK_FRAME     ) = p_as_config->signal_mask_frame;
  *((uint8_t*)cpba + FS_ETPU_AS_OFFSET_SIGNAL_MASK_CENTER    ) = p_as_config->signal_mask_center;
  *((uint8_t*)cpba + FS_ETPU_AS_OFFSET_OPTIONS_PHASE_CURRENTS) = p_as_config->phase_current_processing; 

  /* 32-bit */
  *(cpba + (FS_ETPU_AS_OFFSET_LINK_CHANS_FRAME_START  >>2)) = p_as_config->link_chans_frame_start;
  *(cpba + (FS_ETPU_AS_OFFSET_LINK_CHANS_FRAME_END    >>2)) = p_as_config->link_chans_frame_end;
  *(cpba + (FS_ETPU_AS_OFFSET_LINK_CHANS_CENTER_START >>2)) = p_as_config->link_chans_center_start;
  *(cpba + (FS_ETPU_AS_OFFSET_LINK_CHANS_CENTER_END   >>2)) = p_as_config->link_chans_center_end;

  /* Write signal array parameters */
  for(i=0; i<signal_count; i++)
  {
	*(cpbae_signals + ((FS_ETPU_AS_OFFSET_GAIN          - 1)>>2)) = p_as_config->p_signal_config[i].gain;
	*(cpbae_signals + ((FS_ETPU_AS_OFFSET_DC_OFFSET     - 1)>>2)) = p_as_config->p_signal_config[i].dc_offset;
	*(cpbae_signals + ((FS_ETPU_AS_OFFSET_FORGET_FACTOR - 1)>>2)) = p_as_config->p_signal_config[i].forget_factor;
    cpbae_signals += FS_ETPU_AS_SIGNAL_STRUCT_SIZE >> 2;
  }
  
  /* Write  */
  eTPU->CHAN[p_as_instance->chan_num].SCR.R = (uint32_t)p_as_config->pulse_selection;

  return(FS_ETPU_ERROR_NONE);
}

/*******************************************************************************
* FUNCTION: fs_etpu_as_get_outputs
****************************************************************************//*!
* @brief   This function reads output values of the AS function.
*
* @note    The following actions are performed in order:
*          -# Read output parameter values from eTPU DATA RAM
*
* @param   *p_as_instance - This is a pointer to the instance structure 
*            @ref as_instance_t.
* @param   *p_as_outputs - This is a pointer to the structure of states
*            @ref as_outputs_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_NONE - No error
*
*******************************************************************************/
uint32_t fs_etpu_as_get_outputs(
  struct as_instance_t *p_as_instance,
  struct as_outputs_t  *p_as_outputs)
{
  uint32_t *cpbae_signals;
  uint8_t  signal_count;
  uint8_t  i;

  cpbae_signals = p_as_instance->cpba_signals + (0x4000 >> 2); /* sign-extended memory area */
  signal_count  = p_as_instance->signal_count;                       
  
  /* Read signal value from signal array */
  for(i=0; i<signal_count; i++)
  {
    p_as_outputs->signal_value[i] = *(cpbae_signals + ((FS_ETPU_AS_OFFSET_VALUE - 1)>>2));
    cpbae_signals += FS_ETPU_AS_SIGNAL_STRUCT_SIZE >> 2;     
  }

  return(FS_ETPU_ERROR_NONE);
}

/*******************************************************************************
* FUNCTION: fs_etpu_as_measure_dc_offsets
****************************************************************************//*!
* @brief   This function uses the signal output values to set the signal 
*          dc-offsets. It is presummed the current physical signal value 
*          corresponds to what a zero value is.
*
* @note    The following actions are performed in order:
*          -# Read output parameter values from eTPU DATA RAM
*
* @param   *p_as_instance - This is a pointer to the instance structure 
*            @ref as_instance_t.
* @param   *p_as_config - This is a pointer to the structure of outputs
*            @ref as_config_t. The signal dc_offsets are updated in this 
*            structure.
* @param   signal_mask - this mask defines which signal dc_offsets are measured.
*            Bit 0 (0x01) corresponds to signal[0].
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_NONE - No error
*
*******************************************************************************/
uint32_t fs_etpu_as_measure_dc_offsets(
  struct as_instance_t *p_as_instance,
  struct as_config_t   *p_as_config,
         uint8_t       signal_mask)
{
  uint32_t *cpbae_signals;
  uint8_t  signal_count;
  int24_t  dc_offset;
  uint8_t  i;

  cpbae_signals = p_as_instance->cpba_signals + (0x4000 >> 2); /* sign-extended memory area */
  signal_count  = p_as_instance->signal_count;                       
  
  for(i=0; i<signal_count; i++)
  {
    if(signal_mask & (1<<i))
    {
      dc_offset = *(cpbae_signals + ((FS_ETPU_AS_OFFSET_DC_OFFSET - 1)>>2));
      dc_offset += *(cpbae_signals + ((FS_ETPU_AS_OFFSET_VALUE - 1)>>2));
      *(cpbae_signals + ((FS_ETPU_AS_OFFSET_DC_OFFSET - 1)>>2)) = dc_offset;
      p_as_config->p_signal_config[i].dc_offset = dc_offset;
    }
    cpbae_signals += FS_ETPU_AS_SIGNAL_STRUCT_SIZE >> 2;     
  } 

  return(FS_ETPU_ERROR_NONE);
}

/*******************************************************************************
 *
 * REVISION HISTORY:
 *
 * FILE OWNER: Marketa Venclikova [nxa17216]
 *
 * Revision 1.1  2017/09/06  r54529
 * as_config_t parameter as_signal_config_t signal_config[FS_ETPU_AS_SIGNALS_MAX]
 * was changed to a pointer to array of individual signal processing configuration
 * as_signal_config_t *p_signal_config
 * Revision 1.0  2012/05/17  r54529
 * Initial version of file.
 ******************************************************************************/
