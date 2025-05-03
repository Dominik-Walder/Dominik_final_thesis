/******************************************************************************
*
*   Copyright 2015-2016 Freescale Semiconductor, Inc.
*   Copyright 2017-2020 NXP
*
****************************************************************************//*!
*
* @file     siu_init.c
*
* @date     October-20-2020
*
* @brief    SIU configuration file
*
*******************************************************************************/

/******************************************************************************
* Included files
*******************************************************************************/
#include <MPC577xC.h>
#include "siu_init.h"

/******************************************************************************
* Global functions
******************************************************************************/
/**************************************************************************//*!
* @brief    Routine that performs SIU configuration for the project
*
* @return   N/A
*
*******************************************************************************/
void SIU_init(void)
{
    /* MC33937 pins */
    SIU.PCR[126].B.PA   = 0;   /* GPIO[126] used as ENABLE */
    SIU.PCR[126].B.OBE  = 1;   /* Output buffer enable */
    SIU.PCR[127].B.PA   = 0;   /* GPIO[127] used as RESET */
    SIU.PCR[127].B.OBE  = 1;   /* Output buffer enable */
    SIU.PCR[138].B.PA   = 2;   /* GPIO[138] used as external IRQ */
    SIU.EIISR.B.ESEL12  = 0;   /* Select IRQ12 for external IRQ input */
    SIU.IMUX7.B.MUXSEL8 = 0;   /* Select input from "ETPUA24_IRQ12_RCH2_A_GPIO138" pad (PCR: 138). */
    SIU.IREER.B.IREE12  = 1;   /* Rising edge event is enabled. */
    SIU.DIRER.B.EIRE12  = 1;   /* Interrupt request enable */

    /* Disable Engclk output */
    SIU.PCR[214].B.OBE  = 0;

    /* SIU configuration for eSCI A */
    SIU.PCR[89].B.PA    = 1;   /* Pin assigned to ESCI A Tx */
    SIU.PCR[89].B.OBE   = 1;   /* Output buffer enable */
    SIU.PCR[90].B.PA    = 1;   /* Pin asigned to ESCI A Rx */
    SIU.PCR[90].B.IBE   = 1;   /* Input buffer enable */

    /* SIU configuration for Resolver - SDADC */
    /* SDADC A +: AN[0] - A4
               -: AN[1] - B5 */
    /* SDADC D +: AN[0] - C18
               -: AN[1] - D19 */

    /* SIU configuration for phase current measurement - EQADC */
    /* PHA      : ANA24/ANB24
     * PHB      : ANA25/ANB25
     * PHC      : ANA26/ANB26
     * DCBV     : ANA28/ANB28  */

    /* DSPI B for MPC5775E-EVB*/
	SIU.PCR[104].B.PA   = 1;   /* SOUT DSPI B Serial Data Output */
	SIU.PCR[104].B.OBE  = 1;   /* Output buffer enable */
	SIU.PCR[103].B.PA   = 1;   /* SIN DSPI B Serial Data Input */
	SIU.PCR[103].B.IBE  = 1;   /* Input buffer enable */
	SIU.PCR[102].B.PA   = 1;   /* SIN DSPI B Clock Output */
	SIU.PCR[102].B.OBE  = 1;   /* Output buffer enable */
	SIU.PCR[108].B.PA   = 1;   /* CSB3 DSPI B Chip select 3 Output */
	SIU.PCR[108].B.OBE  = 1;   /* Output buffer enable */

	/* Configure GPIO functionality for HS and LS toggle on PWM pins */
    SIU.PCR[130].B.PA   = 0;   /* GPIO[130] - PWM PHA base */
    SIU.PCR[130].B.OBE  = 1;   /* Output buffer enable */
    SIU.PCR[131].B.PA   = 0;   /* GPIO[131] - PWM PHA complementary */
    SIU.PCR[131].B.OBE  = 1;   /* Output buffer enable */
    SIU.PCR[132].B.PA   = 0;   /* GPIO[132]  - PWM PHB base */
    SIU.PCR[132].B.OBE  = 1;   /* Output buffer enable */
    SIU.PCR[133].B.PA   = 0;   /* GPIO[133]  - PWM PHB complementary */
    SIU.PCR[133].B.OBE  = 1;   /* Output buffer enable */
    SIU.PCR[134].B.PA   = 0;   /* GPIO[134]  - PWM PHC base */
    SIU.PCR[134].B.OBE  = 1;   /* Output buffer enable */
    SIU.PCR[135].B.PA   = 0;   /* GPIO[135]  - PWM PHC complementary */
    SIU.PCR[135].B.OBE  = 1;   /* Output buffer enable */

	/* High side initial setup */
	SIU.GPDO[130].R     = 0;
	SIU.GPDO[132].R     = 0;
	SIU.GPDO[134].R     = 0;
	/* Low side initial setup */
	SIU.GPDO[131].R     = 1;
	SIU.GPDO[133].R     = 1;
	SIU.GPDO[135].R     = 1;

    /* eTPU channels */
    SIU.PCR[120].B.PA   = 1;   /* eTPUA6 - Resolver excitation signal */
    SIU.PCR[120].B.OBE  = 1;   /* Output buffer enable */
    SIU.PCR[114].B.PA   = 1;   /* eTPUA0 - PWM Master channel - */
    SIU.PCR[114].B.OBE  = 1;   /* Output buffer enable */
    SIU.PCR[141].B.PA   = 1;   /* eTPUA27 - Resolver Sample channel */
    SIU.PCR[141].B.IBE  = 1;   /* Input buffer enable */
    SIU.ISEL8.B.ETPU27  = 1;   /* input mux for eTPU channels A24-29 has to be configured */

    SIU.PCR[142].B.PA   = 1;   /* eTPUA28 - AS channel */
    SIU.PCR[142].B.OBE  = 1;   /* Output buffer enable */
    SIU.PCR[154].B.PA   = 1;   /* eTPUB7 - channel to show Resolver exc. activity - for debug purpose only */
    SIU.PCR[154].B.OBE  = 1;   /* Output buffer enable */
    SIU.PCR[140].B.PA   = 1;   /* eTPUA26 - channel to show Resolver sample activity - for debug purpose only */
    SIU.PCR[140].B.OBE  = 1;   /* Output buffer enable */
    SIU.PCR[143].B.PA   = 1;   /* eTPUA29 - AS channel activity - for debug purpose only */
    SIU.PCR[143].B.OBE  = 1;   /* Output buffer enable */
    SIU.PCR[144].B.PA   = 1;   /* eTPUA30 - Resolver sample channel activity - for debug purpose only */
    SIU.PCR[144].B.OBE  = 1;   /* Output buffer enable */
    /* Brake resistor */
    SIU.PCR[129].B.PA   = 0;   /* GPIO[129] - Brake resistor input */
    SIU.PCR[129].B.OBE  = 1;   /* Output buffer enable */

    /* APP switch pin */
    SIU.PCR[137].B.PA   = 0;   /* GPIO[137] - App switch */
    SIU.PCR[137].B.IBE  = 1;   /* Input buffer enable */

    /* UP/DOWN button */
    SIU.PCR[453].B.PA   = 0;   /* GPIO[453] - BTN UP */
    SIU.PCR[453].B.IBE  = 1;   /* Input buffer enable */
    SIU.PCR[454].B.PA   = 0;   /* GPIO[454] - BTN DOWN */
    SIU.PCR[454].B.IBE  = 1;   /* Input buffer enable */

    /* Debug pin */
    SIU.PCR[157].B.PA   = 0;   /* GPIO[157] - debug pin 0 */
    SIU.PCR[157].B.OBE  = 1;   /* Output buffer enable */
    SIU.PCR[178].B.PA   = 0;   /* GPIO[178] - debug pin 1 */
    SIU.PCR[178].B.OBE  = 1;   /* Output buffer enable */
}

/**************************************************************************//*!
* @brief    Routine that configures intended PWM phase pins for eTPU
*           functionality
*
* @note     This routine is called after MC33937 initialization to configure
*           eTPU functionality for PWM pins
*
*******************************************************************************/
void SIU_PWM_pin_config(void)
{
	/* Configure pins for eTPU functionality */
    SIU.PCR[130].B.PA   = 1;   /* eTPUA16 - PWM PHA base */
    SIU.PCR[130].B.OBE  = 1;   /* Output buffer enable */
    SIU.PCR[131].B.PA   = 1;   /* eTPUA17 - PWM PHA complementary */
    SIU.PCR[131].B.OBE  = 1;   /* Output buffer enable */
    SIU.PCR[132].B.PA   = 1;   /* eTPUA18 - PWM PHB base */
    SIU.PCR[132].B.OBE  = 1;   /* Output buffer enable */
    SIU.PCR[133].B.PA   = 1;   /* eTPUA19 - PWM PHB complementary */
    SIU.PCR[133].B.OBE  = 1;   /* Output buffer enable */
    SIU.PCR[134].B.PA   = 1;   /* eTPUA20 - PWM PHC base */
    SIU.PCR[134].B.OBE  = 1;   /* Output buffer enable */
    SIU.PCR[135].B.PA   = 1;   /* eTPUA21 - PWM PHC complementary */
    SIU.PCR[135].B.OBE  = 1;   /* Output buffer enable */
}


