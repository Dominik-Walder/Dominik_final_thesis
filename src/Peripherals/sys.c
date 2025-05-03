/******************************************************************************
*
*   Copyright 2013-2016 Freescale Semiconductor, Inc.
*   Copyright 2017-2020 NXP
*
****************************************************************************//*!
*
* @file     sys.c
*
* @date     October-20-2020
*
* @brief    System clock domain, mode entry, interrupt SW mode configuration.
*
*******************************************************************************/

/******************************************************************************
* Included files
*******************************************************************************/
#include "sys.h"

/******************************************************************************
* Global functions
******************************************************************************/
/***************************************************************************//*!
* @brief    System clock and operation mode initialisation
*
* @details  The function configures clock generation module (CGM), mode
*           entry (ME), reset generation module (RGM).
*
* @return   N/A
*
******************************************************************************/
void mcu_mode_clock_init(void)
{   
    /* 0 The crystal oscillator (XOSC) is the clock source for PLL0    */
    /* 1 The internal RC oscillator (IRC) is the clock source for PLL0 */
    SIU.SYSDIV.B.PLL0SEL        = 0;

    /* Configure PLL0 Dividers from 40Mhx XOSC */
    /* phi   = 200 MHz                         */
    /* phi1  = 50 MHz                          */
    PLLDIG.PLL0DV.B.RFDPHI1     = 4;
    PLLDIG.PLL0DV.B.RFDPHI      = 2;
    PLLDIG.PLL0DV.B.PREDIV      = 1;
    PLLDIG.PLL0DV.B.MFD         = 10;

    /* Check whether crystal is 40MHz, if not forever loop here TODO:implement error reporting */
    while (!(SIU.RSR.B.XOSCHIGH)){};

    /* Ensure SIU_RSR[XOSC] is 1 to confirm that XOSC is stable. */
    while (!(SIU.RSR.B.XOSC)){};

    PLLDIG.PLL0CR.B.CLKCFG      = 3;    /* Enable PLL0 by putting PLL into Normal mode   */
    while (!PLLDIG.PLL0SR.B.LOCK){};    /* Test PLL0 lock                                */

    /* Select the source clock for PLL1                             */
    /* 0 The crystal oscillator (XOSC) is the clock source for PLL1 */
    /* 1 The PHI1 output of PLL0 is the clock source for PLL1       */
    SIU.SYSDIV.B.PLL1SEL        = 0;

    /* Configure PLL1 Dividers - 260MHz from 40Mhx XOSC */
    PLLDIG.PLL1DV.B.RFDPHI      = 2;
    PLLDIG.PLL1DV.B.MFD         = 26;

    PLLDIG.PLL1CR.B.CLKCFG      = 3;    /* Enable PLL1 by putting PLL into Normal mode   */
    while (!PLLDIG.PLL1SR.B.LOCK){};    /* Test PLL1 lock                                */

    /* System Clock Divider:
     * 000 Divide by 2.
     * 001 Divide by 4.
     * 010 Divide by 8.
     * 011 Divide by 16.
     * 100 Divide by 1.
     * 101 Divide by 1.
     * 110 Divide by 1.
     * 111 Divide by 1
     */
    SIU.SYSDIV.B.SYSCLKDIV      = 4;

    /* System Clock Select.
     * 0 = The system clock is driven by the 16 MHz Internal RC Oscillator (IRC).
     * 1 = The system clock is driven by the external oscillator (XOSC).
     * 2 = The system clock is driven by the FM PLL (PLL1).
     * 3 = The system clock is driven by the non-FM PLL (PLL0).
     */
    SIU.SYSDIV.B.SYSCLKSEL      = 2;

    /* Peripheral Clock Select.
     * 0 = Clock is connected to the FM clock domain (output of the SYSCLKDIV divider), for legacy support.
     * 1 = Clock is connected to the non-FM clock domain (output of PLL0).
     */
    SIU.SYSDIV.B.PERCLKSEL      = 1;                /* = per_clk_src */

    /* Non-FM Clock Peripheral Divider.
     * 0 = Divide by 2.
     * 1 = Divide by 4.
     * 2 = Divide by 8.
     * 3 = Reserved.
     */
    SIU.SYSDIV.B.PERDIV         = 0;                /* per_clk = per_clk_src / PERDIV    max = 100[MHz]  */

    /* Sigma Delta Clock Divider.
     * DivisionFactor = SDDIV + 1
     */
    SIU.SDCLKCFG.B.SDDIV        = 12;               /* sdadc_clk = per_clk_src / SDDIV   4 - 16 [MHz]    */
                                                    /*
                                                     * PLL0         = 200 MHz
                                                     * sdadc_clk    = PLL0/13 = 200/13 = 15,38 MHz => SDDIV = 12
                                                     */

    /* eTPU Clock Divider.
     * eTPU Clock Divider. Selects the divider value for the eTPU clock.
     *
     * 0 Divide by 2.
     * 1 Divide by 1.
     */
    SIU.SYSDIV.B.ETPUDIV        = 1;                /* eTPU_clk = per_clk_src / ETPUDIV max = 200[MHz]    */
                                                    /*
                                                     * PLL0         = 200 MHz
                                                     * eTPU_clk    = PLL0/1 = 200/1 = 200 MHz => ETPUDIV = 1
                                                     */
													 
}

/***************************************************************************//*!
* @brief   Set the interrupt source and priority.
*
* @return  N/A
*
******************************************************************************/

void my_interrupt_init(uint32_t source, uint8_t priority)
{
	INTC.PSR[source].R = 0x4000 | (priority & 0xFF) ;
}

