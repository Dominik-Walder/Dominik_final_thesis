/******************************************************************************
*
*   Copyright 2015 Freescale Semiconductor, Inc.
*
****************************************************************************//*!
*
* @file     mpc5777c_vars.h
*
* @date     October-20-2020
*
* @brief    Variables that define some features of the MPC5777C.
*
* @warning  This file must only be included once in every project
*
*******************************************************************************/

/******************************************************************************
 * Included files
*******************************************************************************/

/******************************************************************************
 * Defines and Macros
*******************************************************************************/

/******************************************************************************
* Global Variables Access
******************************************************************************/

/* eTPU characteristics definition */
struct eTPU_struct *eTPU = (struct eTPU_struct *)0xC3FC0000;

uint32_t fs_etpu_code_start =     0xC3FD0000;
uint32_t fs_etpu_data_ram_start = 0xC3FC8000;
uint32_t fs_etpu_data_ram_end =   0xC3FC97FC;
uint32_t fs_etpu_data_ram_ext =   0xC3FCC000;



