/******************************************************************************
*
*   Copyright 2015-2016 Freescale Semiconductor, Inc.
*   Copyright 2017-2020 NXP
*
****************************************************************************//*!
*
* @file     esci.h
*
* @date     October-20-2020
*
* @brief    Header file for eSCI  configuration
*
*******************************************************************************/
#ifndef _ESCI_H_
#define _ESCI_H_

/******************************************************************************
* Included files
*******************************************************************************/
#include "typedefs.h"
/******************************************************************************
* Defines and Macros
*******************************************************************************/
/* eSCI Module */
#define ESCI_A_MODULE 0
#define ESCI_B_MODULE 1
#define ESCI_C_MODULE 2
#define ESCI_D_MODULE 3
#define ESCI_E_MODULE 4
#define ESCI_F_MODULE 5

/* eSCI Mode */
#define MODE_SCI	0
#define MODE_LIN	1
/******************************************************************************
* Global Variables Access
******************************************************************************/
void esci_init(uint8_t module, uint8_t mode);

#endif /* _ESCI_H_ */
