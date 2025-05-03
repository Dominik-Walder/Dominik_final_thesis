/******************************************************************************
*
*   Copyright 2016 Freescale Semiconductor, Inc.
*   Copyright 2017-2020 NXP
*
****************************************************************************//*!
*
* @file     dspi.c
*
* @date     October-20-2020
*
* @brief    DSPI configuration file
*
*******************************************************************************/

/******************************************************************************
* Included files
*******************************************************************************/
#include <MPC577xC.h>
#include "dspi.h"

/******************************************************************************
* Global functions
******************************************************************************/
/**************************************************************************//*!
* @brief    Routine that performs configuration of DSPI
*
* @details  This routine performs DSPI interface configuration for communication
*           with MC33937 driver
* @return   N/A
*
*******************************************************************************/

void dspi_init(uint8_t module, uint8_t mode, uint8_t com_mode, uint8_t ctar)
{

	switch(module){
	case DSPI_A_MODULE:
			DSPI_A.MCR.B.HALT           =  0x1;      /* Module is set to HALT mode to perform initialization */
			DSPI_A.MCR.B.MDIS           =  0x1;      /* Module is disabled during initialization */
			DSPI_A.MCR.B.MSTR           =  mode;     /* Select module mode: MASTER or SLAVE */
			DSPI_A.MCR.B.PCSIS          = 0x3F;      /* The chip select inactive state is high */
			DSPI_A.MCR.B.DIS_RXF        =  0x1;      /* RX FIFO disabled */
			DSPI_A.MCR.B.DIS_TXF        =  0x1;      /* TX FIFO disabled */
			DSPI_A.MCR.B.CLR_TXF        =  0x1;      /* Clear the TX FIFO counter */
			DSPI_A.MCR.B.CLR_RXF        =  0x1;      /* Clear the RX FIFO counter */

			DSPI_A.MODE.CTAR[ctar].B.FMSZ  =  0x7;   /* Frame size is FRMSZ + 1 = 8bit per frame */
			DSPI_A.MODE.CTAR[ctar].B.DBR   =  0x0;   /* Duty cycle of the SCK is 50/50 */
			DSPI_A.MODE.CTAR[ctar].B.CPOL  =  0x0;   /* Clock polarity: the inactive state value of SCK is low */
			DSPI_A.MODE.CTAR[ctar].B.CPHA  =  0x1;   /* Clock phase: Data is changed on the leading edge of SCK */
			DSPI_A.MODE.CTAR[ctar].B.LSBFE =  0x0;   /* LSB first: Data is transferred MSB first */
			DSPI_A.MODE.CTAR[ctar].B.CSSCK =  0x3;   /* PCS to SCK Delay Scaler set to 16 */
			DSPI_A.MODE.CTAR[ctar].B.ASC   =  0x3;   /* After SCK Delay Scaler set to 16 */
			DSPI_A.MODE.CTAR[ctar].B.DT    =  0x7;   /* Delay After Transfer Scaler set to 128 */
			DSPI_A.MODE.CTAR[ctar].B.BR    =  0x3;   /* Baud rate scaler is set to be 8, resultant DSPI clock is 100 MHZ/(2x8) = 6.25 MHz */

			DSPI_A.RSER.R               =  0x0;      /* No interrupts or DMA transfers enabled */
		break;
	case DSPI_B_MODULE:
			DSPI_B.MCR.B.HALT           =  0x1;      /* Module is set to HALT mode to perform initialization */
			DSPI_B.MCR.B.MDIS           =  0x1;      /* Module is disabled during initialization */
			DSPI_B.MCR.B.MSTR           =  mode;     /* Select module mode: MASTER or SLAVE */
			DSPI_B.MCR.B.PCSIS          = 0x3F;      /* The chip select inactive state is high */
			DSPI_B.MCR.B.DIS_RXF        =  0x1;      /* RX FIFO disabled */
			DSPI_B.MCR.B.DIS_TXF        =  0x1;      /* TX FIFO disabled */
			DSPI_B.MCR.B.CLR_TXF        =  0x1;      /* Clear the TX FIFO counter */
			DSPI_B.MCR.B.CLR_RXF        =  0x1;      /* Clear the RX FIFO counter */

			DSPI_B.MODE.CTAR[ctar].B.FMSZ  =  0x7;   /* Frame size is FRMSZ + 1 = 8bit per frame */
			DSPI_B.MODE.CTAR[ctar].B.DBR   =  0x0;   /* Duty cycle of the SCK is 50/50 */
			DSPI_B.MODE.CTAR[ctar].B.CPOL  =  0x0;   /* Clock polarity: the inactive state value of SCK is low */
			DSPI_B.MODE.CTAR[ctar].B.CPHA  =  0x1;   /* Clock phase: Data is changed on the leading edge of SCK */
			DSPI_B.MODE.CTAR[ctar].B.LSBFE =  0x0;   /* LSB first: Data is transferred MSB first */
			DSPI_B.MODE.CTAR[ctar].B.CSSCK =  0x3;   /* PCS to SCK Delay Scaler set to 16 */
			DSPI_B.MODE.CTAR[ctar].B.ASC   =  0x3;   /* After SCK Delay Scaler set to 16 */
			DSPI_B.MODE.CTAR[ctar].B.DT    =  0x7;   /* Delay After Transfer Scaler set to 128 */
			DSPI_B.MODE.CTAR[ctar].B.BR    =  0x3;   /* Baud rate scaler is set to be 8, resultant DSPI clock is 100 MHZ/(2x8) = 6.25 MHz */

			DSPI_B.RSER.R               =  0x0;      /* No interrupts or DMA transfers enabled */
		break;
	case DSPI_C_MODULE:
			DSPI_C.MCR.B.HALT           =  0x1;      /* Module is set to HALT mode to perform initialization */
			DSPI_C.MCR.B.MDIS           =  0x1;      /* Module is disabled during initialization */
			DSPI_C.MCR.B.MSTR           =  mode;     /* Select module mode: MASTER or SLAVE */
			DSPI_C.MCR.B.PCSIS          = 0x3F;      /* The chip select inactive state is high */
			DSPI_C.MCR.B.DIS_RXF        =  0x1;      /* RX FIFO disabled */
			DSPI_C.MCR.B.DIS_TXF        =  0x1;      /* TX FIFO disabled */
			DSPI_C.MCR.B.CLR_TXF        =  0x1;      /* Clear the TX FIFO counter */
			DSPI_C.MCR.B.CLR_RXF        =  0x1;      /* Clear the RX FIFO counter */

			DSPI_C.MODE.CTAR[ctar].B.FMSZ  =  0x7;   /* Frame size is FRMSZ + 1 = 8bit per frame */
			DSPI_C.MODE.CTAR[ctar].B.DBR   =  0x0;   /* Duty cycle of the SCK is 50/50 */
			DSPI_C.MODE.CTAR[ctar].B.CPOL  =  0x0;   /* Clock polarity: the inactive state value of SCK is low */
			DSPI_C.MODE.CTAR[ctar].B.CPHA  =  0x1;   /* Clock phase: Data is changed on the leading edge of SCK */
			DSPI_C.MODE.CTAR[ctar].B.LSBFE =  0x0;   /* LSB first: Data is transferred MSB first */
			DSPI_C.MODE.CTAR[ctar].B.CSSCK =  0x3;   /* PCS to SCK Delay Scaler set to 16 */
			DSPI_C.MODE.CTAR[ctar].B.ASC   =  0x3;   /* After SCK Delay Scaler set to 16 */
			DSPI_C.MODE.CTAR[ctar].B.DT    =  0x7;   /* Delay After Transfer Scaler set to 128 */
			DSPI_C.MODE.CTAR[ctar].B.BR    =  0x3;   /* Baud rate scaler is set to be 8, resultant DSPI clock is 100 MHZ/(2x8) = 6.25 MHz */

			DSPI_C.RSER.R               =  0x0;      /* No interrupts or DMA transfers enabled */
		break;
	case DSPI_D_MODULE:
			DSPI_D.MCR.B.HALT           =  0x1;      /* Module is set to HALT mode to perform initialization */
			DSPI_D.MCR.B.MDIS           =  0x1;      /* Module is disabled during initialization */
			DSPI_D.MCR.B.MSTR           =  mode;     /* Select module mode: MASTER or SLAVE */
			DSPI_D.MCR.B.PCSIS          = 0x3F;      /* The chip select inactive state is high */
			DSPI_D.MCR.B.DIS_RXF        =  0x1;      /* RX FIFO disabled */
			DSPI_D.MCR.B.DIS_TXF        =  0x1;      /* TX FIFO disabled */
			DSPI_D.MCR.B.CLR_TXF        =  0x1;      /* Clear the TX FIFO counter */
			DSPI_D.MCR.B.CLR_RXF        =  0x1;      /* Clear the RX FIFO counter */

			DSPI_D.MODE.CTAR[ctar].B.FMSZ  =  0x7;   /* Frame size is FRMSZ + 1 = 8bit per frame */
			DSPI_D.MODE.CTAR[ctar].B.DBR   =  0x0;   /* Duty cycle of the SCK is 50/50 */
			DSPI_D.MODE.CTAR[ctar].B.CPOL  =  0x0;   /* Clock polarity: the inactive state value of SCK is low */
			DSPI_D.MODE.CTAR[ctar].B.CPHA  =  0x1;   /* Clock phase: Data is changed on the leading edge of SCK */
			DSPI_D.MODE.CTAR[ctar].B.LSBFE =  0x0;   /* LSB first: Data is transferred MSB first */
			DSPI_D.MODE.CTAR[ctar].B.CSSCK =  0x3;   /* PCS to SCK Delay Scaler set to 16 */
			DSPI_D.MODE.CTAR[ctar].B.ASC   =  0x3;   /* After SCK Delay Scaler set to 16 */
			DSPI_D.MODE.CTAR[ctar].B.DT    =  0x7;   /* Delay After Transfer Scaler set to 128 */
			DSPI_D.MODE.CTAR[ctar].B.BR    =  0x3;   /* Baud rate scaler is set to be 8, resultant DSPI clock is 100 MHZ/(2x8) = 6.25 MHz */

			DSPI_D.RSER.R               =  0x0;      /* No interrupts or DMA transfers enabled */
		break;
	case DSPI_E_MODULE:
			DSPI_E.MCR.B.HALT           =  0x1;      /* Module is set to HALT mode to perform initialization */
			DSPI_E.MCR.B.MDIS           =  0x1;      /* Module is disabled during initialization */
			DSPI_E.MCR.B.MSTR           =  mode;     /* Select module mode: MASTER or SLAVE */
			DSPI_E.MCR.B.PCSIS          = 0x3F;      /* The chip select inactive state is high */
			DSPI_E.MCR.B.DIS_RXF        =  0x1;      /* RX FIFO disabled */
			DSPI_E.MCR.B.DIS_TXF        =  0x1;      /* TX FIFO disabled */
			DSPI_E.MCR.B.CLR_TXF        =  0x1;      /* Clear the TX FIFO counter */
			DSPI_E.MCR.B.CLR_RXF        =  0x1;      /* Clear the RX FIFO counter */

			DSPI_E.MODE.CTAR[ctar].B.FMSZ  =  0x7;   /* Frame size is FRMSZ + 1 = 8bit per frame */
			DSPI_E.MODE.CTAR[ctar].B.DBR   =  0x0;   /* Duty cycle of the SCK is 50/50 */
			DSPI_E.MODE.CTAR[ctar].B.CPOL  =  0x0;   /* Clock polarity: the inactive state value of SCK is low */
			DSPI_E.MODE.CTAR[ctar].B.CPHA  =  0x1;   /* Clock phase: Data is changed on the leading edge of SCK */
			DSPI_E.MODE.CTAR[ctar].B.LSBFE =  0x0;   /* LSB first: Data is transferred MSB first */
			DSPI_E.MODE.CTAR[ctar].B.CSSCK =  0x3;   /* PCS to SCK Delay Scaler set to 16 */
			DSPI_E.MODE.CTAR[ctar].B.ASC   =  0x3;   /* After SCK Delay Scaler set to 16 */
			DSPI_E.MODE.CTAR[ctar].B.DT    =  0x7;   /* Delay After Transfer Scaler set to 128 */
			DSPI_E.MODE.CTAR[ctar].B.BR    =  0x3;   /* Baud rate scaler is set to be 8, resultant DSPI clock is 100 MHZ/(2x8) = 6.25 MHz */

			DSPI_E.RSER.R               =  0x0;      /* No interrupts or DMA transfers enabled */
		break;

	}


}

/**************************************************************************//*!
* @brief    Routine that enables DSPI module
*
* @details  This routine configures DSPI module to be out of debug mode, clears
*           the End of Queue flag, clears Halt bit, enables module and checks
*           if the Tx and Rx operations are enabled.
* @return   tBool
*
*******************************************************************************/
tBool dspi_start(void)
{
	tBool statusPass = TRUE;

	/* Following bits have to be cleared: */
	DSPI_B.SR.B.EOQF            = 0x1;   /* Clear the End of Queue flag */
	DSPI_B.MCR.B.FRZ            = 0x0;   /* The device is not in the Debug mode */
    DSPI_B.MCR.B.MDIS           = 0x0;   /* Module is enabled */
	DSPI_B.MCR.B.HALT           = 0x0;   /* HALT bit is cleared -> transfer is started */

	/* Check the status bit if TX and RX operations are enabled */
	while(DSPI_B.SR.B.TXRXS == 0x0);

	return(statusPass);
}

/**************************************************************************//*!
* @brief    Routine that stops DSPI transfers
*
* @details  This routine configures Halt bit that stops DSPI transfers
* @return   tBool
*
*******************************************************************************/
tBool dspi_stop(void)
{
	tBool statusPass = TRUE;
	/* HALT bit is set */
	DSPI_B.MCR.B.HALT = 0x1;

	/* Check the status bit if TX and RX operations are disabled */
	while(DSPI_B.SR.B.TXRXS == 0x1);

	return(statusPass);
}

/**************************************************************************//*!
* @brief    Routine that performs DSPI data transfer
*
* @details  This routine performs DSPI interface configuration for communication
*           with MC33937 driver
* @param    cont. This parameter configures continuous peripheral chip select.
* @param    chip_select. Parameter selecting PCS signal to be asserted.
* @param    ctar. Parameter that determines CTAR register to be used for transfer.
* @param    tx_data. 16-bit data to be sent.
* @param    *rx_data. Pointer to variable where to store received data.
*
* @return   tBool
*
*******************************************************************************/
tBool dspi_master_data_tx(uint8_t cont, uint8_t chip_select, uint8_t ctar, uint16_t tx_data, uint8_t *rx_data)
{
    tBool statusPass = TRUE;

	/* Clear the TCF flag */
	DSPI_B.SR.B.TCF = 1;
	/* Write data to PUSH register */
	DSPI_B.PUSHR.PUSHR.R = ((cont & 0x1) << 31) | ((ctar & 0x7) << 28) | ((chip_select & 0x3F) <<  16) | (tx_data);
	/* Wait till the transfer is completed */
	while(DSPI_B.SR.B.TCF == 0);
	*rx_data = DSPI_B.POPR.R;
    return(statusPass);
}

/**************************************************************************//*!
* @brief    Routine that reads DSPI received data
*
* @return   32 bit unsigned integer received data
*
*******************************************************************************/
uint32_t dspi_master_read_data(void)
{
	uint32_t rx_data;
	return rx_data = DSPI_B.POPR.R;
}

