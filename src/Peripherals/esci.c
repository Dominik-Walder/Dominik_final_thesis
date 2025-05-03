/******************************************************************************
*
*   Copyright 2015-2016 Freescale Semiconductor, Inc.
*   Copyright 2017-2020 NXP
*
****************************************************************************//*!
*
* @file     esci.c
*
* @date     October-20-2020
*
* @brief    eSCI  configuration file
*
*******************************************************************************/

/******************************************************************************
* Included files
*******************************************************************************/
#include "esci.h"
#include <MPC577xC.h>

/******************************************************************************
* Global functions
******************************************************************************/
/***************************************************************************//*!
* @brief   Init ESCI to 115kbd @ 200MHz.
*
* @return  N/A
*
******************************************************************************/
void esci_init(uint8_t module, uint8_t mode)
{
	switch(module){
	case ESCI_A_MODULE:
		  eSCI_A.LCR1.B.LIN = mode;          /* Select between LIN or SCI functionality                      */
		  eSCI_A.CR2.R = 0x2000;             /* Enable ESCI and set all bits to reset value                  */
		  eSCI_A.CR1.B.TE = 1;               /* Transmitter  enable                                          */
		  eSCI_A.CR1.B.RE = 1;               /* Receiver enable                                              */
		  eSCI_A.CR1.B.PT = 0;               /* Parity is even                                               */
		  eSCI_A.CR1.B.PE = 0;               /* Parity control disable                                       */
		  eSCI_A.BRR.B.SBR = 54;             /* Baud rate = 115200: MCLK = (200MHz/2), Txclk = MCLK/(16*SBR) */
		break;
	case ESCI_B_MODULE:
		  eSCI_B.LCR1.B.LIN = mode;          /* Select between LIN or SCI functionality                      */
		  eSCI_B.CR2.R = 0x2000;             /* Enable ESCI and set all bits to reset value                  */
		  eSCI_B.CR1.B.TE = 1;               /* Transmitter enable                                           */
		  eSCI_B.CR1.B.RE = 1;               /* Receiver enable                                              */
		  eSCI_B.CR1.B.PT = 0;               /* Parity is even                                               */
		  eSCI_B.CR1.B.PE = 0;               /* Parity control disable                                       */
		  eSCI_B.BRR.B.SBR = 54;             /* Baud rate = 115200: MCLK = (200MHz/2), Txclk = MCLK/(16*SBR) */
		break;
	case ESCI_C_MODULE:
		  eSCI_C.LCR1.B.LIN = mode;          /* Select between LIN or SCI functionality                      */
		  eSCI_C.CR2.R = 0x2000;             /* Enable ESCI and set all bits to reset value                  */
		  eSCI_C.CR1.B.TE = 1;               /* Transmitter  enable                                          */
		  eSCI_C.CR1.B.RE = 1;               /* Receiver enable                                              */
		  eSCI_C.CR1.B.PT = 0;               /* Parity is even                                               */
		  eSCI_C.CR1.B.PE = 0;               /* Parity control disable                                       */
		  eSCI_C.BRR.B.SBR = 54;             /* Baud rate = 115200: MCLK = (200MHz/2), Txclk = MCLK/(16*SBR) */
		break;
	case ESCI_D_MODULE:
		  eSCI_D.LCR1.B.LIN = mode;          /* Select between LIN or SCI functionality                      */
		  eSCI_D.CR2.R = 0x2000;             /* Enable ESCI and set all bits to reset value                  */
		  eSCI_D.CR1.B.TE = 1;               /* Transmitter enable                                           */
		  eSCI_D.CR1.B.RE = 1;               /* Receiver enable                                              */
		  eSCI_D.CR1.B.PT = 0;               /* Parity is even                                               */
		  eSCI_D.CR1.B.PE = 0;               /* Parity control disable                                       */
		  eSCI_D.BRR.B.SBR = 54;             /* Baud rate = 115200: MCLK = (200MHz/2), Txclk = MCLK/(16*SBR) */
		break;
	case ESCI_E_MODULE:
		  eSCI_E.LCR1.B.LIN = mode;          /* Select between LIN or SCI functionality                      */
		  eSCI_E.CR2.R = 0x2000;             /* Enable ESCI and set all bits to reset value                  */
		  eSCI_E.CR1.B.TE = 1;               /* Transmitter enable                                           */
		  eSCI_E.CR1.B.RE = 1;               /* Receiver enable                                              */
		  eSCI_E.CR1.B.PT = 0;               /* Parity is even                                               */
		  eSCI_E.CR1.B.PE = 0;               /* Parity control disable                                       */
		  eSCI_E.BRR.B.SBR = 54;             /* Baud rate = 115200: MCLK = (200MHz/2), Txclk = MCLK/(16*SBR) */
		break;
	case ESCI_F_MODULE:
		  eSCI_F.LCR1.B.LIN = mode;          /* Select between LIN or SCI functionality                      */
		  eSCI_F.CR2.R = 0x2000;             /* Enable ESCI and set all bits to reset value                  */
		  eSCI_F.CR1.B.TE = 1;               /* Transmitter enable                                           */
		  eSCI_F.CR1.B.RE = 1;               /* Receiver enable                                              */
		  eSCI_F.CR1.B.PT = 0;               /* Parity is even                                               */
		  eSCI_F.CR1.B.PE = 0;               /* Parity control disable                                       */
		  eSCI_F.BRR.B.SBR = 54;             /* Baud rate = 115200: MCLK = (200MHz/2), Txclk = MCLK/(16*SBR) */
		break;
	}

}
