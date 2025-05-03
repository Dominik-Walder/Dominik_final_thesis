
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
* @file    etpu_as.h
*
* @author   Marketa Venclikova [nxa17216]
* 
* @version 1.1
* 
* @date    06-September-2017
*
* @brief   This file contains useful macros and prototypes for AS API.
*
*******************************************************************************/
#ifndef _ETPU_AS_H_
#define _ETPU_AS_H_

/*******************************************************************************
*                                        INCLUDE FILES
*******************************************************************************/
#include "../_utils/etpu_util.h"        /* 24-bit types */
#include "../_etpu_set/etpu_as_auto.h"     /* auto generated header file */
#include "../pwmm/etpu_pwmm.h"        /* struct pwmm_instance_t definition */

/*******************************************************************************
*                                      DEFINES AND MACROS
*******************************************************************************/
/** AS Mode */
#define FS_ETPU_AS_MODE_MASTER       FS_ETPU_AS_HSR_INIT_MODE_PERIODIC
#define FS_ETPU_AS_MODE_PWMM_SLAVE   FS_ETPU_AS_HSR_INIT_MODE_SYNC

/*******************************************************************************
*                                STRUCTURES AND OTHER TYPEDEFS
*******************************************************************************/
/** A structure to represent an instance of AS. 
 *  It includes static AS initialization items. */
struct as_instance_t
{
  const uint8_t  chan_num;  /**< Channel number of the AS channel. */
  const uint8_t  priority;  /**< Channel priority for the AS channel. */
  const uint8_t  mode;      /**< AS mode. 
    It can be any of:
    - @ref FS_ETPU_AS_HSR_INIT_MODE_PERIODIC - AS signal generation is based on 
        start_offset and period parameters
    - @ref FS_ETPU_AS_HSR_INIT_MODE_SYNC     - AS signal generation is based on
        PWMM frame_time and center_time */
  const uint8_t  polarity;  /**< AS output signal polarity. 
    It can be any of:
    - @ref FS_ETPU_AS_POLARITY_PULSE_HIGH
    - @ref FS_ETPU_AS_POLARITY_PULSE_LOW */
  const uint8_t  signal_count; /**< Count of AS signals processed. It can be
    1 to FS_ETPU_AS_SIGNALS_MAX. */
        uint32_t *cpba;     /**< Channel parameter base address. 
    Set cpba = 0 to use automatic allocation of eTPU DATA RAM for AS channel 
    parameters using the eTPU utility function fs_etpu_malloc (recommended),
    or assign the cpba manually by an address where the AS channel parameter 
    space will start from, e.g. 0xC3FC8100. */
        uint32_t *cpba_signals; /**< Signal array base address.
    Set cpba_signals = 0 to use automatic allocation of eTPU DATA RAM for AS  
    signal parameter array (recommended), or assign the cpba_signals manually
    by an address where the array will start. */
};

/** A structure to represent a single AS signal processing configuration. 
 *  It includes both static and dynamic values. */  
struct as_signal_config_t
{
  const uint8_t  queue_offset;  /**< This is a byte offset of the ADC result
      in the eQADC result queue. Together with as_config.result_queue, it 
      defines where the input of AS signal processing is read. */
        int24_t  gain;          /**< This is an integer gain to apply to the
      ADC result. The ADC result is a (max) 16-bit number and the gain is used
      to align it to a 24-bit number. Additionally, a gain correction can be
      applied using this gain. */ 
        int24_t  dc_offset;     /**< This 24-bit DC-offset is applied after 
      the gain. */
        fract24_t forget_factor; /**< This 24-bit fractional value defines
      a first-order low-pass IIR filter called Exponentioally Weighted Moving
      Average (EWMA). 
      - forget_factor = 0x000000 results in no averaging
      - forget_factor = 0x7FFFFF results in maximum averaging */
};

/** A structure to represent a configuration of AS. 
 *  It includes AS configuration items which can mostly be changed in 
 *  run-time. */
struct as_config_t
{
  const uint24_t start_offset;  /**< This parameter is used to synchronize 
    various eTPU functions that generate a signal. For AS: the first period 
    frame is scheduled start_offset TCR1 cycles after initialization. */
        uint24_t period;       /**< AS period as a number of TCR1 cycles. */
        uint24_t pulse_width;  /**< AS pulse-width as a number of TCR1 cycles.*/
        uint8_t  pulse_selection;/**< selection of pulse generation.
    Use none, one or both of:
    - @ref FS_ETPU_AS_FM0_FRAME_PULSE_ON
    - @ref FS_ETPU_AS_FM1_CENTER_PULSE_ON */
         int24_t pulse_adjustment_frame;  /**< frame pulse position adjustment
    as a positive or negative number of TCR1 cycles.*/
         int24_t pulse_adjustment_center; /**< center pulse position adjustment
    as a positive or negative number of TCR1 cycles.*/
        uint8_t  irq_dma_options;  /**< selection of IRQ&DMA generation.
    Use none, one or more of:
    - @ref FS_ETPU_AS_IRQ_FRAME_PULSE_START
    - @ref FS_ETPU_AS_IRQ_FRAME_PULSE_END 
    - @ref FS_ETPU_AS_IRQ_CENTER_PULSE_START
    - @ref FS_ETPU_AS_IRQ_CENTER_PULSE_END */
        uint8_t  link_options;  /**< selection of link generation.
    Use none, one or more of:
    - @ref FS_ETPU_AS_LINK_FRAME_PULSE_START
    - @ref FS_ETPU_AS_LINK_FRAME_PULSE_END 
    - @ref FS_ETPU_AS_LINK_CENTER_PULSE_START
    - @ref FS_ETPU_AS_LINK_CENTER_PULSE_END */
        uint32_t link_chans_frame_start;  /**< up to 4 link numbers packed to
    a single 32-bit word. These links are sent at the frame pulse start. Use 
    FS_ETPU_CHANNEL_TO_LINK() to convert a channel number to a link number. */
        uint32_t link_chans_frame_end;  /**< up to 4 link numbers packed to
    a single 32-bit word. These links are sent at the frame pulse end. Use 
    FS_ETPU_CHANNEL_TO_LINK() to convert a channel number to a link number. */
        uint32_t link_chans_center_start;  /**< up to 4 link numbers packed to
    a single 32-bit word. These links are sent at the center pulse start. Use 
    FS_ETPU_CHANNEL_TO_LINK() to convert a channel number to a link number. */
        uint32_t link_chans_center_end;  /**< up to 4 link numbers packed to
    a single 32-bit word. These links are sent at the center pulse end. Use 
    FS_ETPU_CHANNEL_TO_LINK() to convert a channel number to a link number. */
  const uint32_t *p_result_queue; /**< Pointer to eQADC results array 
    in the eTPU DATA RAM */
  struct as_signal_config_t *p_signal_config; /**< Pointer to array of
    of individual signal processing configurations. */
        uint8_t  signal_mask_frame;  /**< this mask defines which signals are
    processed on the frame pulse end. Bit 0 (LSB) corresponds to signal[0].
    This parameter is used only if FS_ETPU_AS_FM0_FRAME_PULSE_ON is set. */
        uint8_t  signal_mask_center;  /**< this mask defines which signals are
    processed on the center pulse end. Bit 0 (LSB) corresponds to signal[0].
    This parameter is used only if FS_ETPU_AS_FM1_CENTER_PULSE_ON is set. */
        uint8_t  phase_current_processing; /**< 3-phase current processing.
    This switch turns on a calculation the 3rd phase current from the other 2.
    Which phase current is calculated is based on actual motor position in one 
    of 6 sectors. See also phase_current_idx_a/b/c and pwmm_chan_num.    
    Use one of:
    - @ref FS_ETPU_AS_PHASE_CURRENTS_ON
    - @ref FS_ETPU_AS_PHASE_CURRENTS_OFF */
  const uint8_t  phase_current_idx_a; /**< index of the phase A current 
    in the signal array. Used only when FS_ETPU_AS_PHASE_CURRENTS_ON is set. */    
  const uint8_t  phase_current_idx_b; /**< index of the phase B current 
    in the signal array. Used only when FS_ETPU_AS_PHASE_CURRENTS_ON is set. */    
  const uint8_t  phase_current_idx_c; /**< index of the phase C current 
    in the signal array. Used only when FS_ETPU_AS_PHASE_CURRENTS_ON is set. */
  const uint8_t  cfifo_update;  /**< update of eQADC command FIFO.
    This enables 2 selected phase current to be sampled in parallel. 
    Which phase currents are selected is based on actual motor position in one
    of 6 sectors. See also *command_queue, cmd_ia/b/c_adc_0/1 and pwmm_chan_num.
    Use none, one or more of:
    - @ref AS_CFIFO_FRAME_PULSE_START  - modify eQADC Command FIFO at the start
      of the frame pulse. Applies only if FS_ETPU_AS_FM0_FRAME_PULSE_ON is set.
    - @ref AS_CFIFO_FRAME_PULSE_END    - modify eQADC Command FIFO at the end
      of the frame pulse.
    - @ref AS_CFIFO_CENTER_PULSE_START - modify eQADC Command FIFO at the start
      of the center pulse. Applies only if FS_ETPU_AS_FM0_CENTER_PULSE_ON is set
    - @ref AS_CFIFO_CENTER_PULSE_END   - modify eQADC Command FIFO at the end
      of the center pulse. */
  const uint32_t *p_cfifo; /**< Pointer to eQADC command queue in eTPU DATA RAM.
    Used only when any of the AS_CFIFO_FRAME/CENTER_PULSE_START/END is set. */
  const uint32_t cmd_ia_adc_0;   /**< eQADC command to sample 
    phase A current on ADC0. 
    Used only when any of the AS_CFIFO_FRAME/CENTER_PULSE_START/END is set. */ 
  const uint32_t cmd_ia_adc_1;   /**< eQADC command to sample 
    phase A current on ADC1. 
    Used only when any of the AS_CFIFO_FRAME/CENTER_PULSE_START/END is set. */ 
  const uint32_t cmd_ib_adc_0;   /**< eQADC command to sample 
    phase B current on ADC0. 
    Used only when any of the AS_CFIFO_FRAME/CENTER_PULSE_START/END is set. */ 
  const uint32_t cmd_ib_adc_1;   /**< eQADC command to sample 
    phase B current on ADC1. 
    Used only when any of the AS_CFIFO_FRAME/CENTER_PULSE_START/END is set. */ 
  const uint32_t cmd_ic_adc_0;   /**< eQADC command to sample 
    phase C current on ADC0. 
    Used only when any of the AS_CFIFO_FRAME/CENTER_PULSE_START/END is set. */ 
  const uint32_t cmd_ic_adc_1;   /**< eQADC command to sample 
    phase C current on ADC1. 
    Used only when any of the AS_CFIFO_FRAME/CENTER_PULSE_START/END is set. */ 
};

/** A structure to represent outputs of AS processig. */ 
struct as_outputs_t
{
        int24_t  signal_value[FS_ETPU_AS_SIGNALS_MAX]; /**< This is the output 
      of the signal processing - the current signal value as a 24-bit integer.*/ 
};


/*******************************************************************************
*                                    FUNCTION PROTOTYPES
*******************************************************************************/
/* Initialize */
uint32_t fs_etpu_as_init(
  struct as_instance_t   *p_as_instance,
  struct pwmm_instance_t *p_pwmm_instance,
  struct as_config_t     *p_as_config);

/* Change configuration */
uint32_t fs_etpu_as_config(
  struct as_instance_t *p_as_instance,
  struct as_config_t   *p_as_config);

/* Get outputs */
uint32_t fs_etpu_as_get_outputs(
  struct as_instance_t *p_as_instance,
  struct as_outputs_t  *p_as_outputs);

/* Measure DC-offsets */
uint32_t fs_etpu_as_measure_dc_offsets(
  struct as_instance_t *p_as_instance,
  struct as_config_t   *p_as_config,
         uint8_t       signal_mask);


#endif /* _ETPU_AS_H_ */

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
