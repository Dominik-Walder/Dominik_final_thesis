/*******************************************************************************
*
* Copyright 2006-2015 Freescale Semiconductor, Inc.
* Copyright 2016-2020 NXP
*
****************************************************************************//*!
*
* @file     pospe_sensor.h
*
* @date     October-20-2020
*
* @brief    Header file for position sensor processing
*
*******************************************************************************/
#ifndef POSPE_SENSOR_H_
#define POSPE_SENSOR_H_

/******************************************************************************
* Included files
*******************************************************************************/
#include "gflib.h"
#include "amclib.h"
#include "Config\PMSM_appconfig.h"

/******************************************************************************
* Defines and Macros
*******************************************************************************/

/******************************************************************************
* Typedefs and structures
******************************************************************************/
typedef struct
{
    tFloat    raw;   /*! raw value */
    tFloat    filt;  /*! filtered value */
}pospeValue_t;

typedef struct
{
	tFloat								thRotMec;
	pospeValue_t						wRotMec;
	pospeValue_t						thRotEl;
	pospeValue_t						thRotElk1;
	pospeValue_t						wRotEl;
	tFloat                              thRoErr;
	pospeValue_t						thRotMecSin;
	pospeValue_t						thRotMecCos;
	pospeValue_t						thRotElSin;
	pospeValue_t						thRotElCos;
	AMCLIB_TRACK_OBSRV_T_FLT 			TrackObsrv;
	tFrac32								s32MotorPpScale;
	tFrac16								s16MotorPpScaleShift;
	tFloat								fltMotorPP;
	tFrac16								ftmCntValue;
	tFrac16								ftmModValue;
	tFrac32								offset;
}resolverPospe_t;

/******************************************************************************
* Global Variables Access
******************************************************************************/
extern tBool POSPE_GetPospeElRes(resolverPospe_t *ptr);
extern tBool POSPE_ClearPospeElRes(resolverPospe_t *ptr);

#endif /* POSPE_SENSOR_H_ */
