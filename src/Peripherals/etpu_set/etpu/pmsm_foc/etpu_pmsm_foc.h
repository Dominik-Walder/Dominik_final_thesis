/*******************************************************************************
*
*   Copyright 2017 - 2020 NXP
*   NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
*   in accordance with the applicable license terms. By expressly accepting
*   such terms or by downloading, installing, activating and/or otherwise using
*   the software, you are agreeing that you have read, and that you agree to
*   comply with and are bound by, such license terms. If you do not agree to
*   be bound by the applicable license terms, then you may not retain,
*   install, activate or otherwise use the software.
****************************************************************************//*!
*
* @file    etpu_pmsm_foc.c
*
* @author  Marketa Venclikova [nxa17216]
* 
* @version 1.3
* 
* @date    25-October-2019
*
* @brief   This file contains API for using the eTPU function
*          PMSM Field Oriented Control (FOC).
*
*******************************************************************************/
#ifndef _ETPU_PMSM_FOC_H_
#define _ETPU_PMSM_FOC_H_

/*******************************************************************************
*                                        INCLUDE FILES
*******************************************************************************/
#include "../_utils/etpu_util.h"       /* 24-bit types */
#include "../_etpu_set/etpu_pmsm_foc_auto.h"     /* auto generated header file */
#include "../pwmm/etpu_pwmm.h"    /* access to PWMM parameter offsets */
#include "../as/etpu_as.h"    /* access to AS parameter offsets */
#include "../resolver/etpu_resolver.h"    /* access to Resolver parameter offsets */

/*******************************************************************************
*                                      DEFINES AND MACROS
*******************************************************************************/


/*******************************************************************************
*                                STRUCTURES AND OTHER TYPEDEFS
*******************************************************************************/
/** A structure to represent an instance of PMSM FOC. 
 *  It includes static PMSM FOC initialization items. */
struct pmsm_foc_instance_t
{
  const uint8_t  chan_num;  /**< Channel number of the PMSM FOC channel. */
  const uint8_t  priority;  /**< Channel priority for the PMSM FOC channel. */
  const uint8_t  mode;      /**< PMSM FOC mode. 
    It can be any of:
    - @ref FS_ETPU_PMSM_FOC_FM0_MODE_PERIODIC - PMSM FOC calculation is based on 
        start_offset and period parameters
    - @ref FS_ETPU_PMSM_FOC_FM0_MODE_SYNC     - PMSM FOC calculation is based on
        link from another channel */
  const  uint8_t motor_pole_pair; /**< Motor pole pair number  */
  const  uint8_t resolver_pole_pair; /**< Resolver pole pair number  */
  const uint24_t start_offset;  /**< This parameter is used to synchronize 
    various eTPU functions. For PMSM FOC: the first calculation is scheduled 
	start_offset TCR1 cycles after initialization. */
        uint32_t *cpba;     /**< Channel parameter base address. 
    Set cpba = 0 to use automatic allocation of eTPU DATA RAM for PMSM FOC channel 
    parameters using the eTPU utility function fs_etpu_malloc (recommanded),
    or assign the cpba manually by an address where the PMSM FOC channel parameter 
    space will start from, e.g. 0xC3FC8100. */
  const	fract24_t u_dcb_max;	/**< Maximun DC Bus voltage in the same fractional format
		and scale as *u_dcb. */
};

/** A structure to represent a configuration of PMSM FOC. 
 *  It includes PMSM FOC configuration items which can mostly be changed in 
 *  run-time. */
struct pmsm_foc_config_t
{
        uint24_t period;             /**< PMSM FOC period as a number of TCR1 cycles. */
        uint24_t speed_scale;        /**< Speed scale factor. Scaling from RPM (resolver exc. frequency [Hz] * 60) to max speed of motor [RPM] */
        uint8_t ctrl_mode;			 /**< Control structure mode.
        	Can be one of:
        - @ref FS_ETPU_PMSM_FOC_CTRL_MODE_SCALAR
        - @ref FS_ETPU_PMSM_FOC_CTRL_MODE_VOLTAGE
        - @ref FS_ETPU_PMSM_FOC_CTRL_MODE_CURRENT
        - @ref FS_ETPU_PMSM_FOC_CTRL_MODE_SPEED */
        fract24_t d_k_p_gain;        /**< D voltage regulator K_p-gain */
        fract24_t d_k_i_gain;        /**< D voltage regulator K_i-gain */
		fract24_t q_k_p_gain;        /**< Q voltage regulator K_p-gain */
        fract24_t q_k_i_gain;        /**< Q voltage regulator K_i-gain */
		fract24_t speed_k_p_gain;        /**< Speed regulator K_p-gain */
        fract24_t speed_k_i_gain;        /**< Speed regulator K_i-gain */
		fract24_t speed_ramp_up;         /**< Speed ramp UP */
        fract24_t speed_ramp_down;       /**< Speed ramp DOWN */
        fract24_t scalar_angle_k_p_gain;        /**< Scalar angle regulator K_p-gain */
        fract24_t scalar_angle_k_i_gain;        /**< Scalar angle regulator K_i-gain */
        fract24_t scalar_u_rpm_factor;     /**< U_RPM factor: The ratio of nominal motor voltage and nominal frequency for scalar control only */
		fract24_t res_angle_offset;/**< Value of the electrical angle when Resolver angle is 0 */
};

struct pmsm_foc_inputs_t
{
		fract24_t d_required;	/**< D required current.*/ 
		fract24_t q_required;   /**< Q required current.*/ 
		fract24_t speed_required;   /**< Required mechanical speed.*/
};

/** A structure to represent states of PMSM FOC processing. */
struct pmsm_foc_states_t
{
        fract24_t i_a;  /**< Input phase A current.*/ 
		fract24_t i_b;  /**< Input phase B current.*/ 
		fract24_t i_c;  /**< Input phase C current.*/  
		fract24_t i_alpha; /**< Alpha component of motor current in stationary coordinates.*/
		fract24_t i_beta;  /**< Beta component of motor current in stationary coordinates.*/
		fract24_t i_d;  /**< D component of motor current in rotary coordinates.*/ 
		fract24_t i_q;  /**< Q component of motor current in rotary coordinates.*/  
        fract24_t u_d;  /**< D component of applied motor voltage in rotary coordinates.*/ 
		fract24_t u_q;  /**< Q component of applied motor voltage in rotary coordinates.*/  
		fract24_t u_alpha; /**< Alpha component of applied motor voltage in stationary coordinates.*/
		fract24_t u_beta;  /**< Beta component of applied motor voltage in stationary coordinates.*/
		fract24_t u_alpha_relim; /**< Alpha component after ripple elimination in stationary coordinates.*/
		fract24_t u_beta_relim;  /**< Beta component after ripple elimination in stationary coordinates.*/
		fract24_t u_dcb;   /**< DC bus voltage.*/
		fract24_t angle_el; /**< Electrical angle.*/
		fract24_t advanced_angle; /**< The angle that is used in high speed to avoid losing the torque at high speed - Electrical angle.*/
		fract24_t scalar_angle_el; /**< The angle that is used in Scalar control.*/
		fract24_t speed_required_ramp;   /**< Required mechanical ramped speed.*/
};


/*******************************************************************************
*                                    FUNCTION PROTOTYPES
*******************************************************************************/
/* Initialize */
uint32_t fs_etpu_pmsm_foc_init(
  struct pmsm_foc_instance_t   *p_pmsm_foc_instance,
  struct pwmm_instance_t *p_pwmm_instance,
  struct resolver_instance_t *p_resolver_instance,
  struct as_instance_t *p_as_instance,
  struct pmsm_foc_config_t     *p_pmsm_foc_config);

/* Change configuration */
uint32_t fs_etpu_pmsm_foc_config(
  struct pmsm_foc_instance_t *p_pmsm_foc_instance,
  struct pmsm_foc_config_t   *p_pmsm_foc_config);

/* Clear controller variables */
uint32_t fs_etpu_pmsm_foc_clear_controller(
  struct pmsm_foc_instance_t *p_pmsm_foc_instance);

/* Set inputs */
uint32_t fs_etpu_pmsm_foc_set_inputs(
  struct pmsm_foc_instance_t *p_pmsm_foc_instance,
  struct pmsm_foc_inputs_t   *p_pmsm_foc_inputs);

/* Get states */
uint32_t fs_etpu_pmsm_foc_get_states(
  struct pmsm_foc_instance_t *p_pmsm_foc_instance,
  struct pmsm_foc_states_t  *p_pmsm_foc_states);


#endif /* _ETPU_PMSM_FOC_H_ */

/*******************************************************************************
 *
 * REVISION HISTORY:
 *
 * FILE OWNER: Marketa Venclikova [nxa17216]
 * Revision 1.3  2019/10/25  nxa17216
 * Updates related to eTPU function redesign:
 * - added advanced time parameters, match time changed to frame time, added pointer
 * to PWMM period, added ctrl_mode parameter to allow 4 function modes for debugging
 * - added structure and parameters for scalar control
 * Revision 1.2  2019/02/25  nxa17216
 * - Added pointer to third PWMM input U_c
 * - Parameter motor_resolver_pole_pair_ratio has been divided into two separate
 * parameters: motor_pole_pair and  resolver_pole_pair
 * Revision 1.1  2018/01/25  b52107
 * - Added speed controller parameters.
 * - Added function to clear controller integration variables
 * fs_etpu_pmsm_foc_clear_controller();
 *
 * Revision 1.0  2017/09/06  b52107
 * Initial version of file.
 ******************************************************************************/
