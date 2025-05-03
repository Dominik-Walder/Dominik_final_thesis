/******************************************************************************
*
*   Copyright 2016 Freescale Semiconductor, Inc.
*   Copyright 2017-2020 NXP
*
****************************************************************************//*!
*
* @file     dspi.h
*
* @date     October-20-2020
*
* @brief    Header file for DSPI configuration
*
*******************************************************************************/
#ifndef _DSPI_H_
#define _DSPI_H_

/******************************************************************************
* Included files
*******************************************************************************/
#include "typedefs.h"
#include "SWLIBS_Typedefs.h"

/******************************************************************************
* Defines and Macros
*******************************************************************************/
/* Module defines */
#define DSPI_A_MODULE 0
#define DSPI_B_MODULE 1
#define DSPI_C_MODULE 2
#define DSPI_D_MODULE 3
#define DSPI_E_MODULE 4

/* Mode defines */
#define DSPI_SLAVE   0
#define DSPI_MASTER  1

/* Communication Mode defines */
#define DSPI_SPI   0
#define DSPI_DSI   1
#define DSPI_CSI   2

/* Chip select defines */
#define CS0	0b000001
#define CS1 0b000010
#define CS2 0b000100
#define CS3 0b001000
#define CS4 0b010000
#define CS5 0b100000

/* CTAR defines */
#define CTAR0 0
#define CTAR1 1
#define CTAR2 2
#define CTAR3 3
#define CTAR4 4
#define CTAR5 5
#define CTAR6 6
#define CTAR7 7

/* continuous chip select mode defines */
#define SINGLE_CS     0
#define CONTINUOUS_CS 1

/******************************************************************************
* Global Variables Access
******************************************************************************/
void dspi_init(uint8_t module, uint8_t mode, uint8_t com_mode, uint8_t ctar);
tBool dspi_start(void);
tBool dspi_stop(void);
tBool dspi_master_data_tx(uint8_t cont, uint8_t chip_select, uint8_t ctar, uint16_t tx_data, uint8_t *rx_data);
uint32_t dspi_master_read_data(void);


#endif /* _DSPI_H_ */
