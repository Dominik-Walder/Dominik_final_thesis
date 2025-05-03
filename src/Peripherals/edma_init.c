/******************************************************************************
*
*   Copyright 2015-2016 Freescale Semiconductor, Inc.
*   Copyright 2017-2020 NXP
*
/****************************************************************************//*!
*
* @file     edma_init.c
*
* @date     October-20-2020
*
* @brief    DMA channel configuration, EQADC command array, array of constants
*           used to trigger eTPU Resolver
*
*******************************************************************************/

/******************************************************************************
* Included files
*******************************************************************************/
#include "edma_init.h"
#include "etpu_set/etpu_gct.h"
#include "etpu_set/etpu/resolver/etpu_resolver.h"
#include "eqadc.h"
//#include "etpu_set/include/mpc5777c_vars.h"

/******************************************************************************
* Global variables
******************************************************************************/
const uint32_t link_cnst[] = {0,FS_ETPU_RESOLVER_HSR_UPDATE_1ST,0, FS_ETPU_RESOLVER_HSR_UPDATE_2ND};
const uint32_t eqadc_commands[] =
{
		0x80001800,	/* Phase A: EQADC_A ADC 0, channel 24, EOQ */
		0x82101900,	/* Phase B: EQADC_A ADC 1, channel 25, EOQ */
		0x80001A00,	/* Phase C: EQADC_B ADC 0, channel 26, EOQ */
		0x82101C00,	/* DCB	  : EQADC_B ADC 1, channel 28, EOQ */
};

/******************************************************************************
* Global functions
******************************************************************************/
/**************************************************************************//*!
* @brief    Routine that performs configuration of eDMA channels
*
*
* @details  eDMA channels used to cooperate with eTPU Resolver, channels to
*           transfer commands for eEQADC, channels to transfer  eQADC results.
*
*******************************************************************************/
void edma_init(void)
{
	uint8_t hsr_ch = 48;

	/* DMA B ch 36 - SDADC0 result ready
	   DMA B ch 39 - SDADC3 result ready */

	/* DMA settings for SDADC_0 request */
	DMA_B.TCD[36].SADDR.R = (uint32_t)&SDADC_1.CDR.R; /* Source address */

	DMA_B.TCD[36].ATTR.B.SMOD = 0;			/* Source address modulo*/
	DMA_B.TCD[36].ATTR.B.SSIZE = 2;		 	/* Source data size: 32bit */
	DMA_B.TCD[36].ATTR.B.DMOD = 0;			/* Destination address modulo */
	DMA_B.TCD[36].ATTR.B.DSIZE = 2;			/* Destination data size: 32bit */
	DMA_B.TCD[36].SOFF.R = 0;				/* Source address signed offset */

	DMA_B.TCD[36].NBYTES.MLNO.R = 32;		/* Inner "minor" byte count */

	DMA_B.TCD[36].SLAST.R = 0;				/* TCD Last Source Address Adjustment */

	DMA_B.TCD[36].DADDR.R = (vuint32_t)resolver_instance.signals_pba;	/* Destination address */

	DMA_B.TCD[36].CITER.ELINKYES.B.ELINK = 1;			/* Enable channel-to-channel linking on minor-loop complete */
	DMA_B.TCD[36].CITER.ELINKYES.B.LINKCH = hsr_ch;	    /* Minor Loop Link Channel Number */
	DMA_B.TCD[36].CITER.ELINKYES.B.CITER = 4;			/* Current major iteration count */

	DMA_B.TCD[36].DOFF.R = 4;							/* Signed destination address offset */

	DMA_B.TCD[36].DLASTSGA.R = -128; 					/* Last destination address adjustment /scatter gather  address */

	DMA_B.TCD[36].BITER.ELINKYES.B.ELINK = 1;			/* Enable channel-to-channel linking on minor loop complete */
	DMA_B.TCD[36].BITER.ELINKYES.B.LINKCH = hsr_ch;	    /* Minor Loop Link Channel Number */
	DMA_B.TCD[36].BITER.ELINKYES.B.BITER = 4;			/* beginning major iteration count */
	DMA_B.TCD[36].CSR.B.MAJORELINK = 1;				    /* Enable channel-to-channel linking on major loop complete */
	DMA_B.TCD[36].CSR.B.MAJORLINKCH = hsr_ch;			/* Major Loop Link Channel Number */


	/* DMA settings for SDADC_3 request */
	DMA_B.TCD[39].SADDR.R = (uint32_t)&SDADC_4.CDR.R; /* Source address */

	DMA_B.TCD[39].ATTR.B.SMOD = 0;			/* Source address modulo*/
	DMA_B.TCD[39].ATTR.B.SSIZE = 2;		 	/* Source data size: 32bit */
	DMA_B.TCD[39].ATTR.B.DMOD = 0;			/* Destination address modulo */
	DMA_B.TCD[39].ATTR.B.DSIZE = 2;			/* Destination data size: 32bit */
	DMA_B.TCD[39].SOFF.R = 0;				/* Source address signed offset */

	DMA_B.TCD[39].NBYTES.MLNO.R = 32;		/* Inner "minor" byte count */

	DMA_B.TCD[39].SLAST.R = 0;				/* TCD Last Source Address Adjustment */

	DMA_B.TCD[39].DADDR.R = (vuint32_t)resolver_instance.signals_pba + 0x80;	/* Destination address */

	DMA_B.TCD[39].CITER.ELINKNO.B.ELINK = 0;			/* Disable channel-to-channel linking on minor-loop complete */
	DMA_B.TCD[39].CITER.ELINKNO.B.CITER = 4;			/* Current major iteration count */

	DMA_B.TCD[39].DOFF.R = 4;							/* Signed destination address offset */

	DMA_B.TCD[39].DLASTSGA.R = -128; 					/* Last destination address adjustment /scatter gather  address */

	DMA_B.TCD[39].BITER.ELINKNO.B.ELINK = 0;			/* Disable channel-to-channel linking on minor loop complete */
	DMA_B.TCD[39].BITER.ELINKNO.B.BITER = 4;			/* Beginning major iteration count */
	DMA_B.TCD[39].CSR.B.MAJORELINK = 0;				    /* Disable channel-to-channel linking on major loop complete */

	/* DMA settings for hsr link */
	DMA_B.TCD[hsr_ch].SADDR.R = (uint32_t)&link_cnst[0]; /* Source address */

	DMA_B.TCD[hsr_ch].ATTR.B.SMOD = 0;			/* Source address modulo*/
	DMA_B.TCD[hsr_ch].ATTR.B.SSIZE = 2;		 	/* Source data size: 32bit */
	DMA_B.TCD[hsr_ch].ATTR.B.DMOD = 0;			/* Destination address modulo */
	DMA_B.TCD[hsr_ch].ATTR.B.DSIZE = 2;			/* Destination data size: 32bit */
	DMA_B.TCD[hsr_ch].SOFF.R = 4;				/* Source address signed offset */

	DMA_B.TCD[hsr_ch].NBYTES.MLNO.R = 4;		/* Inner "minor" byte count */

	DMA_B.TCD[hsr_ch].SLAST.R = -16;			/* TCD Last Source Address Adjustment */

	DMA_B.TCD[hsr_ch].DADDR.R = (uint32_t)&eTPU->CHAN[resolver_instance.chan_num_ato].HSRR.R;	/* Destination address */

	DMA_B.TCD[hsr_ch].CITER.ELINKNO.B.ELINK = 0;		/* Disable channel-to-channel linking on minor-loop complete */
	DMA_B.TCD[hsr_ch].CITER.ELINKNO.B.CITER = 4;		/* Current major iteration count */

	DMA_B.TCD[hsr_ch].DOFF.R = 0;						/* Signed destination address offset */

	DMA_B.TCD[hsr_ch].DLASTSGA.R = 0; 					/* Last destination address adjustment /scatter gather  address */

	DMA_B.TCD[hsr_ch].BITER.ELINKNO.B.ELINK = 0;		/* Disable channel-to-channel linking on minor loop complete */
	DMA_B.TCD[hsr_ch].BITER.ELINKNO.B.BITER = 4;		/* Beginning major iteration count */
	DMA_B.TCD[hsr_ch].CSR.B.MAJORELINK = 0;				/* Disable channel-to-channel linking on major loop complete */

	/* DMA settings for eQADC command FIFO */
	/* phaseA - DMA_A source: 0 EQADC_A Command FIFO 0 Fill Flag */
	/* phaseB - DMA_A source: 2 EQADC_A Command FIFO 1 Fill Flag */
	/* phaseC - DMA_B source: 0 EQADC_B Command FIFO 0 Fill Flag */
	/* DCB Voltage - DMA_B source: 2 EQADC_B Command FIFO 1 Fill Flag */

	DMA_A.TCD[0].SADDR.R = (uint32_t)&eqadc_commands[0]; /* Source address */

	DMA_A.TCD[0].ATTR.B.SMOD = 0;			/* Source address modulo*/
	DMA_A.TCD[0].ATTR.B.SSIZE = 2;		 	/* Source data size: 32bit */
	DMA_A.TCD[0].ATTR.B.DMOD = 0;			/* Destination address modulo */
	DMA_A.TCD[0].ATTR.B.DSIZE = 2;			/* Destination data size: 32bit */
	DMA_A.TCD[0].SOFF.R = 0;				/* Source address signed offset */

	DMA_A.TCD[0].NBYTES.MLNO.R = 4;			/* Inner "minor" byte count */

	DMA_A.TCD[0].SLAST.R = 0;				/* TCD Last Source Address Adjustment */

	DMA_A.TCD[0].DADDR.R = (uint32_t)&EQADC_A.CFPR[0].R;	/* Destination address CFIFO */

	DMA_A.TCD[0].CITER.ELINKNO.B.ELINK = 0;			/* Disable channel-to-channel linking on minor-loop complete */
	DMA_A.TCD[0].CITER.ELINKNO.B.CITER = 1;			/* Current major iteration count */

	DMA_A.TCD[0].DOFF.R = 0;						/* Signed destination address offset */

	DMA_A.TCD[0].DLASTSGA.R = 0; 					/* Last destination address adjustment /scatter gather  address */

	DMA_A.TCD[0].BITER.ELINKNO.B.ELINK = 0;			/* Disable channel-to-channel linking on minor loop complete */
	DMA_A.TCD[0].BITER.ELINKNO.B.BITER = 1;			/* Beginning major iteration count */
	DMA_A.TCD[0].CSR.B.MAJORELINK = 0;				/* Disable channel-to-channel linking on major loop complete */

	DMA_A.TCD[2].SADDR.R = (uint32_t)&eqadc_commands[1]; /* Source address */

	DMA_A.TCD[2].ATTR.B.SMOD = 0;			/* Source address modulo*/
	DMA_A.TCD[2].ATTR.B.SSIZE = 2;		 	/* Source data size: 32bit */
	DMA_A.TCD[2].ATTR.B.DMOD = 0;			/* Destination address modulo */
	DMA_A.TCD[2].ATTR.B.DSIZE = 2;			/* Destination data size: 32bit */
	DMA_A.TCD[2].SOFF.R = 0;				/* Source address signed offset */

	DMA_A.TCD[2].NBYTES.MLNO.R = 4;			/* Inner "minor" byte count */

	DMA_A.TCD[2].SLAST.R = 0;				/* TCD Last Source Address Adjustment */

	DMA_A.TCD[2].DADDR.R = (uint32_t)&EQADC_A.CFPR[1].R;	/* Destination address CFIFO */

	DMA_A.TCD[2].CITER.ELINKNO.B.ELINK = 0;			/* Disable channel-to-channel linking on minor-loop complete */
	DMA_A.TCD[2].CITER.ELINKNO.B.CITER = 1;			/* Current major iteration count */

	DMA_A.TCD[2].DOFF.R = 0;						/* Signed destination address offset */

	DMA_A.TCD[2].DLASTSGA.R = 0; 					/* Last destination address adjustment /scatter gather  address */

	DMA_A.TCD[2].BITER.ELINKNO.B.ELINK = 0;			/* Disable channel-to-channel linking on minor loop complete */
	DMA_A.TCD[2].BITER.ELINKNO.B.BITER = 1;			/* Beginning major iteration count */
	DMA_A.TCD[2].CSR.B.MAJORELINK = 0;				/* Disable channel-to-channel linking on major loop complete */

	DMA_B.TCD[0].SADDR.R = (uint32_t)&eqadc_commands[2]; /* Source address */

	DMA_B.TCD[0].ATTR.B.SMOD = 0;			/* Source address modulo*/
	DMA_B.TCD[0].ATTR.B.SSIZE = 2;		 	/* Source data size: 32bit */
	DMA_B.TCD[0].ATTR.B.DMOD = 0;			/* Destination address modulo */
	DMA_B.TCD[0].ATTR.B.DSIZE = 2;			/* Destination data size: 32bit */
	DMA_B.TCD[0].SOFF.R = 0;				/* Source address signed offset */

	DMA_B.TCD[0].NBYTES.MLNO.R = 4;			/* Inner "minor" byte count */

	DMA_B.TCD[0].SLAST.R = 0;				/* TCD Last Source Address Adjustment */

	DMA_B.TCD[0].DADDR.R = (uint32_t)&EQADC_B.CFPR[0].R;	/* Destination address CFIFO */

	DMA_B.TCD[0].CITER.ELINKNO.B.ELINK = 0;			/* Disable channel-to-channel linking on minor-loop complete */
	DMA_B.TCD[0].CITER.ELINKNO.B.CITER = 1;			/* Current major iteration count */

	DMA_B.TCD[0].DOFF.R = 0;						/* Signed destination address offset */

	DMA_B.TCD[0].DLASTSGA.R = 0; 					/* Last destination address adjustment /scatter gather  address */

	DMA_B.TCD[0].BITER.ELINKNO.B.ELINK = 0;			/* Disable channel-to-channel linking on minor loop complete */
	DMA_B.TCD[0].BITER.ELINKNO.B.BITER = 1;			/* Beginning major iteration count */
	DMA_B.TCD[0].CSR.B.MAJORELINK = 0;				/* Disable channel-to-channel linking on major loop complete */

	DMA_B.TCD[2].SADDR.R = (uint32_t)&eqadc_commands[3]; /* Source address */

	DMA_B.TCD[2].ATTR.B.SMOD = 0;			/* Source address modulo*/
	DMA_B.TCD[2].ATTR.B.SSIZE = 2;		 	/* Source data size: 32bit */
	DMA_B.TCD[2].ATTR.B.DMOD = 0;			/* Destination address modulo */
	DMA_B.TCD[2].ATTR.B.DSIZE = 2;			/* Destination data size: 32bit */
	DMA_B.TCD[2].SOFF.R = 0;				/* Source address signed offset */

	DMA_B.TCD[2].NBYTES.MLNO.R = 4;			/* Inner "minor" byte count */

	DMA_B.TCD[2].SLAST.R = 0;				/* TCD Last Source Address Adjustment */

	DMA_B.TCD[2].DADDR.R = (uint32_t)&EQADC_B.CFPR[1].R;	/* Destination address CFIFO */

	DMA_B.TCD[2].CITER.ELINKNO.B.ELINK = 0;			/* Disable channel-to-channel linking on minor-loop complete */
	DMA_B.TCD[2].CITER.ELINKNO.B.CITER = 1;			/* Current major iteration count */

	DMA_B.TCD[2].DOFF.R = 0;						/* Signed destination address offset */

	DMA_B.TCD[2].DLASTSGA.R = 0; 					/* Last destination address adjustment /scatter gather  address */

	DMA_B.TCD[2].BITER.ELINKNO.B.ELINK = 0;			/* Disable channel-to-channel linking on minor loop complete */
	DMA_B.TCD[2].BITER.ELINKNO.B.BITER = 1;			/* Beginning major iteration count */
	DMA_B.TCD[2].CSR.B.MAJORELINK = 0;				/* Disable channel-to-channel linking on major loop complete */


	/* DMA settings for eQADC result FIFO */
	/* phaseA - DMA_A source: 1 EQADC_A Receive FIFO 0 Drain Flag*/
	/* phaseB - DMA_A source: 3 EQADC_A Receive FIFO 1 Drain Flag */
	/* phaseC - DMA_B source: 1 EQADC_B Receive FIFO 0 Drain Flag */
	/* DCB Voltage - DMA_B source: 3 EQADC_B Receive FIFO 1 Drain Flag */

	DMA_A.TCD[1].SADDR.R = (uint32_t)&EQADC_A.RFPR[0].R; /* Source address */

	DMA_A.TCD[1].ATTR.B.SMOD = 0;			/* Source address modulo*/
	DMA_A.TCD[1].ATTR.B.SSIZE = 2;		 	/* Source data size: 32bit */
	DMA_A.TCD[1].ATTR.B.DMOD = 0;			/* Destination address modulo */
	DMA_A.TCD[1].ATTR.B.DSIZE = 2;			/* Destination data size: 32bit */
	DMA_A.TCD[1].SOFF.R = 0;				/* Source address signed offset */

	DMA_A.TCD[1].NBYTES.MLNO.R = 4;			/* Inner "minor" byte count */

	DMA_A.TCD[1].SLAST.R = 0;				/* TCD Last Source Address Adjustment */

	DMA_A.TCD[1].DADDR.R = 0xC3FC8000 + 0x1000;	    /* Destination address eTPU AS signals array */

	DMA_A.TCD[1].CITER.ELINKNO.B.ELINK = 0;			/* Disable channel-to-channel linking on minor-loop complete */
	DMA_A.TCD[1].CITER.ELINKNO.B.CITER = 1;			/* Current major iteration count */

	DMA_A.TCD[1].DOFF.R = 0;						/* Signed destination address offset */

	DMA_A.TCD[1].DLASTSGA.R = 0; 					/* Last destination address adjustment /scatter gather  address */

	DMA_A.TCD[1].BITER.ELINKNO.B.ELINK = 0;			/* Disable channel-to-channel linking on minor loop complete */
	DMA_A.TCD[1].BITER.ELINKNO.B.BITER = 1;			/* Beginning major iteration count */
	DMA_A.TCD[1].CSR.B.MAJORELINK = 0;				/* Disable channel-to-channel linking on major loop complete */

	DMA_A.TCD[3].SADDR.R = (uint32_t)&EQADC_A.RFPR[1].R; /* Source address */

	DMA_A.TCD[3].ATTR.B.SMOD = 0;			/* Source address modulo */
	DMA_A.TCD[3].ATTR.B.SSIZE = 2;		 	/* Source data size: 32bit */
	DMA_A.TCD[3].ATTR.B.DMOD = 0;			/* Destination address modulo */
	DMA_A.TCD[3].ATTR.B.DSIZE = 2;			/* Destination data size: 32bit */
	DMA_A.TCD[3].SOFF.R = 0;				/* Source address signed offset */

	DMA_A.TCD[3].NBYTES.MLNO.R = 4;			/* Inner "minor" byte count */

	DMA_A.TCD[3].SLAST.R = 0;				/* TCD Last Source Address Adjustment */

	DMA_A.TCD[3].DADDR.R = 0xC3FC8000 + 0x1004;	    /* Destination address eTPU AS signals array */

	DMA_A.TCD[3].CITER.ELINKNO.B.ELINK = 0;			/* Disable channel-to-channel linking on minor-loop complete */
	DMA_A.TCD[3].CITER.ELINKNO.B.CITER = 1;			/* Current major iteration count */

	DMA_A.TCD[3].DOFF.R = 0;						/* Signed destination address offset */

	DMA_A.TCD[3].DLASTSGA.R = 0; 					/* Last destination address adjustment /scatter gather  address */

	DMA_A.TCD[3].BITER.ELINKNO.B.ELINK = 0;			/* Disable channel-to-channel linking on minor loop complete */
	DMA_A.TCD[3].BITER.ELINKNO.B.BITER = 1;			/* Beginning major iteration count */
	DMA_A.TCD[3].CSR.B.MAJORELINK = 0;				/* Disable channel-to-channel linking on major loop complete */

	DMA_B.TCD[1].SADDR.R = (uint32_t)&EQADC_B.RFPR[0].R; /* Source address */

	DMA_B.TCD[1].ATTR.B.SMOD = 0;			/* Source address modulo*/
	DMA_B.TCD[1].ATTR.B.SSIZE = 2;		 	/* Source data size: 32bit */
	DMA_B.TCD[1].ATTR.B.DMOD = 0;			/* Destination address modulo */
	DMA_B.TCD[1].ATTR.B.DSIZE = 2;			/* Destination data size: 32bit */
	DMA_B.TCD[1].SOFF.R = 0;				/* Source address signed offset */

	DMA_B.TCD[1].NBYTES.MLNO.R = 4;			/* Inner "minor" byte count */

	DMA_B.TCD[1].SLAST.R = 0;				/* TCD Last Source Address Adjustment */

	DMA_B.TCD[1].DADDR.R = 0xC3FC8000 + 0x1008;	    /* Destination address eTPU AS signals array */

	DMA_B.TCD[1].CITER.ELINKNO.B.ELINK = 0;			/* Disable channel-to-channel linking on minor-loop complete */
	DMA_B.TCD[1].CITER.ELINKNO.B.CITER = 1;			/* Current major iteration count */

	DMA_B.TCD[1].DOFF.R = 0;						/* Signed destination address offset */

	DMA_B.TCD[1].DLASTSGA.R = 0; 					/* Last destination address adjustment /scatter gather  address */

	DMA_B.TCD[1].BITER.ELINKNO.B.ELINK = 0;			/* Disable channel-to-channel linking on minor loop complete */
	DMA_B.TCD[1].BITER.ELINKNO.B.BITER = 1;			/* Beginning major iteration count */
	DMA_B.TCD[1].CSR.B.MAJORELINK = 0;				/* Disable channel-to-channel linking on major loop complete */

	DMA_B.TCD[3].SADDR.R = (uint32_t)&EQADC_B.RFPR[1].R; /* Source address */

	DMA_B.TCD[3].ATTR.B.SMOD = 0;			/* Source address modulo*/
	DMA_B.TCD[3].ATTR.B.SSIZE = 2;		 	/* Source data size: 32bit */
	DMA_B.TCD[3].ATTR.B.DMOD = 0;			/* Destination address modulo */
	DMA_B.TCD[3].ATTR.B.DSIZE = 2;			/* Destination data size: 32bit */
	DMA_B.TCD[3].SOFF.R = 0;				/* Source address signed offset */

	DMA_B.TCD[3].NBYTES.MLNO.R = 4;			/* Inner "minor" byte count */

	DMA_B.TCD[3].SLAST.R = 0;				/* TCD Last Source Address Adjustment */

	DMA_B.TCD[3].DADDR.R = 0xC3FC8000 + 0x100C;	    /* Destination address eTPU AS signals array */

	DMA_B.TCD[3].CITER.ELINKNO.B.ELINK = 0;			/* Disable channel-to-channel linking on minor-loop complete */
	DMA_B.TCD[3].CITER.ELINKNO.B.CITER = 1;			/* Current major iteration count */

	DMA_B.TCD[3].DOFF.R = 0;						/* Signed destination address offset */

	DMA_B.TCD[3].DLASTSGA.R = 0; 					/* Last destination address adjustment /scatter gather  address */

	DMA_B.TCD[3].BITER.ELINKNO.B.ELINK = 0;			/* Disable channel-to-channel linking on minor loop complete */
	DMA_B.TCD[3].BITER.ELINKNO.B.BITER = 1;			/* Beginning major iteration count */
	DMA_B.TCD[3].CSR.B.MAJORELINK = 0;				/* Disable channel-to-channel linking on major loop complete */

}

/**************************************************************************//*!
* @brief    Routine that starts configured eDMA channels
*
* @return   none
*
*******************************************************************************/
void edma_start(void)
{
	/* Enable eDMA channels*/
	DMA_A.SERQ.R = 0;		/* EQADC_A Command FIFO 0 Fill */
	DMA_A.SERQ.R = 1;		/* EQADC_A Receive FIFO 0 Drain */
	DMA_A.SERQ.R = 2;		/* EQADC_A Command FIFO 1 Fill */
	DMA_A.SERQ.R = 3;		/* EQADC_A Receive FIFO 1 Drain */

	DMA_B.SERQ.R = 0;		/* EQADC_B Command FIFO 0 Fill */
	DMA_B.SERQ.R = 1;		/* EQADC_B Receive FIFO 0 Drain */
	DMA_B.SERQ.R = 2;		/* EQADC_B Command FIFO 1 Fill */
	DMA_B.SERQ.R = 3;		/* EQADC_B Receive FIFO 1 Drain */
	DMA_B.SERQ.R = 36;		/* SDADC0 result ready */
	DMA_B.SERQ.R = 39;		/* SDADC3 result ready */
	DMA_B.SERQ.R = 48;	    /* linked by DMA_B ch 36 */

}

