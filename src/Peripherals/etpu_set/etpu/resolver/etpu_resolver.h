/*******************************************************************************
*   (c) Copyright 2004-2016 Freescale Semiconductor, Inc.
*   Copyright 2017-2019 NXP
*   NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
*   in accordance with the applicable license terms. By expressly accepting
*   such terms or by downloading, installing, activating and/or otherwise using
*   the software, you are agreeing that you have read, and that you agree to
*   comply with and are bound by, such license terms. If you do not agree to
*   be bound by the applicable license terms, then you may not retain,
*   install, activate or otherwise use the software.
****************************************************************************//*!
*
* @file    etpu_resolver.h
*
* @author  Marketa Venclikova [nxa17216]
* 
* @version 1.75
* 
* @date    17-Jan-2020
*
* @brief   This file contains useful macros and prototypes for RESOLVER API.
*
*******************************************************************************/
#ifndef _ETPU_RESOLVER_H_
#define _ETPU_RESOLVER_H_

/*******************************************************************************
*                                        INCLUDE FILES
*******************************************************************************/
#include "../_utils/etpu_util.h"        /* 24-bit types */
#include "../_etpu_set/etpu_resolver_auto.h"   /* auto generated header file */

/*******************************************************************************
*                                      DEFINES AND MACROS
*******************************************************************************/

/* Diagnostic Flags Basic */
#define FS_ETPU_RESOLVER_DIAG_FLAG_BASIC_LOS          0x01
#define FS_ETPU_RESOLVER_DIAG_FLAG_BASIC_DOS          0x02
#define FS_ETPU_RESOLVER_DIAG_FLAG_BASIC_LOT          0x04

/* Diagnostic Flag Basic Masks */
#define FS_ETPU_RESOLVER_DIAG_FLAG_BASIC_LOS_MASK  (FS_ETPU_RESOLVER_DIAG_FLAG_VEC_MIN | FS_ETPU_RESOLVER_DIAG_FLAG_STARTUP_SIGNAL)
#define FS_ETPU_RESOLVER_DIAG_FLAG_BASIC_DOS_MASK  (FS_ETPU_RESOLVER_DIAG_FLAG_VEC_MAX | FS_ETPU_RESOLVER_DIAG_FLAG_AMPL_DIFF | FS_ETPU_RESOLVER_DIAG_FLAG_STARTUP_ALIGN)
#define FS_ETPU_RESOLVER_DIAG_FLAG_BASIC_LOT_MASK  (FS_ETPU_RESOLVER_DIAG_FLAG_ATO_ANGLE_ERR | FS_ETPU_RESOLVER_DIAG_FLAG_ATO_SPEED | FS_ETPU_RESOLVER_DIAG_FLAG_STARTUP_SETTLE)

/* Diagnostic Flags Advanced */
#define FS_ETPU_RESOLVER_DIAG_FLAG_ADV_SIN_DISCONNECT 0x01
#define FS_ETPU_RESOLVER_DIAG_FLAG_ADV_SIN_SHORT      0x02
#define FS_ETPU_RESOLVER_DIAG_FLAG_ADV_COS_DISCONNECT 0x04
#define FS_ETPU_RESOLVER_DIAG_FLAG_ADV_COS_SHORT      0x08
#define FS_ETPU_RESOLVER_DIAG_FLAG_ADV_EXC_DISCONNECT 0x10
#define FS_ETPU_RESOLVER_DIAG_FLAG_ADV_EXC_SHORT      0x20

/* Diagnostic Flag Advanced Masks */
/* Should be modified according to input/output circuitry */
/*
#define FS_ETPU_RESOLVER_DIAG_FLAG_ADV_SIN_DISC_MASK  (FS_ETPU_RESOLVER_DIAG_FLAG_SIN_MEAN | FS_ETPU_RESOLVER_DIAG_FLAG_AMPL_DIFF | FS_ETPU_RESOLVER_DIAG_FLAG_ATO_ANGLE_ERR)
#define FS_ETPU_RESOLVER_DIAG_FLAG_ADV_COS_DISC_MASK  (FS_ETPU_RESOLVER_DIAG_FLAG_COS_MEAN | FS_ETPU_RESOLVER_DIAG_FLAG_AMPL_DIFF | FS_ETPU_RESOLVER_DIAG_FLAG_ATO_ANGLE_ERR)
#define FS_ETPU_RESOLVER_DIAG_FLAG_ADV_EXC_DISC_MASK  (FS_ETPU_RESOLVER_DIAG_FLAG_VEC_MIN | FS_ETPU_RESOLVER_DIAG_FLAG_ATO_SPEED)
#define FS_ETPU_RESOLVER_DIAG_FLAG_ADV_SIN_SHORT_MASK (FS_ETPU_RESOLVER_DIAG_FLAG_SIN_MEAN | FS_ETPU_RESOLVER_DIAG_FLAG_ATO_ANGLE_ERR)
#define FS_ETPU_RESOLVER_DIAG_FLAG_ADV_COS_SHORT_MASK (FS_ETPU_RESOLVER_DIAG_FLAG_COS_MEAN | FS_ETPU_RESOLVER_DIAG_FLAG_ATO_ANGLE_ERR)
#define FS_ETPU_RESOLVER_DIAG_FLAG_ADV_EXC_SHORT_MASK (FS_ETPU_RESOLVER_DIAG_FLAG_VEC_MIN)
*/
#define FS_ETPU_RESOLVER_DIAG_FLAG_ADV_SIN_DISC_MASK  (FS_ETPU_RESOLVER_DIAG_FLAG_VEC_MIN | FS_ETPU_RESOLVER_DIAG_FLAG_SIN_LOW_AMPL | FS_ETPU_RESOLVER_DIAG_FLAG_ATO_ANGLE_ERR)
#define FS_ETPU_RESOLVER_DIAG_FLAG_ADV_COS_DISC_MASK  (FS_ETPU_RESOLVER_DIAG_FLAG_VEC_MIN | FS_ETPU_RESOLVER_DIAG_FLAG_COS_LOW_AMPL | FS_ETPU_RESOLVER_DIAG_FLAG_ATO_ANGLE_ERR)
#define FS_ETPU_RESOLVER_DIAG_FLAG_ADV_EXC_DISC_MASK  (FS_ETPU_RESOLVER_DIAG_FLAG_VEC_MIN | FS_ETPU_RESOLVER_DIAG_FLAG_SIN_LOW_AMPL | FS_ETPU_RESOLVER_DIAG_FLAG_COS_LOW_AMPL | FS_ETPU_RESOLVER_DIAG_FLAG_ATO_SPEED)
#define FS_ETPU_RESOLVER_DIAG_FLAG_ADV_SIN_SHORT_MASK (FS_ETPU_RESOLVER_DIAG_FLAG_VEC_MIN | FS_ETPU_RESOLVER_DIAG_FLAG_SIN_LOW_AMPL)
#define FS_ETPU_RESOLVER_DIAG_FLAG_ADV_COS_SHORT_MASK (FS_ETPU_RESOLVER_DIAG_FLAG_VEC_MIN | FS_ETPU_RESOLVER_DIAG_FLAG_COS_LOW_AMPL)
#define FS_ETPU_RESOLVER_DIAG_FLAG_ADV_EXC_SHORT_MASK (FS_ETPU_RESOLVER_DIAG_FLAG_VEC_MIN | FS_ETPU_RESOLVER_DIAG_FLAG_SIN_LOW_AMPL | FS_ETPU_RESOLVER_DIAG_FLAG_COS_LOW_AMPL)


/*******************************************************************************
*                                STRUCTURES AND OTHER TYPEDEFS
*******************************************************************************/

/** A structure to represent an instance of RESOLVER. 
 *  It includes static RESOLVER initialization items. */
struct resolver_instance_t
{
  const uint8_t  chan_num_exc;     /**< Channel number - Excitation signal generation. */
  const uint8_t  chan_num_ato;     /**< Channel number - ATO processing. */
  const uint8_t  chan_num_diag;    /**< Channel number - Diagnostic processing.
    This channel can be one of
     - a channel on the other eTPU engine of the same eTPU module. This enables
       to run the diagnostics in parallel to the position and speed calculation.
     - the same channel as the chan_num_exc. This enables to run both the angle 
       processing and the diagnostics on the same channel. Diagnostics follow 
       the position and speed processing. */
  const uint8_t  chan_num_sample;  /**< Channel number - Sample channel 
     triggering extrapolation:
     - by Host Service Request FS_ETPU_RESOLVER_HSR_SAMPLE
     - by Link from another eTPU function
     - by input transition. */
  const uint8_t  chan_num_dma;     /**< Channel number - channel to generate DMA request
    on ATO update 2nd end to transfer data to LL RDC checker. */
  const uint8_t  priority;         /**< Channel priority. */
  const uint24_t start_offset;     /**< This parameter is used to synchronize 
    various eTPU functions that generate a signal. For RESOLVER: the first  
    excitation signal rising edge is scheduled start_offset TCR1 cycles 
    after initialization. */
  const uint24_t adc_delay;        /**< This parameter determines the delay from
    max amplitude sample to writing FS_ETPU_RESOLVER_HSR_UPDATE. It is used 
    to set the timestamp of calculated and also extrapolated outputs.
    The ADC delay should be measured on a scope - period from sin/cos maximum 
    to RESOLVER+1 channel output pulse (set RESOLVER_DEBUG to 1). 
    This period must be converted to a number of TCR1 cycles. */
        uint32_t *cpba;              /**< Channel parameter base address. 
    Set cpba = 0 to use automatic allocation of eTPU DATA RAM for RESOLVER channel 
    parameters using the eTPU utility function fs_etpu_malloc (recommanded),
    or assign the cpba manually by an address where the RESOLVER channel parameter 
    space will start from, e.g. 0xC3FC8100. */
        uint32_t *signals_pba;       /**< Address of sin, cos and exc signals. 
    A field of 16 samples (one period) stored as uint32_t, is transferred 
    by DMA from ADC to eTPU DATA RAM, for each of the signals, forming the
    following structure in the eTPU DATA RAM:
      uint32_t sin[32]; 
      uint32_t cos[32]; 
    The parameter value *signals_pba points to the address of sin[0].  
    Set signals_pba = 0 to use automatic allocation of eTPU DATA RAM 
    using the eTPU utility function fs_etpu_malloc (recommanded),
    or assign the cpba manually by an address, e.g. 0xC3FC8700. */
        uint32_t *checker_signals_pba; /**< Address of structure with
        data for RDC checker */
};

/** A structure to represent a configuration of RESOLVER. 
 *  It includes RESOLVER configuration items which can be changed in run-time. */
struct resolver_config_t
{
		  uint8_t smpr_id;           /**< semaphore number used to lock data during update:
    - @ref FS_ETPU_RESOLVER_SEMAPHORE_0
    - @ref FS_ETPU_RESOLVER_SEMAPHORE_1
    - @ref FS_ETPU_RESOLVER_SEMAPHORE_2
    - @ref FS_ETPU_RESOLVER_SEMAPHORE_3 */
          uint8_t options;           /**< The available options are:
    - @ref FS_ETPU_RESOLVER_OPTIONS_CALCULATION_ON
    - @ref FS_ETPU_RESOLVER_OPTIONS_EXC_ADAPTATION_ON
    - @ref FS_ETPU_RESOLVER_OPTIONS_EXC_GENERATION_ON
    - @ref FS_ETPU_RESOLVER_OPTIONS_DIAG_MEASURES_ON
    - @ref FS_ETPU_RESOLVER_OPTIONS_RDC_CHECKER_ON
    - @ref FS_ETPU_RESOLVER_OPTIONS_DIAG_FLAGS_ON */
         uint24_t excitation_period; /**< Excitation signal period as a number of TCR1 cycles. */
        fract24_t ato_p_gain;        /**< Angle Tracking Observer P-gain. */
        fract24_t ato_i_gain;        /**< Angle Tracking Observer I-gain. */
        fract24_t exc_p_gain;        /**< Excitation signal adaptive phase shift P-gain */
        fract24_t exc_i_gain;        /**< Excitation signal adaptive phase shift I-gain */
        fract24_t q_ewma_speed;      /**< Speed EWMA filter coefficient (~0.9) */
};

/** A structure to represent calculated or extrapolated RESOLVER outputs. */ 
struct resolver_outputs_t
{
      fract24_t angle; /**< This is the angular motor position  
    as a signed 24-bit fractional value in range (0x800000, 0x7FFFFF)
    corresponding to range (-pi, pi) rads. */
      fract24_t speed; /**< This is the motor speed  
    as a signed 24-bit fractional value in range (0x800000, 0x7FFFFF)
    corresponding to ???. */
        int24_t rev_counter; /**< This is the revolution counter. */
      fract24_t sin; /**< This is the sine of the motor angle  
    as a signed 24-bit fractional value in range (0x800000, 0x7FFFFF). */
      fract24_t cos; /**< This is the cosine of the motor angle  
    as a signed 24-bit fractional value in range (0x800000, 0x7FFFFF). */
        int24_t timestamp; /**< This is the TCR1 count value corresponding 
    to the angular position. */
};

/** A structure to represent RESOLVER diagnostic measures. */ 
struct resolver_diag_measures_t
{
   fract24_t  sin_ampl_1;   /**< The Sin signal amplitude of the first, positive, half period. */
   fract24_t  sin_ampl_2;   /**< The Sin signal amplitude of the second, negative, half period. */
   fract24_t  sin_ampl_min; /**< The Sin signal lowest amplitude ever detected. */
   fract24_t  sin_ampl_max; /**< The Sin signal highest amplitude ever detected. */
   fract24_t  sin_mean;     /**< The Sin signal signal mean value (DC component) . */
   fract24_t  sin_mean_min; /**< The Sin signal lowest mean value ever detected. */
   fract24_t  sin_mean_max; /**< The Sin signal highest mean value ever detected. */
   fract24_t  cos_ampl_1;   /**< The Cos signal amplitude of the first, positive, half period. */
   fract24_t  cos_ampl_2;   /**< The Cos signal amplitude of the second, negative, half period. */
   fract24_t  cos_ampl_min; /**< The Cos signal lowest amplitude ever detected. */
   fract24_t  cos_ampl_max; /**< The Cos signal highest amplitude ever detected. */
   fract24_t  cos_mean;     /**< The Cos signal mean value between ampl_1 and ampl_2. */
   fract24_t  cos_mean_min; /**< The Cos signal lowest mean value ever detected. */
   fract24_t  cos_mean_max; /**< The Cos signal highest mean value ever detected. */
   fract24_t  vec;          /**< The value of a vector formed by Sin and Cos amplitudes as its orthogonal components */
   fract24_t  vec_min;      /**< The lowest value of the vector value ever detected */
   fract24_t  vec_max;      /**< The highest value of the vector value ever detected */
   fract24_t  ampl_diff;    /**< The unit circle deformation as difference between the highest amplitude of Sin and Cos */
};

/** A structure to represent RESOLVER diagnostic thresholds. */ 
struct resolver_diag_thresholds_t
{
   fract24_t ampl_thrs;          /**< Amplitude threshold */
   fract24_t low_ampl_thrs;      /**< Low amplitude threshold */
   fract24_t mean_thrs;          /**< Mean value threshold */
   fract24_t vec_min_thrs;       /**< Sin-Cos vector threshold */
   fract24_t vec_max_thrs;       /**< Sin-Cos vector threshold */
   fract24_t ampl_diff_thrs;     /**< Sin/Cos maximum/minimum amplitude difference threshold */
   fract24_t ato_angle_err_thrs; /**< ATO angle error threshold */
   fract24_t ato_speed_thrs;     /**< ATO speed threshold */
};

/** A structure to represent RESOLVER diagnostic flags. */ 
struct resolver_diag_flags_t
{
   uint24_t  actual;      /**< The actual diagnostic flags. The bit flags are:
    - @ref FS_ETPU_RESOLVER_DIAG_FLAG_SIN_AMPL 
    - @ref FS_ETPU_RESOLVER_DIAG_FLAG_COS_AMPL 
    - @ref FS_ETPU_RESOLVER_DIAG_FLAG_SIN_MEAN     
    - @ref FS_ETPU_RESOLVER_DIAG_FLAG_COS_MEAN     
    - @ref FS_ETPU_RESOLVER_DIAG_FLAG_VEC_MIN      
    - @ref FS_ETPU_RESOLVER_DIAG_FLAG_VEC_MAX      
    - @ref FS_ETPU_RESOLVER_DIAG_FLAG_AMPL_DIFF    
    - @ref FS_ETPU_RESOLVER_DIAG_FLAG_ATO_ANGLE_ERR
    - @ref FS_ETPU_RESOLVER_DIAG_FLAG_ATO_SPEED     */
   uint24_t  cumulative;  /**< The cumulative diagnostic flags. 
    The bit flags are similar to the actual flags. */
};

/** A structure to represent RESOLVER internal states. */ 
struct resolver_states_t
{
  int24_t  exc_phase_shift;
  int24_t  exc_zero_cross_err;
  int24_t  exc_pi_acc;
  int24_t  ato_sig_sin;
  int24_t  ato_sig_cos;
  int24_t  ato_acc;
  int24_t  ato_angle_error;
  int24_t  ato_angle;
  int24_t  ato_speed;
  uint8_t  upd;
  uint8_t  state;
};


/*******************************************************************************
*                                    FUNCTION PROTOTYPES
*******************************************************************************/

/* Initialize */
uint32_t fs_etpu_resolver_init(
  struct resolver_instance_t *p_resolver_instance,
  struct resolver_config_t   *p_resolver_config);

/* Change configuration */
uint32_t fs_etpu_resolver_config(
  struct resolver_instance_t *p_resolver_instance,
  struct resolver_config_t   *p_resolver_config);

/* Get outputs */
uint32_t fs_etpu_resolver_get_outputs_calculated(
  struct resolver_instance_t *p_resolver_instance,
  struct resolver_outputs_t  *p_resolver_outputs_calculated);

uint32_t fs_etpu_resolver_get_outputs_extrapolated(
  struct resolver_instance_t *p_resolver_instance,
  struct resolver_outputs_t  *p_resolver_outputs_extrapolated);

/* Transform outputs */
uint32_t fs_etpu_resolver_trans_outputs_el_to_mech(
  int32_t pole_pairs,
  struct resolver_outputs_t  *p_resolver_outputs_el,
  struct resolver_outputs_t  *p_resolver_outputs_mech);

/* Sample outputs (trigger extrapolation) */
uint32_t fs_etpu_resolver_sample(
  struct resolver_instance_t      *p_resolver_instance);

/* Get diagnostic measures */
uint32_t fs_etpu_resolver_get_diag_measures(
  struct resolver_instance_t      *p_resolver_instance,
  struct resolver_diag_measures_t *p_resolver_diag_measures);

/* Get diagnostic flags */
uint32_t fs_etpu_resolver_get_diag_flags(
  struct resolver_instance_t   *p_resolver_instance,
  struct resolver_diag_flags_t *p_resolver_diag_flags);

/* Decode basic diagnostic flags */
uint8_t fs_etpu_resolver_decode_diag_flags_basic(
  struct resolver_diag_flags_t *p_resolver_diag_flags);

/* Decode advanced diagnostic flags */
uint8_t fs_etpu_resolver_decode_diag_flags_advanced(
  struct resolver_diag_flags_t *p_resolver_diag_flags);

/* Reset diagnostics */
uint32_t fs_etpu_resolver_diag_reset(
  struct resolver_instance_t      *p_resolver_instance);

/* Set diagnostic thresholds */
uint32_t fs_etpu_resolver_set_thresholds(
  struct resolver_instance_t        *p_resolver_instance,
  struct resolver_diag_thresholds_t *p_resolver_diag_thresholds);

/* Calibrate diagnostic thresholds */
uint32_t fs_etpu_resolver_diag_calibrate(
  struct resolver_instance_t        *p_resolver_instance,
  struct resolver_diag_measures_t   *p_resolver_diag_measures,
  struct resolver_diag_thresholds_t *p_resolver_diag_thresholds);

/* Get states */
uint32_t fs_etpu_resolver_get_states(
  struct resolver_instance_t *p_resolver_instance,
  struct resolver_states_t   *p_resolver_states);



#endif /* _ETPU_RESOLVER_H_ */

/*******************************************************************************
*
 * REVISION HISTORY:
 *
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
 * Revision 1.61 2017/05/04  r54529
 * Filtered vector value added. Vector threshold values not squared.
 * Revision 1.6 2017/04/28   r54529
 * State machine added, which enables diagnostics even after EXC and ATO settles.
 * Structure resolver_states_t updated.  
 * Revision 1.5 2017/04/07   r54529
 * q_ewma_speed added (Speed EWMA filter).
 * Revision 1.41 2017/04/03  r54529
 * Fix of diagnostics not executed.
 * Revision 1.4  2017/03/22  r54529
 * Sample channel added for triggering extrapolation.
 * Revision 1.2  2016/03/08  r54529
 * New improved diagnostics.
 * Revision 1.0  2015/09/30  r54529
 * Initial version of file.
 ******************************************************************************/

