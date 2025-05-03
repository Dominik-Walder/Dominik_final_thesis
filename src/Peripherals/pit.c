/******************************************************************************
*
*   Copyright 2017-2020 NXP
*
****************************************************************************//*!
*
* @file     pit.c
*
* @date     October-20-2020
*
* @brief    PIT runtime and configuration routines
*
*******************************************************************************/

/******************************************************************************
* Included files
*******************************************************************************/
#include "pit.h"
#include <MPC577xC.h>

/******************************************************************************
* Global functions
******************************************************************************/
/***************************************************************************//*!
* @brief   Routine to read PIT counter value scaled to ns
*
* @return  32 bit unsigned integer value corresponding to counted value scaled
*          ns.
*
******************************************************************************/
uint32_t pit_read_values_ns(void)
{
	return ((uint32_t)0xFFFFFFFF - (PIT.TIMER[0].CVAL.R)* SCALE_TO_NS);
}

/***************************************************************************//*!
* @brief   Routine that configures and  starts
*
* @param   32 bit unsigned integer value of ns to be counted
*
* @return  N/A
*
******************************************************************************/
void pit_start_wait_ns(uint32_t time_ns)
{
	PIT.TIMER[0].TCTRL.B.TEN = 0;	/* Disable PIT first */
	PIT.MCR.B.MDIS = 0;
	PIT.TIMER[0].LDVAL.R = ((100 * (time_ns / 1000)) - 1); /* Timer clk is 100 MHz (period 10 ns) */
	PIT.TIMER[0].TCTRL.B.TEN = 1;
}

/***************************************************************************//*!
* @brief   Routine that disables PIT
*
* @param   N/A
*
* @return  N/A
*
******************************************************************************/
void pit_stop(void)
{
	PIT.TIMER[0].TCTRL.B.TEN = 0;
}

/***************************************************************************//*!
* @brief   Routine that performs Timer Interrupt Flag reading
*
* @param   N/A
*
* @return  1 if the flag was set, otherwise returns 0.
*
******************************************************************************/
uint8_t pit_read_flag(void)
{
	if(PIT.TIMER[0].TFLG.B.TIF)
	{
		PIT.TIMER[0].TFLG.B.TIF = 1; /* Clear the timer flag */
		return 1;
	}
	return 0;
}
