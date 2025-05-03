/*******************************************************************************
*
* Copyright 2006-2015 Freescale Semiconductor, Inc.
* Copyright 2016-2020 NXP
*
****************************************************************************//*!
*
* @file     actuate_mpc57xx.c
*
* @date     October-20-2020
*
* @brief    Header file for actuator module
*
*******************************************************************************/

/******************************************************************************
* Included files
*******************************************************************************/
#include "actuate_mpc57xx.h"
#include "state_machine.h"
#include "amclib.h"
#include "gflib.h"
#include "gmclib.h"
#include "gdflib.h"
#include "peripherals\etpu_set\etpu\pwmm\etpu_pwmm.h"  /* eTPU PWMM function API */
#include "peripherals\etpu_set\etpu_gct.h"   /* eTPU configuration */
#include "MC33937/MC33937.h"

/******************************************************************************
* Global variables
******************************************************************************/
 extern MC33937_T			MC33937;

/******************************************************************************
* Global functions
******************************************************************************/
/**************************************************************************//*!
* @brief    Enable PWM output and set 50% duty cycle
*
* @return   tBool
*
*******************************************************************************/
tBool ACTUATE_EnableOutput(void)
{
	SWLIBS_3Syst_FLT fltpwm;

	tBool statePWM;

	/* Apply 0.5 duty cycle */
	fltpwm.fltArg1 = 0.5F;
	fltpwm.fltArg2 = 0.5F;
	fltpwm.fltArg3 = 0.5F;
	
	statePWM = ACTUATE_SetDutycycle(&fltpwm);

    /* Enable PWM and gate driver */
	 fs_etpu_pwmm_enable(&pwmm_instance);
	 MC33937_enable_output(&MC33937);

	 return(statePWM);
}

/**************************************************************************//*!
* @brief    Disable PWM output and set 50% duty cycle
*
* @return   tBool
*
*******************************************************************************/
tBool ACTUATE_DisableOutput(void)
{
	SWLIBS_3Syst_FLT fltpwm;

	tBool statePWM;

	/* Apply 0.5 duty cycle */
	fltpwm.fltArg1 = 0.5F;
	fltpwm.fltArg2 = 0.5F;
	fltpwm.fltArg2 = 0.5F;
	
	statePWM = ACTUATE_SetDutycycle(&fltpwm);

    /* Disable PWM */
	fs_etpu_pwmm_disable(&pwmm_instance);

	return(statePWM);
}

/**************************************************************************//*!
* @brief    Set PWM duty cycle
*
* @details  Call this routine before eTPU PWMM update time to apply duty cycles
*           for next period. In case of later update the duty cycles will be
*           updated in next period and applied for second next period.
* @param    Pointer to a structure holding duty cycles for 3 phase system in
*           float format.
* @return   tBool
*
*******************************************************************************/
tBool ACTUATE_SetDutycycle(SWLIBS_3Syst_FLT *fltpwm)
{
	tBool statePwm = true;

	 pwmm_inputs.input_a = SFRACT24((fltpwm->fltArg1*2.0F)-1.0F);
	 pwmm_inputs.input_b = SFRACT24((fltpwm->fltArg2*2.0F)-1.0F);
	 pwmm_inputs.input_c = SFRACT24((fltpwm->fltArg3*2.0F)-1.0F);

	 /* Store the inputs to PWMM function eTPU memory */
	 fs_etpu_pwmm_set_inputs(&pwmm_instance,&pwmm_inputs);
	 statePwm = false;

	 return(statePwm);
}
