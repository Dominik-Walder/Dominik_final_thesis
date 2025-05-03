/*******************************************************************************
*
*   Copyright 2004-2016 Freescale Semiconductor, Inc.
*   Copyright 2017-2020 NXP
*
****************************************************************************//*!
*
* @file    etpu_gct.h_
*
* @author  Milan Brejl [r54529]
*
* @version 1.2
*
* @date    October-20-2020
*
* @brief   This file contains a template of prototypes and defines for
*          the same name .c file. Template was modified for the purpose
*          of MCSPTR2A5775E demo.
*
*******************************************************************************/

/******************************************************************************
 * Included files
*******************************************************************************/
#include "typedefs.h"

/*******************************************************************************
* General Macros
*******************************************************************************/
#define ETPU_ENGINE_A_CHANNEL(x)  (x)
#define ETPU_ENGINE_B_CHANNEL(x)  ((x)+64)

#define FS_ETPU_ENTRY_TABLE_ADDR  (((FS_ETPU_ENTRY_TABLE)>>11) & 0x1F)

/******************************************************************************
* Application Constants and Macros
******************************************************************************/
#define SYS_FREQ_HZ                                                       200E6
#define TCR1_FREQ_HZ                                            (SYS_FREQ_HZ/1)
#define MSEC2TCR1(x)                                 (TCR1_FREQ_HZ/1E3*(x)/1E0)
#define USEC2TCR1(x)                                 (TCR1_FREQ_HZ/1E3*(x)/1E3)
#define NSEC2TCR1(x)                                 (TCR1_FREQ_HZ/1E3*(x)/1E6)
#define UFRACT24(x)                                              ((x)*0xFFFFFF)
#define SFRACT24(x)                                              ((x)*0x7FFFFF)



#define RES_POLE_PAIRS                                                      (1)

#define FRACT2RPM(speed_fract, period_ns) \
         ((speed_fract) * TCR1_FREQ_HZ * 60 / (0x1000000 * NSEC2TCR1(period_ns))
#define RPM2FRACT(rpm, period_ns) \
                ((rpm) * NSEC2TCR1(period_ns) / TCR1_FREQ_HZ * (0x1000000 / 60))

#define DEG2FRACT(deg)                                       ((deg)*0x200000/45)
#define FRACT2DEG(angle_fract)                             ((angle)*45/0x200000)

/*******************************************************************************
* Define Functions to Channels
*******************************************************************************/
#define ETPU_PWMM_MASTER_CH		        ETPU_ENGINE_A_CHANNEL(0)
#define ETPU_PWMM_PHASEA_BASE_CH        ETPU_ENGINE_A_CHANNEL(16)
#define ETPU_PWMM_PHASEA_COMPL_CH       ETPU_ENGINE_A_CHANNEL(17)
#define ETPU_PWMM_PHASEB_BASE_CH        ETPU_ENGINE_A_CHANNEL(18)
#define ETPU_PWMM_PHASEB_COMPL_CH       ETPU_ENGINE_A_CHANNEL(19)
#define ETPU_PWMM_PHASEC_BASE_CH        ETPU_ENGINE_A_CHANNEL(20)
#define ETPU_PWMM_PHASEC_COMPL_CH       ETPU_ENGINE_A_CHANNEL(21)

#define ETPU_RESOLVER_EXC_CHAN          ETPU_ENGINE_A_CHANNEL(6)
#define ETPU_RESOLVER_ATO_CHAN          ETPU_ENGINE_B_CHANNEL(8)
#define ETPU_RESOLVER_DIAG_CHAN         ETPU_ENGINE_B_CHANNEL(0)
#define ETPU_RESOLVER_SAMPLE_CHAN       ETPU_ENGINE_A_CHANNEL(27)

#define ETPU_AS_CH                      ETPU_ENGINE_A_CHANNEL(28) /* internally can trigger EQADC */

/*******************************************************************************
* Define Interrupt Enable, DMA Enable and Output Disable
*******************************************************************************/
#define ETPU_CIE_A    (1<<(ETPU_AS_CH))  /* enable interrupt on channel ETPU_<FUNC1>_CHAN */
#define ETPU_DTRE_A   0x00000000         /* enable DMA request on channel ETPU_<FUNC2>_CHAN */
#define ETPU_ODIS_A   0x00000000
#define ETPU_OPOL_A   0x00000000
#define ETPU_CIE_B    0x00000000
#define ETPU_DTRE_B   0x00000000
#define ETPU_ODIS_B   0x00000000
#define ETPU_OPOL_B   0x00000000

/******************************************************************************
* Global Variables Access
******************************************************************************/
/* Global PWMM structures defined in etpu_gct.c */
extern struct pwmm_instance_t pwmm_instance;
extern struct pwmm_config_t  pwmm_config;
extern struct pwmm_inputs_t pwmm_inputs;
extern struct pwmm_states_t pwmm_states;
/* Global RESOLVER structures defined in etpu_gct.c */
extern struct resolver_instance_t      resolver_instance;
extern struct resolver_config_t        resolver_config;
extern struct resolver_outputs_t       resolver_outputs_calculated;
extern struct resolver_outputs_t       resolver_outputs_extrapolated;
extern struct resolver_outputs_t       resolver_outputs_mechanical;
extern struct resolver_diag_measures_t resolver_diag_measures;
extern struct resolver_diag_thresholds_t resolver_diag_thresholds;
extern struct resolver_diag_flags_t    resolver_diag_flags;
extern struct resolver_states_t        resolver_states;
/* Global AS structures defined in etpu_gct.c */
extern struct as_instance_t as_instance;
extern struct as_config_t as_config;
extern struct as_outputs_t as_outputs;
/* Global AS structures defined in etpu_gct.c */
extern struct pmsm_foc_instance_t pmsm_foc_instance;
extern struct pmsm_foc_config_t pmsm_foc_config;
extern struct pmsm_foc_inputs_t pmsm_foc_inputs;
extern struct pmsm_foc_states_t pmsm_foc_states;


/*******************************************************************************
* Function Prototypes
*******************************************************************************/
int32_t my_system_etpu_init (void);
void    my_system_etpu_start(void);

/*******************************************************************************
 *
 * REVISION HISTORY:
 *
 * FILE OWNER: Milan Brejl [r54529]
 *
 * Revision 1.2  2012/04/10  r54529
 * Adjusted to new API style comming with AN4908.
 *  
 * Revision 1.1  2012/04/10  r54529
 * Usage of fs_etpu2_init with autogenerated macro FS_ETPU_ENGINE_MEM_SIZE.
 *  
 * Revision 1.0  2012/02/22  r54529
 * Initial version based on eTPU Graphical Configuration Tool (GCT) output.
 *
 ******************************************************************************/
