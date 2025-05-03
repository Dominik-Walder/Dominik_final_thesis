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
* @author  Marketa Venclikova [b52107]
* 
* @version 1.3
* 
* @date    25-October-2019
*
* @brief   This file contains API for using the eTPU function
*          PMSM Field Oriented Control (FOC).
* 
****************************************************************************//*!
*
* @mainpage
*
* The eTPU PMSM FOC APIs @ref etpu_PMSM_FOC.c/.h includes API functions for eTPU
* function PMSM Field Oriented Control.
* 
*******************************************************************************/
/*******************************************************************************
*                                        INCLUDE FILES
*******************************************************************************/
#include "etpu_pmsm_foc.h"      /* private header file */
#include "../_utils/etpu_util.h"    /* utility routines for working with the eTPU */
#include "../pwmm/etpu_pwmm.h"    /* access to PWMM parameter offsets */
#include "../as/etpu_as.h"    /* access to AS parameter offsets */
#include "../resolver/etpu_resolver.h"    /* access to Resolver parameter offsets */

/*******************************************************************************
*                                      GLOBAL VARIABLES
*******************************************************************************/
extern uint32_t fs_etpu_data_ram_start;
extern uint32_t fs_etpu_data_ram_ext;

/*******************************************************************************
*                                       GLOBAL FUNCTIONS
*******************************************************************************/
/*******************************************************************************
* FUNCTION: fs_etpu_pmsm_foc_init
****************************************************************************//*!
* @brief   This function initializes eTPU channels to run PMSM FOC function.
*
* @note    The following actions are performed in order:
*          -# Use user-defined CPBA or allocate new eTPU DATA RAM
*          -# Write chan config registers and FM bits
*          -# Write channel parameters
*          -# Write HSR
*          -# Set channel priority
*
* @warning The PWMM channel eTPU DATA RAM allocation, AS channel eTPU DATA RAM 
*          allocation and the RESOLVER channel eTPU DATA RAM allocation must be 
*          done before the PMSM FOC channel initialization (using 
*          fs_etpu_pmsm_foc_init). On PMSM FOC initialization, pointers to PWMM,
*          AS and RESOLVER parameters are created.
*
* @param   *p_pmsm_foc_instance - This is a pointer to the instance structure 
*            @ref pmsm_foc_instance_t.
* @param   *p_pwmm_instance - This is a pointer to the instance structure 
*            @ref pwmm_instance_t. It is used due to interaction between PMSM FOC
*            and PWMM eTPU functions - writing the FOC output alpha and beta values
*            as the input to the PWMM Space vector modulation.
* @param   *p_resolver_instance - This is a pointer to the instance structure 
*            @ref resolver_instance_t. It is used due to interaction
*            between PMSM FOC and RESOLVER eTPU functions - reading of the angle
*            needed for the motor control transformation functions.
* @param   *p_as_instance - This is a pointer to the instance structure 
*            @ref as_instance_t. It is used due to interaction between PMSM FOC
*            and AS eTPU functions - FOC reads outputs of the AS signal processing.
* @param   *p_pmsm_foc_config - This is a pointer to the structure of configuration
*            parameters @ref pmsm_foc_config_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_MALLOC - eTPU DATA RAM memory allocation error
*          - @ref FS_ETPU_ERROR_NONE - No error
*
* @warning This function does not configure the pins, only the eTPU channels.
*******************************************************************************/
uint32_t fs_etpu_pmsm_foc_init(
  struct pmsm_foc_instance_t *p_pmsm_foc_instance,
  struct pwmm_instance_t *p_pwmm_instance,
  struct resolver_instance_t *p_resolver_instance,
  struct as_instance_t *p_as_instance,
  struct pmsm_foc_config_t     *p_pmsm_foc_config)
{
  uint8_t  chan_num;
  uint8_t  priority;
  uint32_t *cpba;
  uint32_t cpba_resolver_local;
  uint32_t cpba_as_signals_local;
  uint32_t cpba_pwmm_local;

  chan_num     = p_pmsm_foc_instance->chan_num;   
  priority     = p_pmsm_foc_instance->priority;                
  cpba         = p_pmsm_foc_instance->cpba;      
  cpba_resolver_local   = (uint32_t)p_resolver_instance->cpba - fs_etpu_data_ram_start;
  cpba_as_signals_local = (uint32_t)p_as_instance->cpba_signals - fs_etpu_data_ram_start;  
  cpba_pwmm_local       = (uint32_t)p_pwmm_instance->cpba - fs_etpu_data_ram_start;
  
  /* Use user-defined CPBA or allocate new eTPU DATA RAM */
  if(cpba == 0)
  {
    cpba = fs_etpu_malloc(FS_ETPU_PMSM_FOC_NUM_PARMS);
    if(cpba == 0)
    {
      return(FS_ETPU_ERROR_MALLOC);
    }
    else
    {
      p_pmsm_foc_instance->cpba = cpba;
    }
  }

  
  /* Write chan config registers and FM bits */
  eTPU->CHAN[chan_num].CR.R = (FS_ETPU_PMSM_FOC_TABLE_SELECT << 24) +
       (FS_ETPU_PMSM_FOC_FUNCTION_NUMBER << 16) +
       (((uint32_t)cpba - fs_etpu_data_ram_start) >> 3); 
  eTPU->CHAN[chan_num].SCR.R = (uint32_t)p_pmsm_foc_instance->mode;

  /* Write channel parameters */
  /* 24-bit */
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_START_OFFSET            - 1)>>2)) = p_pmsm_foc_instance->start_offset;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_PERIOD                  - 1)>>2)) = p_pmsm_foc_config->period;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_FRAME_TIME              - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_SPEED_SCALE             - 1)>>2)) = p_pmsm_foc_config->speed_scale;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_P_PWMM_PERIOD           - 1)>>2)) = cpba_pwmm_local + FS_ETPU_PWMM_OFFSET_PERIOD;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_ADVANCED_ANGLE_TIME     - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_ADVANCED_ANGLE          - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_RES_ANGLE_OFFSET        - 1)>>2)) = p_pmsm_foc_config->res_angle_offset;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_P_I_A                   - 1)>>2)) = cpba_as_signals_local + 0*FS_ETPU_AS_SIGNAL_STRUCT_SIZE + FS_ETPU_AS_OFFSET_VALUE;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_P_I_B                   - 1)>>2)) = cpba_as_signals_local + 1*FS_ETPU_AS_SIGNAL_STRUCT_SIZE + FS_ETPU_AS_OFFSET_VALUE;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_P_I_C                   - 1)>>2)) = cpba_as_signals_local + 2*FS_ETPU_AS_SIGNAL_STRUCT_SIZE + FS_ETPU_AS_OFFSET_VALUE;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_P_U_DCB                 - 1)>>2)) = cpba_as_signals_local + 3*FS_ETPU_AS_SIGNAL_STRUCT_SIZE + FS_ETPU_AS_OFFSET_VALUE;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_U_DCB_MAX               - 1)>>2)) = p_pmsm_foc_instance->u_dcb_max;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_P_U_A                   - 1)>>2)) = cpba_pwmm_local + FS_ETPU_PWMM_OFFSET_INPUTS_A;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_P_U_B                   - 1)>>2)) = cpba_pwmm_local + FS_ETPU_PWMM_OFFSET_INPUTS_B;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_P_U_C                   - 1)>>2)) = cpba_pwmm_local + FS_ETPU_PWMM_OFFSET_INPUTS_C;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_P_ANGLE                 - 1)>>2)) = cpba_resolver_local + FS_ETPU_RESOLVER_OFFSET_EXTRAPOLATED_ANGLE;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_ANGLE_EL                - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_U_DCB	                 - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_P_SPEED                 - 1)>>2)) = cpba_resolver_local + FS_ETPU_RESOLVER_OFFSET_EXTRAPOLATED_SPEED;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_SPEED                   - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_SPEED_REQUIRED          - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_SPEED_REQUIRED_RAMP     - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_SPEED_RAMP_PARAMS_STATE - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_SPEED_RAMP_PARAMS_UP    - 1)>>2)) = p_pmsm_foc_config->speed_ramp_up;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_SPEED_RAMP_PARAMS_DOWN  - 1)>>2)) = p_pmsm_foc_config->speed_ramp_down;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_I_ABC_A	             - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_I_ABC_B                 - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_I_ABC_C                 - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_I_AB_ALPHA              - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_I_AB_BETA               - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_I_DQ_D                  - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_I_DQ_Q                  - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_U_DQ_D                  - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_U_DQ_Q                  - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_U_AB_ALPHA              - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_U_AB_BETA               - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_U_AB_RELIM_ALPHA        - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_U_AB_RELIM_BETA         - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_SIN_COS_SIN             - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_SIN_COS_COS             - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_PI_D_K_P                - 1)>>2)) = p_pmsm_foc_config->d_k_p_gain;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_PI_D_K_I                - 1)>>2)) = p_pmsm_foc_config->d_k_i_gain;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_PI_D_ERROR              - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_PI_D_ACC_INTEGRAL       - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_PI_Q_K_P                - 1)>>2)) = p_pmsm_foc_config->q_k_p_gain;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_PI_Q_K_I                - 1)>>2)) = p_pmsm_foc_config->q_k_i_gain;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_PI_Q_ERROR              - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_PI_Q_ACC_INTEGRAL       - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_PI_SPEED_K_P            - 1)>>2)) = p_pmsm_foc_config->speed_k_p_gain;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_PI_SPEED_K_I            - 1)>>2)) = p_pmsm_foc_config->speed_k_i_gain;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_PI_SPEED_ERROR          - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_PI_SPEED_ACC_INTEGRAL   - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_D_REQUIRED              - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_Q_REQUIRED              - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_SCALAR_CTRL_ANGLE_EL              - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_SCALAR_CTRL_SPEED_REQUIRED        - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_SCALAR_CTRL_UM_REQ        - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_SCALAR_CTRL_U_RPMM_FACTOR        - 1)>>2)) = p_pmsm_foc_config->scalar_u_rpm_factor;;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_SCALAR_CTRL_PI_K_P                - 1)>>2)) = p_pmsm_foc_config->scalar_angle_k_p_gain;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_SCALAR_CTRL_PI_K_I                - 1)>>2)) = p_pmsm_foc_config->scalar_angle_k_i_gain;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_SCALAR_CTRL_PI_ERROR        - 1)>>2)) = 0;
  *(cpba + ((FS_ETPU_PMSM_FOC_OFFSET_SCALAR_CTRL_PI_ACC_INTEGRAL        - 1)>>2)) = 0;


  /* 8-bit */

  *((uint8_t*)cpba + FS_ETPU_PMSM_FOC_OFFSET_MOTOR_POLE_PAIR ) = p_pmsm_foc_instance->motor_pole_pair;
  *((uint8_t*)cpba + FS_ETPU_PMSM_FOC_OFFSET_RESOLVER_POLE_PAIR ) = p_pmsm_foc_instance->resolver_pole_pair;
  *((uint8_t*)cpba + FS_ETPU_PMSM_FOC_OFFSET_SPEED_LOOP_CNT ) = 0;
  *((uint8_t*)cpba + FS_ETPU_PMSM_FOC_OFFSET_PWMM_CHAN ) = p_pwmm_instance->chan_num_master + 64; /* used to link PWMM */
  *((uint8_t*)cpba + FS_ETPU_PMSM_FOC_OFFSET_CTRL_MODE ) = p_pmsm_foc_config->ctrl_mode;

  /* Write HSR */  
  eTPU->CHAN[chan_num].HSRR.R = FS_ETPU_PMSM_FOC_HSR_INIT;
  
  /* Set channel priority */
  fs_etpu_enable(chan_num, priority);
  
  return(FS_ETPU_ERROR_NONE);
}

/*******************************************************************************
* FUNCTION: fs_etpu_pmsm_foc_config
****************************************************************************//*!
* @brief   This function changes the PMSM FOC configuration.
*
* @note    The following actions are performed in order:
*          -# Write configuration parameter values to eTPU DATA RAM
*
* @param   *p_pmsm_foc_instance - This is a pointer to the instance structure 
*            @ref pmsm_foc_instance_t.
* @param   *p_pmsm_foc_config - This is a pointer to the structure of configuration
*            parameters @ref pmsm_foc_config_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_NONE - No error
*
*******************************************************************************/
uint32_t fs_etpu_pmsm_foc_config(
  struct pmsm_foc_instance_t *p_pmsm_foc_instance,
  struct pmsm_foc_config_t   *p_pmsm_foc_config)
{
  uint32_t *cpba;
  uint32_t *cpbae;


  cpba = p_pmsm_foc_instance->cpba;                       
  cpbae = cpba + (0x4000 >> 2); /* sign-extended memory area */

  /* Write channel parameters */
  /* 24-bit - use cpbae to prevent from overwriting bits 31:24 */
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_PERIOD                  - 1)>>2)) = p_pmsm_foc_config->period;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_SPEED_SCALE             - 1)>>2)) = p_pmsm_foc_config->speed_scale;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_PI_D_K_P                - 1)>>2)) = p_pmsm_foc_config->d_k_p_gain;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_PI_D_K_I                - 1)>>2)) = p_pmsm_foc_config->d_k_i_gain;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_PI_Q_K_P                - 1)>>2)) = p_pmsm_foc_config->q_k_p_gain;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_PI_Q_K_I                - 1)>>2)) = p_pmsm_foc_config->q_k_i_gain;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_PI_SPEED_K_P            - 1)>>2)) = p_pmsm_foc_config->speed_k_p_gain;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_PI_SPEED_K_I            - 1)>>2)) = p_pmsm_foc_config->speed_k_i_gain;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_SPEED_RAMP_PARAMS_UP    - 1)>>2)) = p_pmsm_foc_config->speed_ramp_up;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_SPEED_RAMP_PARAMS_DOWN  - 1)>>2)) = p_pmsm_foc_config->speed_ramp_down;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_SCALAR_CTRL_PI_K_P                - 1)>>2)) = p_pmsm_foc_config->scalar_angle_k_p_gain;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_SCALAR_CTRL_PI_K_I                - 1)>>2)) = p_pmsm_foc_config->scalar_angle_k_i_gain;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_SCALAR_CTRL_U_RPMM_FACTOR        - 1)>>2)) = p_pmsm_foc_config->scalar_u_rpm_factor;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_RES_ANGLE_OFFSET        - 1)>>2)) = p_pmsm_foc_config->res_angle_offset;


  *((uint8_t*)cpba + FS_ETPU_PMSM_FOC_OFFSET_CTRL_MODE ) = p_pmsm_foc_config->ctrl_mode;
  return(FS_ETPU_ERROR_NONE);
}

/*******************************************************************************
* FUNCTION: fs_etpu_pmsm_foc_config
****************************************************************************//*!
* @brief   This function clears the PMSM FOC regulator variables.
*
* @note    The following actions are performed in order:
*          -# Clear controller variable values in eTPU DATA RAM
*
* @param   *p_pmsm_foc_instance - This is a pointer to the instance structure
*            @ref pmsm_foc_instance_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_NONE - No error
*
*******************************************************************************/
uint32_t fs_etpu_pmsm_foc_clear_controller(
  struct pmsm_foc_instance_t *p_pmsm_foc_instance)
{
  uint32_t *cpba;
  uint32_t *cpbae;


  cpba = p_pmsm_foc_instance->cpba;
  cpbae = cpba + (0x4000 >> 2); /* sign-extended memory area */

  /* Write channel parameters */
  /* 24-bit - use cpbae to prevent from overwriting bits 31:24 */
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_PI_D_ACC_INTEGRAL           - 1)>>2)) = 0;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_PI_Q_ACC_INTEGRAL           - 1)>>2)) = 0;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_PI_SPEED_ACC_INTEGRAL       - 1)>>2)) = 0;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_SCALAR_CTRL_PI_ACC_INTEGRAL - 1)>>2)) = 0;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_U_DQ_D                      - 1)>>2)) = 0;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_U_DQ_Q                      - 1)>>2)) = 0;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_SPEED_REQUIRED              - 1)>>2)) = 0;


  return(FS_ETPU_ERROR_NONE);
}

/*******************************************************************************
* FUNCTION: fs_etpu_pmsm_foc_set_inputs
****************************************************************************//*!
* @brief   This function sets the inputs of the PMSM FOC from the CPU.
*
* @note    The following actions are performed in order:
*          -# Write input parameter values to eTPU DATA RAM
*
* @param   *p_pmsm_foc_instance - This is a pointer to the instance structure 
*            @ref pmsm_foc_instance_t.
* @param   *p_pmsm_foc_inputs - This is a pointer to the structure of inputs
*            @ref pmsm_foc_inputs_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_NONE - No error
*
*******************************************************************************/
uint32_t fs_etpu_pmsm_foc_set_inputs(
  struct pmsm_foc_instance_t *p_pmsm_foc_instance,
  struct pmsm_foc_inputs_t   *p_pmsm_foc_inputs)
{
  uint32_t *cpba;
  uint32_t *cpbae;

  cpba = p_pmsm_foc_instance->cpba;                       
  cpbae = cpba + (0x4000 >> 2); /* sign-extended memory area */

  /* Write channel parameters */
  /* 24-bit - use cpbae to prevent from overwriting bits 31:24 */
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_Q_REQUIRED    - 1)>>2)) = p_pmsm_foc_inputs->q_required;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_D_REQUIRED    - 1)>>2)) = p_pmsm_foc_inputs->d_required;
  *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_SPEED_REQUIRED- 1)>>2)) = p_pmsm_foc_inputs->speed_required;
  
  return(FS_ETPU_ERROR_NONE);	
}

/*******************************************************************************
* FUNCTION: fs_etpu_pmsm_foc_get_states
****************************************************************************//*!
* @brief   This function reads the PMSM FOC state variables.
*
* @note    The following actions are performed in order:
*          -# Read state parameter values from eTPU DATA RAM
*
* @param   *p_pmsm_foc_instance - This is a pointer to the instance structure 
*            @ref pmsm_foc_instance_t.
* @param   *p_pmsm_foc_states - This is a pointer to the structure of states
*            @ref pmsm_foc_states_t.
*
* @return  Error codes that can be returned are:
*          - @ref FS_ETPU_ERROR_NONE - No error
*
*******************************************************************************/
/* Get states */
uint32_t fs_etpu_pmsm_foc_get_states(
  struct pmsm_foc_instance_t *p_pmsm_foc_instance,
  struct pmsm_foc_states_t  *p_pmsm_foc_states)
{
  uint32_t *cpba;
  uint32_t *cpbae;

  cpba = p_pmsm_foc_instance->cpba;                       
  cpbae = cpba + (0x4000 >> 2); /* sign-extended memory area */
  
  p_pmsm_foc_states->i_a     = *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_I_ABC_A                         - 1)>>2));
  p_pmsm_foc_states->i_b     = *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_I_ABC_B                         - 1)>>2));
  p_pmsm_foc_states->i_c     = *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_I_ABC_C                         - 1)>>2));
  p_pmsm_foc_states->i_alpha = *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_I_AB_ALPHA                      - 1)>>2));
  p_pmsm_foc_states->i_beta  = *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_I_AB_BETA                       - 1)>>2));
  p_pmsm_foc_states->i_d     = *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_I_DQ_D                          - 1)>>2));
  p_pmsm_foc_states->i_q     = *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_I_DQ_Q                          - 1)>>2));
  p_pmsm_foc_states->u_d     = *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_U_DQ_D                          - 1)>>2));
  p_pmsm_foc_states->u_q     = *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_U_DQ_Q                          - 1)>>2));
  p_pmsm_foc_states->u_dcb   = *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_U_DCB                           - 1)>>2));
  p_pmsm_foc_states->u_alpha = *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_U_AB_ALPHA                      - 1)>>2));
  p_pmsm_foc_states->u_beta  = *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_U_AB_BETA                       - 1)>>2));
  p_pmsm_foc_states->u_alpha_relim = *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_U_AB_RELIM_ALPHA          - 1)>>2));
  p_pmsm_foc_states->u_beta_relim  = *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_U_AB_RELIM_BETA           - 1)>>2));
  p_pmsm_foc_states->angle_el = *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_ANGLE_EL                       - 1)>>2));
  p_pmsm_foc_states->advanced_angle = *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_ADVANCED_ANGLE           - 1)>>2));
  p_pmsm_foc_states->scalar_angle_el = *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_SCALAR_CTRL_ANGLE_EL    - 1)>>2));
  p_pmsm_foc_states->speed_required_ramp = *(cpbae + ((FS_ETPU_PMSM_FOC_OFFSET_SPEED_REQUIRED_RAMP - 1)>>2));

  
  return(FS_ETPU_ERROR_NONE);
}
  
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
 * Revision 1.1  2018/01/25  nxa17216
 * - Added speed controller parameters.
 * - Added function to clear controller integration variables
 * fs_etpu_pmsm_foc_clear_controller();
 *
 * Revision 1.0  2017/09/06  nxa17216
 * Initial version of file.
 ******************************************************************************/
