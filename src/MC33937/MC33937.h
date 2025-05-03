/******************************************************************************
*
*   Copyright 2017-2020 NXP
*
****************************************************************************//*!
*
* @file     MC33937.h
*
* @date     October-20-2020
*
* @brief    Header file for MC33937 configuration and runtime routines
*
*******************************************************************************/
#ifndef _MC33937_H_
#define _MC33937_H_

/******************************************************************************
 * Included files
*******************************************************************************/
#include "SWLIBS_Typedefs.h"
#include "mlib.h"
#include "../peripherals/dspi.h"
#include "../peripherals/pit.h"

/******************************************************************************
 * Defines and Macros
*******************************************************************************/
#define MC33937_RESET_PIN	    127
#define MC33937_ENABLE_PIN	    126

#define DEADTIME_PASSED			0
#define DEADTIME_FAILED			1


/******************************************************************************
* Typedefs and structures
******************************************************************************/

/******************************************************************************
* @brief    Structure containing MC33937 configuration mask
*******************************************************************************/
typedef union
{
    uint16_t R;
    struct {
        uint16_t                   :1;
        uint16_t                   :1;
        uint16_t                   :1;
        uint16_t                   :1;
        uint16_t    resetEvent     :1;
        uint16_t    writeErr       :1;
        uint16_t    framingErr     :1;
        uint16_t    phaseErr       :1;
        uint16_t                   :1;
        uint16_t                   :1;
        uint16_t                   :1;
        uint16_t                   :1;
        uint16_t    overCurrent    :1;
        uint16_t    lowVls         :1;
        uint16_t    desaturation   :1;
        uint16_t    overTemp       :1;
    }B;
}mc33937ConfigMask_t;

/******************************************************************************
* @brief    Structure containing MC33937 modes
*******************************************************************************/
typedef union
{
    uint8_t R;
    struct {
        uint8_t                 :1;
        uint8_t                 :1;
        uint8_t                 :1;
        uint8_t                 :1;
        uint8_t    disableDesat :1;
        uint8_t                 :1;
        uint8_t    enableFullOn :1;
        uint8_t    lock         :1;
    }B;
}mc33937Mode_t;

/******************************************************************************
* @brief    Structure containing status register 0 image of the MC33937 device
*******************************************************************************/
typedef union {
    uint8_t R;
    struct {
        uint8_t     resetEvent   :1;    /*!< reset event flag, is set upon exiting /RST */
        uint8_t     writeErr     :1;    /*!< Write Error After the Lock flag */
        uint8_t     framingErr   :1;    /*!< Framing error flag */
        uint8_t     phaseErr     :1;    /*!< Phase error flag */
        uint8_t     overCurrent  :1;    /*!< Over-current event flag */
        uint8_t     lowVls       :1;    /*!< Low VLS voltage flag */
        uint8_t     desaturation :1;    /*!< DESAT flag detected on any channel */
        uint8_t     overTemp     :1;    /*!< TLIM flag detected on any channel */
    } B;
}mc33937SR0_t;

/******************************************************************************
* @brief    Structure containing status register 1 image of the MC33937 device
*******************************************************************************/
typedef union {
    uint8_t R;
    struct {
        uint8_t                  :1;     /*!< reserved */
        uint8_t     desat_mode   :1;     /*!< current state of the Desaturation/Phase Error turn-off mode */
        uint8_t     zds          :1;     /*!< Zero deadtime is commanded */
        uint8_t     calib_overfl :1;     /*!< flag for a Deadtime Calibration Overflow */
        uint8_t     deadtime_cal :1;     /*!< Deadtime calibration occurred */
        uint8_t                  :1;     /*!< reserved */
        uint8_t     fullon       :1;     /*!< present status of FULLON MODE */
        uint8_t     lockbit      :1;     /*!< LockBit indicates the IC regs are locked */
    } B;
}mc33937SR1_t;


/******************************************************************************
* @brief    Structure containing status register 2 image of the MC33937 device
*******************************************************************************/
typedef union {
    uint8_t R;
    struct {
        uint8_t     mask1_3      :1;     /*!< status of the MASK1.3 bit */
        uint8_t     mask1_2      :1;     /*!< status of the MASK1.2 bit */
        uint8_t     mask1_1      :1;     /*!< status of the MASK1.1 bit */
        uint8_t     mask1_0      :1;     /*!< status of the MASK1.0 bit */
        uint8_t     mask0_3      :1;     /*!< status of the MASK0.3 bit */
        uint8_t     mask0_2      :1;     /*!< status of the MASK0.2 bit */
        uint8_t     mask0_1      :1;     /*!< status of the MASK0.1 bit */
        uint8_t     mask0_0      :1;     /*!< status of the MASK0.0 bit */
    } B;
}mc33937SR2_t;

/******************************************************************************
* @brief    Structure containing status of the software driver for mc33937
*******************************************************************************/
typedef union {
    tU32 R;
    struct {
        uint32_t                     :26; /* RESERVED */
        uint32_t outputDisablingError:1;
        uint32_t outputEnablingError :1;
        uint32_t configurationError  :1;
        uint32_t clearingError       :1;
        uint32_t initializationError :1;
        uint32_t deadtimeSetupError  :1;
    } B;
}mc33937StatusSW_t;


/******************************************************************************
* @brief    Structure containing status registers sr0-3 of the MC33937 device
*******************************************************************************/
typedef struct {
    mc33937SR0_t                sr0;    /*!< status register 0 */
    mc33937SR1_t                sr1;    /*!< status register 1 */
    mc33937SR2_t                sr2;    /*!< status register 2 */
    uint8_t                     sr3;    /*!< status register 3 */
    mc33937StatusSW_t           sw;
}MC33937_STATUS_T;

/******************************************************************************
* @brief    Structure containing mode, mask and deadtime config of MC33937
*******************************************************************************/
typedef struct{
    uint16_t                    deadtime;       /*!< define dead time of HS and LS transistors, value in [ns]*/
    mc33937ConfigMask_t         interruptEnable;
    mc33937Mode_t               mode;
}MC33937_CONFIG_DATA_T;

/******************************************************************************
* @brief    Structure containing actual MC33937 data
*******************************************************************************/
typedef struct{
    uint16_t            requiredDeadTimeNs;
    uint16_t            actualDeadTimeNs;
    uint8_t        		mc33937mask0;
    uint8_t         	mc33937mask1;
    uint8_t         	mc33937mode;
}MC33937_DATA_T;

/******************************************************************************
* @brief    Structure containing config data, status and actual MC33937 data
*******************************************************************************/
typedef struct{
    MC33937_DATA_T              data;
    MC33937_STATUS_T            status;
    MC33937_CONFIG_DATA_T       configData;
}MC33937_T;

/******************************************************************************
* Global Variables Access
******************************************************************************/
extern void MC33937_config(void);
extern tBool MC33937_sw_init( MC33937_T *sw_ptr);
extern tBool SetEN(void);
extern tBool ClearEN(void);
extern tBool SetRESET(void);
extern tBool ClearRESET(void);
extern tBool MC33937_enable_output(MC33937_T *ptr);
extern tBool MC33937_disable_output( MC33937_T *ptr);
extern tBool MC33937_clear_faults(MC33937_T *ptr);
extern tBool MC33937_get_status_SR0(MC33937_T *ptr);
extern tBool MC33937_get_status( MC33937_T *ptr);
extern tBool MC33937_set_deadtime(MC33937_T * ptr, uint16_t deadtimeNs);

#endif /* _MC33937_H_ */

