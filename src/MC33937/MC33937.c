/******************************************************************************
*
*   Copyright 2017-2020 NXP
*
****************************************************************************//*!
*
* @file     MC33937.c
*
* @date     October-20-2020
*
* @brief    Source file containing MC33937 configuration and runtime routines
*
*******************************************************************************/

/******************************************************************************
 * Included files
*******************************************************************************/
#include "MC33937.h"
#include <MPC577xC.h>

/******************************************************************************
* Global functions
******************************************************************************/

/***************************************************************************//*!
* @brief    Configure MC33937 -> initialization, dead-time set, read status
*           registers.
*
* @param    Pointer to the current object.
*
* @return   tBool
*
******************************************************************************/
tBool MC33937_configure( MC33937_T *ptr){
	volatile uint8_t     	cnt;
	uint8_t                 empty_data;
    uint16_t     	        command;
	volatile tBool 		    statusPass;

	statusPass = TRUE;

    statusPass &= ClearRESET();

    statusPass &= ClearEN();
    statusPass &= SetRESET();

	/* start the PIT to count 2 ms */
	pit_start_wait_ns(2000000);
	while(!pit_read_flag());
	pit_stop();

    /* Start DSPI */
	statusPass &= dspi_start();

    /* Clear all faults */
    statusPass &= MC33937_clear_faults(ptr);

    /* Initialize MASK register 0 */
    command     = (uint16_t)ptr->data.mc33937mask0;
    statusPass  &= dspi_master_data_tx(SINGLE_CS,CS3,CTAR0, command, &empty_data);

    /* Initialize MASK register 1 */
    command     = ptr->data.mc33937mask1;
    statusPass  &= dspi_master_data_tx(SINGLE_CS,CS3,CTAR0, command, &empty_data);

    /* Dead-time configuration */
    cnt = 5;
	do
	{
		ptr->status.sw.B.deadtimeSetupError = ~MC33937_set_deadtime(ptr,(uint16_t)(ptr->data.requiredDeadTimeNs));
		cnt--;
	} while (ptr->status.sw.B.deadtimeSetupError && cnt);

	statusPass &= ~(ptr->status.sw.B.deadtimeSetupError);

	/* Clear all faults */
	statusPass &= MC33937_clear_faults(ptr);

	/* Read status registers */
	statusPass &= MC33937_get_status(ptr);

	/* Send MODE command */
	command     = ptr->data.mc33937mode;
	statusPass  &= dspi_master_data_tx(SINGLE_CS,CS3,CTAR0, command, &empty_data);

    /* Start the PIT to count 2 us */
    pit_start_wait_ns(2000);
    while(!pit_read_flag());
    pit_stop();

	statusPass &= SetEN();

	/* Wait 1000 ns after EN are set (min is 280 ns) */
	pit_start_wait_ns(1000);
	while(!pit_read_flag());
	pit_stop();

	ptr->status.sw.B.configurationError = ~statusPass;

    return (statusPass);
}
/***************************************************************************//*!
* @brief    Function for SW initialization of the MC33937 functionality
*
* @details	Initialize the SW configuration according the application
			requirements (dead-time, interrupts ... )
* @param    hw_ptr	Pointer to MC33937 HW configuration structure
* @param    sw_ptr  Pointer to MC33937 SW configuration structure
*
* @return   tBool
*
******************************************************************************/
tBool MC33937_sw_init( MC33937_T *sw_ptr)
{
	MC33937_disable_output(sw_ptr);

    sw_ptr->data.requiredDeadTimeNs            = sw_ptr->configData.deadtime;
    sw_ptr->data.mc33937mask0                  = 0x20 | (uint8_t)(sw_ptr->configData.interruptEnable.R);
    sw_ptr->data.mc33937mask1                  = 0x30 | (uint8_t)(sw_ptr->configData.interruptEnable.R >> 8);
    sw_ptr->data.mc33937mode                   = 0x40 | (uint8_t)sw_ptr->configData.mode.R;

    MC33937_configure(sw_ptr);

    return (TRUE);

}

/***************************************************************************//*!
* @brief    Set/Clear MC33937 ENable pin
*
* @return   tBool
*
******************************************************************************/
tBool SetEN(void)
{
    tBool statusPass = TRUE;

	/* Set ENABLE pins */
	SIU.GPDO[MC33937_ENABLE_PIN].R = 1;

    return(statusPass);
}

tBool ClearEN(void)
{
    tBool statusPass = TRUE;

	/* Clear ENABLE pins */
	SIU.GPDO[MC33937_ENABLE_PIN].R = 0;

    return(statusPass);
}

/***************************************************************************//*!
* @brief    Set/Clear MC33937 Reset pin
*
* @return   tBool
*
******************************************************************************/
tBool SetRESET(void)
{
    tBool statusPass = TRUE;

	SIU.GPDO[MC33937_RESET_PIN].R 	= 1;

    return(statusPass);
}

tBool ClearRESET(void)
{
    tBool statusPass = TRUE;

	SIU.GPDO[MC33937_RESET_PIN].R 	= 0;

    return(statusPass);
}

/***************************************************************************//*!
* @brief    Enable MC33937 output by setting Enable pin
*
* @param    Pointer to structure holding MC33937 parameters
* @return   tBool
*
******************************************************************************/
tBool MC33937_enable_output( MC33937_T *ptr)
{
    tBool statusPass = TRUE;

    /* EN pins go HIGH, RST stays HIGH, device goes to ENABLE MODE */
    statusPass &= SetEN();
    statusPass &= SetRESET(); /* RST pin remains high, it goes only low in sleep mode */

    ptr->status.sw.B.outputEnablingError = ~statusPass;

    return(statusPass);
}

/***************************************************************************//*!
* @brief    Disable MC33937 output by clearing Enable pin
*
* @param    Pointer to structure holding MC33937 parameters
* @return   tBool
*
******************************************************************************/
tBool MC33937_disable_output(MC33937_T *ptr)
{
    tBool statusPass = TRUE;

    /* EN pins go LOW, RST stays HIGH, device goes to STANDBY MODE */
    statusPass &= ClearEN();
    statusPass &= ClearRESET();
    ptr->status.sw.B.outputDisablingError = ~statusPass;

    return(statusPass);
}

/***************************************************************************//*!
* @brief    MC33937 fault clearing by setting CLINT0/1 specific commands
*
* @param    Pointer to structure holding MC33937 parameters
* @return   tBool
*
******************************************************************************/
tBool MC33937_clear_faults(MC33937_T *ptr)
{
    tBool statusPass = TRUE;
	volatile uint8_t     	command;
	uint8_t                 empty_data;

    /* CLINT0_COMMAND = 0x6F */
    /* CLINT1_COMMAND = 0x7F */
    command     = 0x6F;
    statusPass  &= dspi_master_data_tx(SINGLE_CS,CS3,CTAR0, command, &empty_data);
    command     = 0x7F;
    statusPass  &= dspi_master_data_tx(SINGLE_CS,CS3,CTAR0, command, &empty_data);

    ptr->status.sw.B.clearingError = ~statusPass;

    return(statusPass);
}

/***************************************************************************//*!
* @brief    Get general status of MC33937 by reading status registers 0
*
* @param    Pointer to structure holding MC33937 parameters
* @return   tBool
*
******************************************************************************/
tBool MC33937_get_status_SR0(MC33937_T *ptr)
{
    tBool statusPass = TRUE;
	uint16_t     	    command;
	uint8_t             empty_data;

    /* Read status register SR0 from MC33937 */
    command     = 0x0;
	statusPass  &= dspi_master_data_tx(SINGLE_CS,CS3,CTAR0, command, &empty_data);

	command     = 0x0;
	statusPass  &= dspi_master_data_tx(SINGLE_CS,CS3,CTAR0, command, &ptr->status.sr0);

    return(statusPass);
}

/***************************************************************************//*!
* @brief    Get status of MC33937 and store it in status registers
*
* @param    Pointer to structure holding MC33937 parameters
* @return   tBool
*
******************************************************************************/
tBool MC33937_get_status( MC33937_T *ptr)
{
    tBool statusPass = TRUE;
	uint16_t         	command;
	uint8_t             empty_data;
    /*
     * Check whether dead-time is already configured
     * STATUS3_COMMAND  0x3
     */
    /* Read status registers from MC33937 */
    command     = 0x0;
	statusPass  &= dspi_master_data_tx(SINGLE_CS,CS3,CTAR0, command, &empty_data);

	command     = 0x1;
	statusPass  &= dspi_master_data_tx(SINGLE_CS,CS3,CTAR0, command, &ptr->status.sr0);

	command     = 0x2;
	statusPass  &= dspi_master_data_tx(SINGLE_CS,CS3,CTAR0, command, &ptr->status.sr1);

	command     = 0x3;
	statusPass  &= dspi_master_data_tx(SINGLE_CS,CS3,CTAR0, command, &ptr->status.sr2);

	command     = 0x0;
	statusPass  &= dspi_master_data_tx(SINGLE_CS,CS3,CTAR0, command, &ptr->status.sr3);

    return(statusPass);
}
/***************************************************************************//*!
* @brief    Set dead-time following the procedure described in MC33937 datasheet
*
* @param    Pointer to structure holding MC33937 parameters
* @param    Required dead-time to be configured
*
* @return   tBool
*
******************************************************************************/
tBool MC33937_set_deadtime(MC33937_T * ptr, uint16_t deadtimeNs)
{
	uint32_t 	    	actualDeadtime;
	volatile tFrac16 	delta;
	volatile tBool		statusPass;
	uint32_t           	dt;
	uint16_t         	command;
	uint8_t             empty_data;

	statusPass = TRUE;

	/* Send DEADTIME command for dead time configuration with zero calibration */
	command     = 0x81;
	statusPass  &= 	dspi_master_data_tx(SINGLE_CS,CS3,CTAR0, command, &empty_data);

	/* Wait 20us = 20 000 ns*/
	pit_start_wait_ns(20000);
	while(!pit_read_flag());
	pit_stop();

	/* Configure CS pad as GPIO for deadtime configuration */
	SIU.PCR[108].B.PA = 0;

	/* Recalculate dead-time for CS pulse width as required DTx16 */
	dt = ((uint32_t)deadtimeNs)<<4;

	/* Pull-down CS to configure dead time */
	SIU.GPDO[108].R = 0;

	/* Apply delay in order to keep CS down for duration of req. deadtime x 16 */
	pit_start_wait_ns(dt);
	while(!pit_read_flag());
	pit_stop();

	/* Pull-up CS to configure dead time */
	SIU.GPDO[108].R 	= 1;

	/* Configure CS pad back as DSPI chip select */
	SIU.PCR[108].B.PA = 1;
	/*
	 * Check whether dead-time is already configured
	 * STATUS3_COMMAND  0x3
	 */

	/* Read deadtime status register 3 from MC33937 */
	command     = 0x3;
	statusPass  &= dspi_master_data_tx(SINGLE_CS,CS3,CTAR0, command, &empty_data);

	command     = 0x0;
	statusPass  &= dspi_master_data_tx(SINGLE_CS,CS3,CTAR0, command, &actualDeadtime);

	ptr->data.actualDeadTimeNs = (uint16_t)((uint16_t)actualDeadtime * (uint16_t)50);

	delta       = MLIB_Abs_F16((tFrac16)(deadtimeNs) - (tFrac16)(ptr->data.actualDeadTimeNs));
	statusPass  &= (delta > 100)?FALSE:TRUE;

	return(statusPass);
}
