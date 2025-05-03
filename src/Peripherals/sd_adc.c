/******************************************************************************
*
*   Copyright 2015-2016 Freescale Semiconductor, Inc.
*   Copyright 2017-2020 NXP
*
****************************************************************************//*!
*
* @file     sd_adc.c
*
* @date     October-20-2020
*
* @brief    SDADC configuration file
*
*******************************************************************************/

/******************************************************************************
* Included files
*******************************************************************************/
#include "sd_adc.h"
#include "MPC577xC.h"

/******************************************************************************
* Global functions
******************************************************************************/
/**************************************************************************//*!
* @brief    Routine that performs configuration of SDADC
*
*
* @details  Two SDADC instances are configured to continually sample resolver
*           sine and cosine feedback signals at sampling rate given by pdr
*           parameter. DMA request when Result FIFO threshold reached is
*           enabled.
* @param    pdr. This parameter configures over sampling rate.
* @param    gain. This parameter configures digital gain of measured results.
* @param    fifo_thld. When this threshold is reached DMA request is generated.
* @param    inp_mode. This parameter configure either differential or single
*           ended sampling mode.
* @return   N/A
*
*******************************************************************************/
void sdadc_init(uint8_t pdr, uint8_t gain, uint8_t fifo_thld, uint8_t inp_mode)
{

	/* SD ADC 1 CONFIGURATION */
	SDADC_1.SFR.R = 0x1B;	         /* Clear all the status flags */
	SDADC_1.FCR.B.FRST = 1;	         /* Generate reset to flush FIFO */
	SDADC_1.FCR.B.FTHLD = fifo_thld; /* FIFO threshold - overcome generates FIFO full event flag */
	SDADC_1.FCR.B.FSIZE = 3;	     /* FIFO size: 0 - 1 word, 1 - 4 words, 2 - 8 words, 3 - 16 words */
	SDADC_1.FCR.B.FOWEN = 0;	     /* Disable FIFO overwrite */
	SDADC_1.FCR.B.FE = 1;	         /* Enable FIFO */
	SDADC_1.RSER.B.DFFDIRS = 1;	     /* DMA request on data FIFO full */
	SDADC_1.RSER.B.DFFDIRE = 1;      /* Data FIFO full request enable */
	SDADC_1.MCR.B.MODE = inp_mode;	 /* Input mode: 0 - differential, 1 - single-ended */
	SDADC_1.MCR.B.PDR = pdr;	     /* Over sampling rate */
	SDADC_1.MCR.B.PGAN = gain;	     /* Gain */
	SDADC_1.MCR.B.HPFEN = 1;	     /* High-pass filter enabled */
	SDADC_1.MCR.B.EN = 1;	         /* Internal SDADC modulator enabled */
	SDADC_1.OSDR.B.OSD = 1;	         /* Set the output settling delay */
	SDADC_1.CSR.B.ANCHSEL = 0;	     /* Analog input channel AN[x] selection */
	SDADC_1.MCR.B.TRIGEN = 1;	     /* Enable HW trigger */
	
	/* SD ADC 4 CONFIGURATION */
	SDADC_4.SFR.R = 0x1B;	         /* Clear all the status flags */
	SDADC_4.FCR.B.FRST = 1;	         /* Generate reset to flush FIFO */
	SDADC_4.FCR.B.FTHLD = fifo_thld; /* FIFO threshold - overcome generates FIFO full event flag */
	SDADC_4.FCR.B.FSIZE = 3;	     /* FIFO size: 0 - 1 word, 1 - 4 words, 2 - 8 words, 3 - 16 words */
	SDADC_4.FCR.B.FOWEN = 0;	     /* Disable FIFO overwrite */
	SDADC_4.FCR.B.FE = 1;	         /* Enable FIFO */
	SDADC_4.RSER.B.DFFDIRS = 1;	     /* DMA request on data FIFO full */
	SDADC_4.RSER.B.DFFDIRE = 1;      /* Data FIFO full request enable */
	SDADC_4.MCR.B.MODE = inp_mode;	 /* Input mode: 0 - differential, 1 - single-ended */
	SDADC_4.MCR.B.PDR = pdr;	     /* Over sampling rate */
	SDADC_4.MCR.B.PGAN = gain;	     /* Gain */
	SDADC_4.MCR.B.HPFEN = 1;	     /* High-pass filter enabled */
	SDADC_4.MCR.B.EN = 1;	         /* Internal SDADC modulator enabled */
	SDADC_4.OSDR.B.OSD = 1;	         /* Set the output settling delay */
	SDADC_4.CSR.B.ANCHSEL = 0;	     /* Analog input channel AN[x] selection */
	SDADC_4.MCR.B.TRIGSEL = 0;       /* SDADC_1 trigger output selected as an input trigger for SDADC4 */
	SDADC_4.MCR.B.TRIGEDSEL = 1;	 /* Rising edge of trigger input selected */
	SDADC_4.MCR.B.TRIGEN = 1;	     /* Enable HW trigger */
}

/**************************************************************************//*!
* @brief    Routine that starts SDADC1 module
*
* @details  SDADC1 is started by SW, SDADC4 is then started by HW output of
*           SDADC1.
*
*******************************************************************************/
void sdadc_start(void)
{
	/* Start the SDADC_1 by SW trigger */
	SDADC_1.STKR.R = 0xFFFF;
}

/**************************************************************************//*!
* @brief    Routine that starts SDADC2 module
*
* @details  Initialization of SDADC2 module for torque measurement on AN5 pin
*
*******************************************************************************/
void SDADC2_init(void){
	SDADC_2.MCR.B.EN = 1;				//SDADC internal modulator enabled
	SDADC_2.MCR.B.MODE = 1;				//Single ended výstup
	SDADC_2.CSR.R = 0x0; 				//Výbìr input ADC pinu (AN0)
	//SDADC_2.CSR.B.ANCHSEL = 0;
	SDADC_2.OSDR.R = 0x10; 				//Nastavení ustálení výsledku
	SDADC_2.RKR.B.RESET_KEY = 0x5AF0;	//reset SDADC block

	/*SDADC_3.MCR.B.EN = 1;				//SDADC internal modulator enabled
	SDADC_3.MCR.B.MODE = 1;				//Single ended výstup
	SDADC_3.CSR.R = 0x1; 				//Výbìr input ADC pinu (AN5)
	SDADC_3.OSDR.R = 0x10; 				//Nastavení ustálení výsledku
	SDADC_3.RKR.B.RESET_KEY = 0x5AF0;	//reset SDADC block*/
}

/**************************************************************************//*!
* @brief    Routine that returns voltage value of AN5
*
* @details  This routine measure voltage on pin AN5 for calibration purposes.
*
*******************************************************************************/
int SDADC2_voltage(void){
	int SDADC2_result; 					//promìnná pro uložení výsledku, zatím zbyteèná
	SDADC_2.RKR.B.RESET_KEY = 0x5AF0; 	//reset SDADC
	while(SDADC_2.SFR.B.DFEF == 0){} 	//pokud bude blbnout, pøidat timeout <--- èekám na vyèištìní
	SDADC2_result = SDADC_2.CDR.B.CDATA; //pøeètu výsledek SDADC v 16b int
 	//Tady pøevedu výsledek na moment

	return (SDADC2_result);//do return pøidat èasem pøepoèet
}

/**************************************************************************//*!
* @brief    Routine returns torque value of T21WN device
*
* @details  This routine is intended for T21WN device for measurment of torque.
* 			Don't use it with anything else and use voltage divider at the input
* 			of ADC (max Vin = +-5V)
*
*******************************************************************************/
float SDADC2_torque(void){
	SDADC_2.RKR.B.RESET_KEY = 0x5AF0; 				//reset SDADC
	while(SDADC_2.SFR.B.DFEF == 0){} 				//Wait till FIFO is empty and ready to use
	return (SDADC_2.CDR.B.CDATA*0.00031790114037);	//return calculation of SDADC resolute and multiply it with linearized line calculated in excel
}



