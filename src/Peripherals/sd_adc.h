/******************************************************************************
*
*   Copyright 2015-2016 Freescale Semiconductor, Inc.
*   Copyright 2017-2020 NXP
*
****************************************************************************//*!
*
* @file     sd_adc.h
*
* @date     October-20-2020
*
* @brief    Header file for SDADC configuration
*
*******************************************************************************/
#ifndef _SD_ADC_H_
#define _SD_ADC_H_

/******************************************************************************
* Included files
*******************************************************************************/
#include "MPC577xC.h"

/******************************************************************************
* Defines and Macros
*******************************************************************************/
/* MCR defines */
#define OSR_24		0
#define OSR_28		1
#define OSR_32		2
#define OSR_36		3
#define OSR_40		4
#define OSR_44		5
#define OSR_48		6
#define OSR_56		7
#define OSR_64		8
#define OSR_72		9
#define OSR_75		10
#define OSR_80		11
#define OSR_88		12
#define OSR_96		13
#define OSR_112		14
#define OSR_128		15
#define OSR_144		16
#define OSR_160		17
#define OSR_176		18
#define OSR_192		19
#define OSR_224		20
#define OSR_256		21

#define GAIN_1		0
#define GAIN_2		1
#define GAIN_4		2
#define GAIN_8		3

#define FTHLD_0		0
#define FTHLD_1		1
#define FTHLD_2		2
#define FTHLD_3		3
#define FTHLD_4		4
#define FTHLD_5		5
#define FTHLD_6		6
#define FTHLD_7		7
#define FTHLD_8		8
#define FTHLD_9		9
#define FTHLD_10	10
#define FTHLD_11	11
#define FTHLD_12	12
#define FTHLD_13	13
#define FTHLD_14	14
#define FTHLD_15	15

#define DIFF		0
#define SINGLE		1

/******************************************************************************
* Global Variables Access
******************************************************************************/
void sdadc_init(uint8_t pdr, uint8_t gain, uint8_t fifo_thld, uint8_t inp_mode);
void sdadc_start(void);
void SDADC2_init(void);
SDADC2_voltage(void);
float SDADC2_torque(void);


#endif /* _SD_ADC_H_ */
