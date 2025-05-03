/*******************************************************************************
*
* Copyright 2006-2015 Freescale Semiconductor, Inc.
* Copyright 2016-2020 NXP
*
****************************************************************************//*!
*
* @file     meas_mpc57xx.c
*
* @date     October-20-2020
*
* @brief    Header file for measurement module
*
*******************************************************************************/

/******************************************************************************
* Included files
*******************************************************************************/
#include "meas_mpc57xx.h"

/******************************************************************************
* Global variables
******************************************************************************/
extern float dcbv_float;
extern float ia_float;
extern float ib_float;
extern float ic_float;

/******************************************************************************
* Global functions
******************************************************************************/
/**************************************************************************//*!
* @brief    Measurement module software initialization
*
* @details  Clear variables needed for both calibration as well as run time
*			measurement.
*
* @param    Pointer to module structure containing measurement related variables.
*
* @return   tBool
*
* @note		It is not intended to be executed when application is in run mode.
*
*******************************************************************************/
tBool MEAS_Clear(measModule_t *ptr)
{
    ptr->measured.fltPhA.filt   = 0.0F;
    ptr->measured.fltPhA.raw    = 0.0F;
    ptr->measured.fltPhB.filt   = 0.0F;
    ptr->measured.fltPhB.raw    = 0.0F;
    ptr->measured.fltPhC.filt   = 0.0F;
    ptr->measured.fltPhC.raw    = 0.0F;
    ptr->measured.fltUdcb.filt  = 0.0F;
    ptr->measured.fltUdcb.raw   = 0.0F;
    ptr->measured.fltTemp.filt  = 0.0F;
    ptr->measured.fltTemp.raw   = 0.0F;

    ptr->offset.fltPhA.fltOffset = MLIB_Div(I_MAX, 2);
	ptr->offset.fltPhB.fltOffset = MLIB_Div(I_MAX, 2);
	ptr->offset.fltPhC.fltOffset = MLIB_Div(I_MAX, 2);
    
    ptr->flag.R             	= 0;

    ptr->param.u16CalibSamples  = 0;
    ptr->flag.B.calibInitDone   = 0;
    ptr->flag.B.calibDone       = 0;

    return 1;
}

/**************************************************************************//*!
* @brief    3Phase current measurement software calibration routine.
*
* @details 	This function performs offset calibration for 3 phase current
*           measurement during the calibration phase of the application. It is
*           not intended to be executed when application is in run mode.
*
* @param    Pointer to module structure containing measurement related variables.
* @param    svmSector. Space Vector Modulation Sector.
*
* @return   tBool
*
*******************************************************************************/
tBool MEAS_CalibCurrentSense(measModule_t *ptr, tU16 svmSector)
{
    if (!(ptr->flag.B.calibInitDone))
    {
    	/* +4 in order to accommodate settling time of the filter */
        ptr->calibCntr = 1<< (ptr->param.u16CalibSamples + 4);

        ptr->measured.fltPhA.filt   = 0x0;
        ptr->measured.fltPhB.filt   = 0x0;
        ptr->measured.fltPhC.filt   = 0x0;

        ptr->offset.fltPhA.filtParam.fltAcc		= MLIB_Div(I_MAX, 2);
        ptr->offset.fltPhB.filtParam.fltAcc		= MLIB_Div(I_MAX, 2);
        ptr->offset.fltPhC.filtParam.fltAcc		= MLIB_Div(I_MAX, 2);

        ptr->flag.B.calibDone       = 0;
        ptr->flag.B.calibInitDone   = 1;
    }

    if (!(ptr->flag.B.calibDone))
    {
    	/* Phase A - DC offset data filtering using MA recursive filter */
    	ptr->offset.fltPhA.fltOffset  = GDFLIB_FilterMA(ptr->measured.fltPhA.raw, &ptr->offset.fltPhA.filtParam);

    	/* Phase B - DC offset data filtering using MA recursive filter */
    	ptr->offset.fltPhB.fltOffset  = GDFLIB_FilterMA(ptr->measured.fltPhB.raw, &ptr->offset.fltPhB.filtParam);

    	/* Phase C - DC offset data filtering using MA recursive filter */
    	ptr->offset.fltPhC.fltOffset  = GDFLIB_FilterMA(ptr->measured.fltPhC.raw, &ptr->offset.fltPhC.filtParam);

        if ((--ptr->calibCntr)<=0)
        {
        	ptr->flag.B.calibDone       = 1;    /* End of DC offset calibration */
        }
    }
    return (ptr->flag.B.calibDone);
}

/**************************************************************************//*!
* @brief    3-phase current measurement reading.
*
* @details  This function performs measurement of three phase currents from
*           two shunt resistors. Because a non-zero length PWM pulse width is
*           required for successful current sample, this approach can not be
*           utilized up to full PWM dutycycle.
*
* @param    Pointer to module structure containing measurement related variables.
* @param    *i. Pointer to a structure where to store measured feedback currents
*           of all the 3 phases in a float format.
*
* @return   tBool
*
*******************************************************************************/
tBool MEAS_Get3PhCurrent(measModule_t *ptr, SWLIBS_3Syst_FLT *i)
{
 // ToDo add current filtration
	i->fltArg1 = ia_float;
	i->fltArg2 = ib_float;
	i->fltArg3 = ic_float;

	return(1);
}

/**************************************************************************//*!
* @brief    DCB Voltage measurement routine.
*
* @param    Pointer to module structure containing measurement related variables.
* @param    *uDcbFilter. Pointer to a structure where parameters of the DC bus
*           filter resides and measured DC bus value is stored.
*
* @return   tBool
*
*******************************************************************************/
tBool MEAS_GetUdcVoltage(measModule_t *ptr, GDFLIB_FILTER_MA_T *uDcbFilter)
{

	ptr->measured.fltUdcb.raw =  dcbv_float;
	ptr->measured.fltUdcb.filt  = GDFLIB_FilterMA(ptr->measured.fltUdcb.raw, uDcbFilter);

	return(1);
}

/* End of file */
