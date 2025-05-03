/******************************************************************************
*
*   Copyright 2017-2020 NXP
*
****************************************************************************//*!
*
* @file     eqadc.c
*
* @date     October-20-2020
*
* @brief    eQADC configuration file
*
*******************************************************************************/

/******************************************************************************
* Included files
*******************************************************************************/
#include "eqadc.h"
#include <MPC577xC.h>

/******************************************************************************
* Global functions
******************************************************************************/
/**************************************************************************//*!
* @brief    Routine that performs configuration of eQADC
*
*
* @details  Channels are configured to sample phase currents and DC bus voltage.
*           Channel's FIFOs are triggered from eTPU channel that runs AS
*           function.
*
*******************************************************************************/
void eqadc_init(void)
{
	  uint32_t tmp;

	/* AN24 - Phase A current
	 * AN25 - Phase B current
	 * AN26 - Phase C current
	 * AN28 - DC bus voltage */

	/* SIU config of CFIFO trigger source selection:
	 * eTPU28_A selected to trigger all the FIFOs at one time */

	SIU.ISEL5.B.CTSEL0_A = 0xC;	/* eTPU28_A selected as trigger for eQADC_A Command FIFO 0 */
	SIU.ISEL5.B.CTSEL1_A = 0xC;	/* eTPU28_A selected as trigger for eQADC_A Command FIFO 1 */
	SIU.ISEL7.B.CTSEL0_B = 0xC;	/* eTPU28_A selected as trigger for eQADC_B Command FIFO 0 */
	SIU.ISEL7.B.CTSEL1_B = 0xC;	/* eTPU28_A selected as trigger for eQADC_B Command FIFO 1 */

	/* Enable EQADC_A ADC0 and ADC1 and set ADC clock prescaler, via commands */
	EQADC_A.CFCR0.B.CFINV0 = 1; 				/* Invalidate CFIFO */
	EQADC_A.CFCR0.B.CFINV1 = 1; 				/* Invalidate CFIFO */
	EQADC_A.CFPR[0].R = 0x80800301 + (0 << 25); /* Push ADC0_CR configuration command to CFIFO_0 */
	EQADC_A.CFPR[1].R = 0x80800301 + (1 << 25); /* Push ADC1_CR configuration command to CFIFO_1 */
	EQADC_A.CFCR0.R = 0x04100410; 				/* Set software triggered single scan mode and enable single scan for both ADC0 and ADC1 */
	while (EQADC_A.FISR[0].B.EOQFX != 1) {} 	/* Wait for end-of-queue flag */
	while (EQADC_A.FISR[1].B.EOQFX != 1) {} 	/* Wait for end-of-queue flag */
	EQADC_A.FISR[0].R = ~0UL; 					/* Clear all flags */
	EQADC_A.FISR[1].R = ~0UL; 					/* Clear all flags */

	/* Enable EQADC_B ADC0 and ADC1 and set ADC clock prescaler, via commands */
	EQADC_B.CFCR0.B.CFINV0 = 1; 				/* Invalidate CFIFO */
	EQADC_B.CFCR0.B.CFINV1 = 1; 				/* Invalidate CFIFO */
	EQADC_B.CFPR[0].R = 0x80800301 + (0 << 25); /* Push ADC0_CR configuration command to CFIFO */
	EQADC_B.CFPR[1].R = 0x80800301 + (1 << 25); /* Push ADC0_CR configuration command to CFIFO */
	EQADC_B.CFCR0.R = 0x04100410; 				/* Set software triggered single scan mode and enable single scan for both ADC0 and ADC1 */
	while (EQADC_B.FISR[0].B.EOQFX != 1) {} 	/* Wait for end-of-queue flag */
	while (EQADC_B.FISR[1].B.EOQFX != 1) {} 	/* Wait for end-of-queue flag */
	EQADC_B.FISR[0].R = ~0UL; 					/* Clear all flags */
	EQADC_B.FISR[1].R = ~0UL; 					/* Clear all flags */

	/* eQADC RFIFO 0,1,2 and eDMA 1,3,5 */
	/* Ensure RFIFOs are empty */
	while (EQADC_A.FISR[0].B.RFCTRX > 0) tmp = EQADC_A.RFPR[0].R;
	while (EQADC_A.FISR[1].B.RFCTRX > 0) tmp = EQADC_A.RFPR[1].R;
	while (EQADC_B.FISR[0].B.RFCTRX > 0) tmp = EQADC_B.RFPR[0].R;
	while (EQADC_B.FISR[1].B.RFCTRX > 0) tmp = EQADC_B.RFPR[1].R;
	/* clear all flags */
	EQADC_A.FISR[0].R = ~0UL; /* clear all flags */
	EQADC_A.FISR[1].R = ~0UL; /* clear all flags */
	EQADC_B.FISR[0].R = ~0UL; /* clear all flags */
	EQADC_B.FISR[1].R = ~0UL; /* clear all flags */

	/* RFIFO drain select eDMA and enable */

	EQADC_A.IDCR0.B.RFDE0 = 1; /* Enable RFIFO Drain DMA or Interrupt request */
	EQADC_A.IDCR0.B.RFDE1 = 1; /* Enable RFIFO Drain DMA or Interrupt request */
	EQADC_A.IDCR0.B.CFFE0 = 1; /* Enable CFIFO Drain DMA or Interrupt request */
	EQADC_A.IDCR0.B.CFFE1 = 1; /* Enable CFIFO Drain DMA or Interrupt request */

	EQADC_B.IDCR0.B.RFDE0 = 1; /* Enable RFIFO Drain DMA or Interrupt request */
	EQADC_B.IDCR0.B.RFDE1 = 1; /* Enable RFIFO Drain DMA or Interrupt request */
	EQADC_B.IDCR0.B.CFFE0 = 1; /* Enable CFIFO Drain DMA or Interrupt request */
	EQADC_B.IDCR0.B.CFFE1 = 1; /* Enable CFIFO Drain DMA or Interrupt request */

	EQADC_A.IDCR0.B.RFDS0 = 1; /* Generate DMA request to move data from RFIFOx to the system memory. */
	EQADC_A.IDCR0.B.RFDS1 = 1; /* Generate DMA request to move data from RFIFOx to the system memory. */
	EQADC_A.IDCR0.B.CFFS0 = 1; /* Generate DMA request to move data from the system memory to CFIFOx. */
	EQADC_A.IDCR0.B.CFFS1 = 1; /* Generate DMA request to move data from the system memory to CFIFOx. */

	EQADC_B.IDCR0.B.RFDS0 = 1; /* Generate DMA request to move data from RFIFOx to the system memory. */
	EQADC_B.IDCR0.B.RFDS1 = 1; /* Generate DMA request to move data from RFIFOx to the system memory. */
	EQADC_B.IDCR0.B.CFFS0 = 1; /* Generate DMA request to move data from the system memory to CFIFOx. */
	EQADC_B.IDCR0.B.CFFS1 = 1; /* Generate DMA request to move data from the system memory to CFIFOx. */

}

/**************************************************************************//*!
* @brief    Routine that starts eQADC conversion
*
* @details
*
*******************************************************************************/
void eqadc_start(void)
{
	  /* This effectively starts eQADC conversions (if any) */
	  EQADC_A.CFCR0.R = 0x00D000D0;		/* MODE = Rising edge external trigger, continuous scan for both ADC 0 and ADC1 */
	  EQADC_B.CFCR0.R = 0x00D000D0;		/* MODE = Rising edge external trigger, continuous scan for both ADC 0 and ADC1 */
}
