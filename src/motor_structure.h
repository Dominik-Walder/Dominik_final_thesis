/*******************************************************************************
*
* Copyright 2006-2015 Freescale Semiconductor, Inc.
* Copyright 2016-2020 NXP
*
****************************************************************************//*!
*
* @file     motor_structure.h
*
* @date     October-20-2020
*
* @brief    Header file for FOC Drive
*
*******************************************************************************/
#ifndef _MOTOR_STRUCTURE_H
#define _MOTOR_STRUCTURE_H

/******************************************************************************
* Includes
******************************************************************************/
#include "gflib.h"
#include "gmclib.h"
#include "gdflib.h"
#include "state_machine.h"
#include "amclib.h"
#include "MC33937/MC33937.h"

/******************************************************************************
* Defines and Macros
*******************************************************************************/
#define SPEED_RPM_INC			2
#define SPEED_RPM_DEC			2
#define	SPEED_UP_CNT			1000
#define	SPEED_DOWN_CNT			1000
#define	APP_OFF_CNT				5000
#define SENSORLESS_CNT			10000
#define MAX_TH_DIF_OPEN_ESTIM	(0.01F)

#define SPEED_RAD_INC			(float)((SPEED_RPM_INC/N_MAX)*WEL_MAX)
#define SPEED_RAD_DEC			(float)((SPEED_RPM_DEC/N_MAX)*WEL_MAX)

/******************************************************************************
* Typedefs and structures
******************************************************************************/

/**************************************************************************//*!
* @brief    Structure containing position/speed module variables
*******************************************************************************/
typedef struct
{
    tFloat               			thRotEl;		/* El. position entering to the control loop */
    tFloat                			wRotEl;			/* El. speed entering to the control loop */
    tFloat                  		wRotElFilt;		/* Filtered El. speed entering to the control loop */
    tFloat                 		 	wRotElReq;		/* Required el. speed */
    tFloat                  		wRotElErr;		/* Error of the el. speed entering to speed controller */
    tS16							speedLoopCntr;	/* Rate between speed and current loop */
} pospeControl_t;

/**************************************************************************//*!
* @brief    Structure containing variables for open loop control
*******************************************************************************/
typedef struct
{
	tFloat							thRotEl;
	tFloat							wRotEl;
	GFLIB_INTEGRATOR_TR_T_F32 		integ;
	tFloat							iQUpperLimit;
	tFloat							iQLowerLimit;
	tFloat							thDifOpenLEstim;
}openLoopPospe_t;

/**************************************************************************//*!
* @brief    Structure containing variables for scalar control
*******************************************************************************/
typedef struct
{
	tFloat							UmReq;				/* Required magnitude of supply voltage for scalar control */
	tFloat							VHzRatioReq;		/* V/f ratio - gain */
	tS16							VHzRatioReq_Shift;	/* V/f ratio - shift */
}scalarControl_t;

/**************************************************************************//*!
* @brief    General structure for PMSM motor
*******************************************************************************/
typedef struct{
	tU16        		            alignCntr;		/* Alignment duration */
	tFloat        		            alignVoltage;	/* Alignment voltage */
    tU16		                    svmSector;      /* Space Vector Modulation sector */
    SWLIBS_2Syst_FLT                iDQFbck;        /* dq - axis current feedback */
    SWLIBS_2Syst_FLT                iAlBeFbck;      /* Alpha/Beta - axis current feedback */
    SWLIBS_2Syst_FLT                iDQReqInLoop;   /* dq - axis required currents, FOC Inner Loop (Current Loop) input */
    SWLIBS_2Syst_FLT                iDQReqOutLoop;  /* dq - axis required currents, FOC Outer Loop (Speed Loop & Field Weakening) output */
	SWLIBS_2Syst_FLT                uDQReq;         /* dq - axis required voltages given by current PIs */
    SWLIBS_2Syst_FLT                uAlBeReq;       /* Alpha/Beta required voltages */
    SWLIBS_2Syst_FLT                uAlBeReqDCB;    /* Alpha/Beta required voltages after DC Bus ripple elimination */
    SWLIBS_2Syst_FLT                uAlBeReqDCBLim; /* Alpha/Beta required voltages after DC Bus ripple elimination with limits */
    SWLIBS_2Syst_FLT                thTransform;    /* Transformation angle - enters to Park transformation */
    SWLIBS_3Syst_FLT                iAbcFbck;       /* Three phases current feedback */
    SWLIBS_3Syst_FLT                pwmflt;         /* Three phase 16bit Duty-Cycles estimated from uAlBeReqDCB */
    GMCLIB_ELIMDCBUSRIP_T_FLT		elimDcbRip;     /* Predefined structure related to DC Bus voltage ripple elimination */
    GDFLIB_FILTER_MA_T_FLT			uDcbFilter;     /* DC bus voltage filter settings */
    tFloat							fltUdcb;		/* DC bus voltage */
    openLoopPospe_t					pospeOpenLoop;	/* Open Loop Position generator */
    pospeControl_t                  pospeControl;   /* Position/Speed variables needed for control */
    scalarControl_t					scalarControl;  /* Scalar Control variables for MCAT purpose */
    AMCLIB_CURRENT_LOOP_T_FLT 		CurrentLoop;	/* Current loop function */
    AMCLIB_FW_SPEED_LOOP_T_FLT		FwSpeedLoop;	/* Speed loop plus field weakining function */
    GFLIB_VECTORLIMIT_T_FLT 		AlBeReqDCBLim;	/* Limits for uAlBeReqDCB */
}pmsmDrive_t;

typedef enum
{
	scalarControl	=0,
	voltageControl	=1,
	currentControl	=2,	
	speedControl	=3
}controlStructMode_t;

typedef struct
{
	tU32				ledCounter;
	tU32				ledFlashing;
	tU32				cntSpeedUp;
	tU32				cntSpeedDown;
	tU32				cntAppOff;
	tU8   				btSpeedUp;						/* Variable to increase the speed command by given step */
	tU8  				btSpeedDown;					/* Variable to lower the speed command by given step */
	tU8    				btFlipFlop, btFlipFlopTemp;		/* Enable/Disable Control */
    controlStructMode_t FOCcontrolMode;     			/* Defines the cascade control mode: range(0-SC, 1-UC, 2-IC, 3-SC) */
    tBool           	switchAppOnOff;         		/* Raw value */
    tBool           	switchAppOnOffState;    		/* Raw value */
    tBool           	switchFaultClear;       		/* Raw value */
    tBool           	switchAppReset;         		/* Raw value */
    tBool           	readFault;						/* Read fault status */
}userControl_t;


typedef struct
{
    AppStates       state;                  /* Raw value */
    AppEvents       event;                  /* Raw value */
    userControl_t   usrControl;				/* User action required */
    tBool			loadDefSetting;			/* Load default application settings */
}driveStates_t;

typedef union
{
	tU16 R;
    struct
    {
        tU16 MC33937IntFlag			: 1;
        tU16 MC33937ClearErr        : 1;
        tU16 MC33937Error           : 1;
        tU16 MC33937InitDone		: 1;
        tU16 						: 12;   /* RESERVED */
    }B;
}MC33937Status_t;

typedef struct
{
	uint32_t PDB0_SeqErrFlags;       	/* PDB0 Sequence error flags */
	uint32_t PDB1_SeqErrFlags;       	/* PDB0 Sequence error flags */
	uint32_t PDB0_SeqErrCounter;		/* PDB0 Sequence error counter */
	uint32_t PDB1_SeqErrCounter;		/* PDB1 Sequence error counter */
}pdbStatus_t;

typedef union
{
    tU16 R;
    struct
    {
        tU16 PDB0_Error              : 1;   /* PDB0 Sequence error */
        tU16 PDB1_Error              : 1;   /* PDB1 Sequence error */
        tU16 FTM_Error               : 1;   /* Error in FTM PWM update */
        tU16 						 : 13;	/* RESERVED */
    }B;
}mcuFaultStatus_t;

typedef union
{
    tU16 R;
    struct
    {
        tU16 InitError              : 1;	/* Error during app initialization */
        tU16 CalibError             : 1;	/* Error during calibration */
        tU16 AlignError             : 1;	/* Error during alignment */
        tU16 RunError             	: 1;	/* Error during run state */
        tU16 FOCError				: 1;	/* Error during FOC calculation */
        tU16 : 11;                      	/* RESERVED */

    }B;
}stMachineFaultStatus_t;

typedef union
{
    tU16 R;
    struct
    {
    	tU16 OffCancError           : 1;   /* Offset Cancellation Error flag */
    	tU16 OverPhaseCCurrent      : 1;   /* OverCurrent fault flag */
    	tU16 OverPhaseBCurrent      : 1;   /* OverCurrent fault flag */
    	tU16 OverPhaseACurrent      : 1;   /* OverCurrent fault flag */
    	tU16 OverHeating            : 1;   /* Overheating fault flag */
    	tU16 MainsFault             : 1;   /* Mains out of range */
    	tU16 OverLoad               : 1;   /* Overload Flag */
    	tU16 UnderDCBusVoltage      : 1;   /* Undervoltage fault flag */
    	tU16 OverDCBusVoltage       : 1;   /* Overvoltage fault flag */
    	tU16 : 7;                          /* RESERVED */
    }B;
}motorFaultStatus_t;

typedef struct
{
	tU32 speed_w_e;
	tU32 speed_n_m;
	tU32 speed_ramp;
	tU32 position;
}fm_scale_t;

typedef struct
{
	mcuFaultStatus_t 		mcu;
	tBool 					mc33937;
	stMachineFaultStatus_t	stateMachine;
	motorFaultStatus_t 		motor;
}appFaultStatus_t;    /* Application fault status user type*/


#endif /* _MOTOR_STRUCTURE */



