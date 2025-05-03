/******************************************************************************
*
*   Copyright 2015 Freescale Semiconductor, Inc.
*
****************************************************************************//*!
*
* @file     mpc5777c_vars_c.h
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
struct eTPU_struct *eTPU_C = (struct eTPU_struct *)0xFFE00000;

uint32_t fs_etpu_c_code_start =     0xFFE10000;
uint32_t fs_etpu_c_data_ram_start = 0xFFE08000;
uint32_t fs_etpu_c_data_ram_end =   0xFFE097FC;
uint32_t fs_etpu_c_data_ram_ext =   0xFFE0C000;




