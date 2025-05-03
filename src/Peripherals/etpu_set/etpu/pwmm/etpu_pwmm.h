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
* @file    etpu_pwmm.h
*
* @author  Marketa Venclikova [nxa17216]
* 
* @version 1.2
* 
* @date    14-February-2018
*
* @brief   This file contains useful macros and prototypes for PWMM API.
*
*******************************************************************************/
#ifndef _ETPU_PWMM_H_
#define _ETPU_PWMM_H_

/*******************************************************************************
*                                        INCLUDE FILES
*******************************************************************************/
#include "../_utils/etpu_util.h"        /* 24-bit types */
#include "../_etpu_set/etpu_pwmm_auto.h"   /* auto generated header file */

/*******************************************************************************
*                                      DEFINES AND MACROS
*******************************************************************************/

/*******************************************************************************
*                                STRUCTURES AND OTHER TYPEDEFS
*******************************************************************************/
/** A structure to represent an instance of PWMM. 
 *  It includes static PWMM initialization items. */
struct pwmm_instance_t
{
  const uint8_t  chan_num_master;  /**< Channel number of the PWMM master channel. */
  const uint8_t  chan_num_phase_a; /**< Channel number of the Phase A channel. */
  const uint8_t  chan_num_phase_b; /**< Channel number of the Phase B channel. */
  const uint8_t  chan_num_phase_c; /**< Channel number of the Phase C channel. */
  const uint8_t  priority;         /**< Channel priority for all PWMM channels. */
  const uint8_t  phase_type;       /**< Type of phases. 
    It can be one of:
    - @ref FS_ETPU_PWMM_FM0_SINGLE_CHANNELS
    - @ref FS_ETPU_PWMM_FM0_COMPLEMENTARY_PAIRS */
  const uint8_t  polarity;         /**< Base and complementary channel polarity 
    options. The available options are:
    - @ref FS_ETPU_PWMM_POLARITY_BASE_ACTIVE_HIGH/LOW - Base channel polarity
    - @ref FS_ETPU_PWMM_POLARITY_COMPL_ACTIVE_HIGH/LOW - Complementary channel 
      polarity 
    The similar options can be also expressed by:
    - FS_ETPU_PWMM_POLARITY_BASE/COMPL_HIGH_TRUE/LOW_TRUE
    - FS_ETPU_PWMM_POLARITY_BASE/COMPL_DIRECT/INVERTED/NON_INVERTED */
  const uint24_t start_offset;     /**< This parameter is used to synchronize 
    various eTPU functions that generate a signal. For PWMM: the first PWM 
    period frame is scheduled start_offset TCR1 cycles after initialization. */
        uint32_t *cpba;            /**< Channel parameter base address. 
    Set cpba = 0 to use automatic allocation of eTPU DATA RAM for PWMM channel 
    parameters using the eTPU utility function fs_etpu_malloc (recommanded),
    or assign the cpba manually by an address where the PWMM channel parameter 
    space will start from, e.g. 0xC3FC8100. */
};

/** A structure to represent a configuration of PWMM. 
 *  It includes PWMM configuration items which can be changed in run-time. */
struct pwmm_config_t
{
        uint8_t  update_option;   /**< selection of PWM update position. 
    It can be one of:
    - @ref FS_ETPU_PWMM_FM1_FRAME_UPDATE_ONLY
    - @ref FS_ETPU_PWMM_FM1_FRAME_AND_CENTER_UPDATE */
        uint8_t  modulation;      /**< Selection of modulation. The inputs are 
    transformed into output PWM duty-cycles by one of the predefined 
    modulations:
    - @ref FS_ETPU_PWMM_MODULATION_UNSIGNED: duty_a/b/c = (ufract24)input_a/b/c
    - @ref FS_ETPU_PWMM_MODULATION_SIGNED:   duty_a/b/c = (sfract24)input_a/b/c
    - @ref FS_ETPU_PWMM_MODULATION_SVMSTD:   duty_a/b/c = SvmStd(input_a/b)
    - @ref FS_ETPU_PWMM_MODULATION_SVMU0N:   duty_a/b/c = SvmU0n(input_a/b)
    - @ref FS_ETPU_PWMM_MODULATION_SVMU7N:   duty_a/b/c = SvmU7n(input_a/b)
    - @ref FS_ETPU_PWMM_MODULATION_PWMICT:   duty_a/b/c = PwmIct(input_a/b)
    - @ref FS_ETPU_PWMM_MODULATION_SINE_TABLE: angle = input_a, 
                                               amplitude = input_b*/
        uint8_t  mode;            /**< PWM Mode selection, a combination of
    PWM Alignment (edge/center) and PWM Type (normal/inverted). 
    The available options are:
    - @ref FS_ETPU_PWMM_MODE_LEFT_ALIGNED
    - @ref FS_ETPU_PWMM_MODE_RIGHT_ALIGNED 
    - @ref FS_ETPU_PWMM_MODE_CENTER_ALIGNED 
    - @ref FS_ETPU_PWMM_MODE_INV_CENTER_ALIGNED 
    The similar options can be also expressed by a combination:
    - @ref FS_ETPU_PWMM_MODE_ALIGN_EDGE or @ref FS_ETPU_PWMM_MODE_ALIGN_CENTER
    together with
    - @ref FS_ETPU_PWMM_MODE_TYPE_START_ACTIVE or @ref FS_ETPU_PWMM_MODE_TYPE_START_INACTIVE
    - @ref FS_ETPU_PWMM_MODE_TYPE_NORMAL or @ref FS_ETPU_PWMM_MODE_TYPE_INVERTED
    Note, the @ref FS_ETPU_PWMM_MODE_CENTER_ALIGNED mode is acombination of 
    @ref FS_ETPU_PWMM_MODE_ALIGN_CENTER and @ref FS_ETPU_PWMM_MODE_TYPE_INVERTED
    because the PWM period starts at inactive state, while 
    the @ref FS_ETPU_PWMM_MODE_INV_CENTER_ALIGNED mode is acombination of 
    @ref FS_ETPU_PWMM_MODE_ALIGN_CENTER and @ref FS_ETPU_PWMM_MODE_TYPE_NORMAL
    because the PWM period starts at active state. */
        uint24_t period;          /**< PWM period as a number of TCR1 cycles. */
        uint24_t dead_time;       /**< PWM deadtime as a number of TCR1 cycles.*/
        uint24_t min_pulse_width; /**< Minimum pulse width as number of TCR1 
    cycles. A threshold for pulse deletion.
    - Pulses longer than min_pulse_width are generated normally.
    - Pulses shorter than min_pulse_width are not generated. */
        uint24_t update_time;      /**< A time period (number of TCR1 cycles) 
    that is needed to perform an update of all PWM phases. */
};

/** A structure to represent inputs of PWMM used for update of generated PWM 
 *  duty-cycles. Based on the selected PWMM modulation, there are different 
 *  meanings of input_a, input_b and input_c. */ 
struct pwmm_inputs_t
{
        fract24_t input_a;
        fract24_t input_b;
        fract24_t input_c;
};

/** A structure to represent internal states of PWMM. */ 
struct pwmm_states_t
{
        uint8_t sector;      /**< This is the motor voltage vector position 
    in one of 6 sectors. The sector value is calculated for space vector 
    modulations only (modulation = FS_ETPU_PWMM_MODULATION_SVM* ). */
        uint8_t state; /**< This is the PWM state, having one of the
    values:
    - @ref FS_ETPU_PWMM_STATE_DISABLED
    - @ref FS_ETPU_PWMM_STATE_ENABLED 
    - @ref FS_ETPU_PWMM_STATE_TO_BE_ENABLED */
        uint8_t update;  /**< This flag can have one of the values: 
    - @ref FS_ETPU_PWMM_UPDATE_READY - new inputs were writen by the CPU
    - @ref FS_ETPU_PWMM_UPDATE_APPLIED - the inputs were read by the eTPU */
        int24_t missing_update_cnt;  /**< This counter is incremented if
    PWMM eTPU function doea not find new inputs on frame or center update. */
        ufract24_t duty_a; /**< This is the current phase A duty-cycle 
    as an unsigned 24-bit fractional value (0x000000, 0x7FFFFF).
    Note, coherent reading of all 3 phase duty-cycles is not ensured! */
        ufract24_t duty_b; /**< This is the current phase B duty-cycle. */
        ufract24_t duty_c; /**< This is the current phase C duty-cycle. */
};

/*******************************************************************************
*                                    FUNCTION PROTOTYPES
*******************************************************************************/

/* Initialize */
uint32_t fs_etpu_pwmm_init(
  struct pwmm_instance_t *p_pwmm_instance,
  struct pwmm_config_t   *p_pwmm_config);

/* Change configuration */
uint32_t fs_etpu_pwmm_config(
  struct pwmm_instance_t *p_pwmm_instance,
  struct pwmm_config_t   *p_pwmm_config);

/* Set inputs */
uint32_t fs_etpu_pwmm_set_inputs(
  struct pwmm_instance_t *p_pwmm_instance,
  struct pwmm_inputs_t   *p_pwmm_inputs);

/* Enable */
uint32_t fs_etpu_pwmm_enable(
  struct pwmm_instance_t *p_pwmm_instance);

/* Disable */
uint32_t fs_etpu_pwmm_disable(
  struct pwmm_instance_t *p_pwmm_instance);

/* Get states */
uint32_t fs_etpu_pwmm_get_states(
  struct pwmm_instance_t *p_pwmm_instance,
  struct pwmm_states_t   *p_pwmm_states);


#endif /* _ETPU_PWMM_H_ */

/*******************************************************************************
*
 * REVISION HISTORY:
 *
 * FILE OWNER: Marketa Venclikova [nxa17216]
 *
 * Revision 1.2  2018/02/14  nxa17216
 * Revision 1.1  2018/01/08  nxa17216
 * Revision 1.0  2012/05/17  r54529
 ******************************************************************************/
