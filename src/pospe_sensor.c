/*******************************************************************************
*
* Copyright 2006-2015 Freescale Semiconductor, Inc.
* Copyright 2016-2020 NXP
*
****************************************************************************//*!
*
* @file     pospe_sensor.c
*
* @date     October-20-2020
*
* @brief    Header file for position sensor processing
*
*******************************************************************************/

/******************************************************************************
* Included files
*******************************************************************************/
#include "pospe_sensor.h"
#include "amclib.h"

/******************************************************************************
* Global variables
******************************************************************************/
extern int32_t angle;

tFrac32 angle_with_offset = 0.0F;

/******************************************************************************
* Global functions
******************************************************************************/
/**************************************************************************//*!
* @brief    Read resolver feedback samples to get rotor position and speed
*
* @param    Pointer to a structure holding parameters related to resolver
*           sensor, position and speed.
* @return   tBool
*
*******************************************************************************/
tBool POSPE_GetPospeElRes(resolverPospe_t *ptr)
{
	tBool statusPass;
	statusPass = TRUE;
	tBool vrap1_status = FALSE;
	tBool vrap2_status = FALSE;;

	// tFloat angle_with_offset;
	static tFrac32 	f32ThRotMe, f32ThRotEl;

	AMCLIB_TrackObsrv(ptr->thRoErr, &(ptr->thRotMec), &(ptr->wRotMec.raw), &(ptr->TrackObsrv));

	/* Mechanical and electrical angular speed calculation - float */
	ptr->wRotEl.raw = MLIB_Mul(ptr->wRotMec.raw, MOTOR_PP);

    /* Mechanical rotor position - transformation in to the range <-1,1> - fix point */
    f32ThRotMe 				 	= MLIB_ConvertPU_F32FLT(MLIB_Div(ptr->thRotMec,FLOAT_PI));

    /* Rotor position - transformation from mechanical to electrical  - fix point */
      f32ThRotEl				 	= MLIB_ShL_F32(MLIB_MulSat_F32(f32ThRotMe, MOTOR_PP_GAIN), MOTOR_PP_SHIFT);

    /* Electrical rotor position - <-pi, pi> range - floating point */
    ptr->thRotEl.filt    	= MLIB_Mul(MLIB_ConvertPU_FLTF32(f32ThRotEl), FLOAT_PI);
    angle_with_offset = MLIB_Sub_F32(angle, ptr->offset);
    ptr->thRoErr = MLIB_Mul(MLIB_ConvertPU_FLTF32(MLIB_Sub_F32(angle_with_offset, f32ThRotMe)), FLOAT_PI);

    return(statusPass);
}

/**************************************************************************//*!
* @brief    Clear internal variables
*
* @param    Pointer to a structure holding parameters related to resolver
*           sensor, position and speed.
* @return   tBool
*
*******************************************************************************/
tBool POSPE_ClearPospeElRes(resolverPospe_t *ptr)
{
	tBool statusPass;
    statusPass = TRUE;

    /* Resolver's mechanical position and speed init */
    ptr->thRotMec									= 0.0F;
    ptr->thRoErr									= 0.0F;
    ptr->wRotMec.raw								= 0.0F;
    ptr->wRotMec.filt								= 0.0F;

    /* resolver's electrical position and speed init */
    ptr->thRotEl.raw								= 0.0F;
    ptr->thRotEl.filt								= 0.0F;
    ptr->wRotEl.raw									= 0.0F;
    ptr->wRotEl.filt								= 0.0F;

    AMCLIB_TrackObsrvInit(&(ptr->TrackObsrv));

    return(statusPass);
}
