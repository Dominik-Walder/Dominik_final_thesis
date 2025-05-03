/*******************************************************************************
*
* Copyright 2006-2015 Freescale Semiconductor, Inc.
* Copyright 2016-2020 NXP
*
****************************************************************************//*!
*
* @file     actuate_mpc57xx.h
*
* @date     October-20-2020
*
* @brief    Header file for actuator module
*
*******************************************************************************/
#ifndef _ACTUATE_MPC57XX_H_
#define _ACTUATE_MPC57XX_H_

/******************************************************************************
 * Included files
*******************************************************************************/
#include "gflib.h"
#include "gmclib.h"
#include "gdflib.h"

/******************************************************************************
 * Defines and Macros
*******************************************************************************/

/******************************************************************************
* Global Variables Access
******************************************************************************/
extern tBool 	ACTUATE_EnableOutput(void);
extern tBool 	ACTUATE_DisableOutput(void);
extern tBool 	ACTUATE_SetDutycycle(SWLIBS_3Syst_FLT *fltpwm);

#endif /* _ACTUATE_MPC57XX_H_ */
