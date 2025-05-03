/*******************************************************************************
*
*   Copyright 2004-2016 Freescale Semiconductor, Inc.
*   Copyright 2017-2020 NXP
*
****************************************************************************//*!
*
* @file    etpu_gct.c_
*
* @author  Milan Brejl [r54529]
*
* @version 1.2
*
* @date    October-20-2020
*
* @brief   This file contains a template of eTPU module initialization.
*          There are 2 functions to be used by the application:
*          - my_system_etpu_init - initialize eTPU global and channel setting
*          - my_system_etpu_start - run the eTPU
*          This template was modified for the purpose of MCSPTR2A5775E demo.
*
*******************************************************************************/

/*******************************************************************************
* Includes
*******************************************************************************/
#include "etpu_gct.h"      /* private header file */
#include "etpu\_utils\etpu_util.h"     /* General C Functions for the eTPU */
#include "etpu\_etpu_set\etpu_set.h"      /* eTPU function set code binary image */
#include "etpu\pwmm\etpu_pwmm.h" 	   /* eTPU PWMM function API */
#include "etpu\resolver\etpu_resolver.h" 	   /* eTPU PWMM function API */
#include "etpu\as\etpu_as.h" 	   /* eTPU PWMM function API */
#include "etpu\pmsm_foc\etpu_pmsm_foc.h" 	   /* eTPU PMSM FOC function API */
#include "include/mpc5777c_vars.h"


/*******************************************************************************
* Global variables
*******************************************************************************/
/** @brief   Pointer to the first free parameter in eTPU DATA RAM */
uint32_t *fs_free_param;

/*******************************************************************************
 * Global eTPU settings - etpu_config structure
 ******************************************************************************/
/** @brief   Structure handling configuration of all global settings */
struct etpu_config_t my_etpu_config =
{
	/* etpu_config.mcr - Module Configuration Register */
	FS_ETPU_GLOBAL_TIMEBASE_DISABLE  /* keep time-bases stopped during intialization (GTBE=0) */
	| FS_ETPU_MISC_DISABLE, /* SCM operation disabled (SCMMISEN=0) */

	/* etpu_config.misc - MISC Compare Register*/
	FS_ETPU_MISC, /* MISC compare value from etpu_set.h */

	/* etpu_config.ecr_a - Engine A Configuration Register */
	FS_ETPU_ENTRY_TABLE_ADDR /* entry table base address = shifted FS_ETPU_ENTRY_TABLE from etpu_set.h */
	| FS_ETPU_CHAN_FILTER_2SAMPLE /* channel filter mode = three-sample mode (CDFC=0) */
	| FS_ETPU_FCSS_DIV2 /* filter clock source selection = div 2 (FSCC=0) */
	| FS_ETPU_FILTER_CLOCK_DIV2 /* filter prescaler clock control = div 2 (FPSCK=0) */
	| FS_ETPU_PRIORITY_PASSING_ENABLE /* scheduler priority passing is enabled (SPPDIS=0) */
	| FS_ETPU_ENGINE_ENABLE, /* engine is enabled (MDIS=0) */

	/* etpu_config.tbcr_a - Time Base Configuration Register A */
	FS_ETPU_TCRCLK_MODE_2SAMPLE /* TCRCLK signal filter control mode = two-sample mode (TCRCF=0x) */
	| FS_ETPU_TCRCLK_INPUT_DIV2CLOCK /* TCRCLK signal filter control clock = div 2 (TCRCF=x0) */
	| FS_ETPU_TCR1CS_DIV1 /* TCR1 clock source = div 1 (TCR1CS=1)*/
	| FS_ETPU_TCR1CTL_DIV2 /* TCR1 source = div 2 (TCR1CTL=2) */
	| FS_ETPU_TCR1_PRESCALER(1) /* TCR1 prescaler = 1 (TCR1P=0) */
	| FS_ETPU_TCR2CTL_DIV8 /* TCR2 source = etpuclk div 8 (TCR2CTL=4) */
	| FS_ETPU_TCR2_PRESCALER(1) /* TCR2 prescaler = 1 (TCR2P=0) */
	| FS_ETPU_ANGLE_MODE_DISABLE, /* TCR2 angle mode is disabled (AM=0) */


	/* etpu_config.stacr_a - Shared Time And Angle Count Register A */
	FS_ETPU_TCR1_STAC_ENABLE /* TCR1 on STAC bus = enabled (REN1=1) */
	| FS_ETPU_TCR1_STAC_SERVER /* TCR1 resource control = server (RSC1=1) */
	| FS_ETPU_TCR1_STAC_SRVSLOT(0) /* TCR1 server slot = 0 (SRV1=0) */
	| FS_ETPU_TCR2_STAC_DISABLE /* TCR2 on STAC bus = disabled (REN2=0) */
	| FS_ETPU_TCR1_STAC_CLIENT /* TCR2 resource control = client (RSC2=0) */
	| FS_ETPU_TCR2_STAC_SRVSLOT(0), /* TCR2 server slot = 0 (SRV2=0) */

	/* etpu_config.ecr_b - Engine B Configuration Register */
	FS_ETPU_ENTRY_TABLE_ADDR /* entry table base address = shifted FS_ETPU_ENTRY_TABLE from etpu_set.h */
	| FS_ETPU_CHAN_FILTER_2SAMPLE /* channel filter mode = three-sample mode (CDFC=0) */
	| FS_ETPU_FCSS_DIV2 /* filter clock source selection = div 2 (FSCC=0) */
	| FS_ETPU_FILTER_CLOCK_DIV2 /* filter prescaler clock control = div 2 (FPSCK=0) */
	| FS_ETPU_PRIORITY_PASSING_ENABLE /* scheduler priority passing is enabled (SPPDIS=0) */
	| FS_ETPU_ENGINE_ENABLE, /* engine is enabled (MDIS=0) */

	/* etpu_config.tbcr_b - Time Base Configuration Register B */
	FS_ETPU_TCRCLK_MODE_2SAMPLE /* TCRCLK signal filter control mode = two-sample mode (TCRCF=0x) */
	| FS_ETPU_TCRCLK_INPUT_DIV2CLOCK /* TCRCLK signal filter control clock = div 2 (TCRCF=x0) */
	| FS_ETPU_TCR1CS_DIV1 /* TCR1 clock source = div 1 (TCR1CS=1)*/
	| FS_ETPU_TCR1CTL_DIV2 /* TCR1 source = div 2 (TCR1CTL=2) */
	| FS_ETPU_TCR1_PRESCALER(1) /* TCR1 prescaler = 1 (TCR1P=0) */
	| FS_ETPU_TCR2CTL_DIV8 /* TCR2 source = etpuclk div 8 (TCR2CTL=4) */
	| FS_ETPU_TCR2_PRESCALER(1) /* TCR2 prescaler = 1 (TCR2P=0) */
	| FS_ETPU_ANGLE_MODE_DISABLE, /* TCR2 angle mode is disabled (AM=0) */

	/* etpu_config.stacr_b - Shared Time And Angle Count Register B */
	FS_ETPU_TCR1_STAC_ENABLE /* TCR1 on STAC bus = enabled (REN1=1) */
	| FS_ETPU_TCR1_STAC_CLIENT /* TCR1 resource control = client (RSC1=0) */
	| FS_ETPU_TCR1_STAC_SRVSLOT(0) /* TCR1 server slot = 0 (SRV1=0) */
	| FS_ETPU_TCR2_STAC_DISABLE /* TCR2 on STAC bus = disabled (REN2=0) */
	| FS_ETPU_TCR1_STAC_CLIENT /* TCR2 resource control = client (RSC2=0) */
	| FS_ETPU_TCR2_STAC_SRVSLOT(0), /* TCR2 server slot = 0 (SRV2=0) */

	/* etpu_config.wdtr_a - Watchdog Timer Register A(eTPU2 only) */
	FS_ETPU_WDM_DISABLED /* watchdog mode = disabled */
	| FS_ETPU_WDTR_WDCNT(0), /* watchdog count = 0 */

	/* etpu_config.wdtr_b - Watchdog Timer Register B (eTPU2 only) */
	FS_ETPU_WDM_DISABLED /* watchdog mode = disabled */
	| FS_ETPU_WDTR_WDCNT(0) /* watchdog count = 0 */
};

/*******************************************************************************
 * eTPU channel settings - PWMM
 ******************************************************************************/
/** @brief   Initialization of PWMM structures */

struct pwmm_instance_t pwmm_instance =
{
		ETPU_PWMM_MASTER_CH,  /**< Channel number of the PWMM master channel. */
		ETPU_PWMM_PHASEA_BASE_CH, /**< Channel number of the Phase A channel. */
		ETPU_PWMM_PHASEB_BASE_CH, /**< Channel number of the Phase B channel. */
		ETPU_PWMM_PHASEC_BASE_CH, /**< Channel number of the Phase C channel. */
		FS_ETPU_PRIORITY_MIDDLE,        /**< Channel priority for all PWMM channels. */
		FS_ETPU_PWMM_FM0_COMPLEMENTARY_PAIRS,       /**< Type of phases. */
		FS_ETPU_PWMM_POLARITY_BASE_ACTIVE_LOW |FS_ETPU_PWMM_POLARITY_COMPL_ACTIVE_HIGH,       /**< Base and complementary channel polarity */
		(uint24_t)USEC2TCR1(180),
        0            /**< Channel parameter base address */
};

/** A structure to represent a configuration of PWMM.
 *  It includes PWMM configuration items which can be changed in run-time. */
struct pwmm_config_t pwmm_config =
{
		FS_ETPU_PWMM_FM1_FRAME_UPDATE_ONLY, /**< selection of PWM update position. */
		FS_ETPU_PWMM_MODULATION_SIGNED,     /**< Selection of modulation */
		FS_ETPU_PWMM_MODE_CENTER_ALIGNED,   /**< PWM Mode selection */
		NSEC2TCR1(100000),                  /**< PWM period as a number of TCR1 cycles. */
		USEC2TCR1(1),                       /**< PWM dead-time as a number of TCR1 cycles.*/
		USEC2TCR1(1),                       /**< Minimum pulse width as number of TCR1 cycles. */
		USEC2TCR1(7)                        /**< A time period (number of TCR1 cycles) that is needed to perform an update of all PWM phases. */
};

/** A structure to represent inputs of PWMM used for update of generated PWM
 *  duty-cycles. Based on the selected PWMM modulation, there are different
 *  meanings of input_a, input_b and input_c. */
struct pwmm_inputs_t pwmm_inputs;

struct pwmm_states_t pwmm_states;


/*******************************************************************************
 * eTPU channel settings - RESOLVER
 ******************************************************************************/
/** @brief   Initialization of RESOLVER structures */
struct resolver_instance_t resolver_instance =
{
  ETPU_RESOLVER_EXC_CHAN,   /* chan_num_exc */
  ETPU_RESOLVER_ATO_CHAN,   /* chan_num_ato */
  ETPU_RESOLVER_DIAG_CHAN,  /* chan_num_diag */
  ETPU_RESOLVER_SAMPLE_CHAN,/* chan_num_sample */
  ETPU_RESOLVER_EXC_CHAN,   /* chan_num_dma */
  FS_ETPU_PRIORITY_HIGH,    /* priority */
  USEC2TCR1(125),           /* start_offset */
  USEC2TCR1(19),            /* adc_delay */
  0,                        /* *cpba */  /* 0 for automatic allocation */
  0,                        /* *signals_pba */  /* 0 for automatic allocation */
  0                         /* *checker_signals_pba  - will be allocated automatically */
};

struct resolver_config_t resolver_config =
{
  FS_ETPU_RESOLVER_SEMAPHORE_0,
  FS_ETPU_RESOLVER_OPTIONS_CALCULATION_ON +
  FS_ETPU_RESOLVER_OPTIONS_DIAG_MEASURES_ON +
  //FS_ETPU_RESOLVER_OPTIONS_DIAG_FLAGS_ON +
  FS_ETPU_RESOLVER_OPTIONS_EXC_ADAPTATION_ON +
  FS_ETPU_RESOLVER_OPTIONS_EXC_GENERATION_ON, /* options */
  NSEC2TCR1(99840), /* excitation_period */
  SFRACT24(0.070597541), /* ato_p_gain */
  SFRACT24(0.002492006), /* ato_i_gain */
  SFRACT24(0.00000), /* exc_p_gain */
  SFRACT24(0.00012), /* exc_i_gain */
  SFRACT24(0.9)     /* q_ewma_speed */
};

struct resolver_diag_thresholds_t resolver_diag_thresholds =
{
  SFRACT24(0.50),     /* ampl_thrs          */
  SFRACT24(0.15),     /* low_ampl_thrs      */
  SFRACT24(0.010),    /* mean_thrs          */
  SFRACT24(0.38*0.38), /* vec_min_thrs       */
  SFRACT24(0.50*0.50), /* vec_max_thrs       */
  SFRACT24(0.1),       /* ampl_diff_thrs     */
  SFRACT24(5.0/360), /* ato_angle_err_thrs */
  RPM2FRACT(10000, 99840)  /* ato_speed_max_thrs     */
};

struct resolver_outputs_t       resolver_outputs_calculated;
struct resolver_outputs_t       resolver_outputs_extrapolated;
struct resolver_outputs_t       resolver_outputs_mechanical;
struct resolver_diag_measures_t resolver_diag_measures;
struct resolver_diag_flags_t    resolver_diag_flags;
struct resolver_states_t        resolver_states;

/*******************************************************************************
 * eTPU channel settings - ANALOG SENSING
 ******************************************************************************/
/** @brief   Initialization of ANALOG SENSING structures */
struct as_instance_t  as_instance =
{
	ETPU_AS_CH,				/* channel number */
	FS_ETPU_PRIORITY_HIGH,  /**< Channel priority for the AS channel. */
	FS_ETPU_AS_HSR_INIT_MODE_SYNC,  /* AS mode */
	FS_ETPU_AS_POLARITY_PULSE_HIGH, /*  polarity */
	4,  /* Count of AS signals processed */
    0,  /* Channel parameter base address */
    0   /* Signal array base address */
};

/** A structure to represent a single AS signal processing configuration.
 *  It includes both static and dynamic values. */
struct as_signal_config_t as_signal_config[4] =
{
	{
		0, /* queue_offset */
		1, /*  gain 2^10 to have result in 24 bit */
		0x2000,//0x800000, /* dc_offset */
		SFRACT24(0) /* forget_factor */
	},
	{
		4, /* queue_offset */
		1, /*  gain 2^10 to have result in 24 bit */
		0x2000,//0x800000, /* dc_offset */
		SFRACT24(0) /* forget_factor */
	},
	{
		8, /* queue_offset */
		1, /*  gain 2^10 to have result in 24 bit */
		0x2000,//0x800000, /* dc_offset */
		SFRACT24(0) /* forget_factor */
	},
	{
		12, /* queue_offset */
		1, /*  gain 2^10 to have result in 24 bit */
		0, /* dc_offset */
		SFRACT24(0) /* forget_factor */
	}
};

/** A structure to represent a configuration of AS.
 *  It includes AS configuration items which can mostly be changed in
 *  run-time. */
struct as_config_t as_config =
{
	0, /* start_offset */
	0, /* period */
    USEC2TCR1(4),   /* pulse_width */
	FS_ETPU_AS_FM0_FRAME_PULSE_ON, /* pulse_selection */
    0, /* pulse_adjustment_frame */
    0, /* pulse_adjustment_center */
	FS_ETPU_AS_IRQ_FRAME_PULSE_END, /* irq_dma_options */
	0, /* link_options */
	0, /* link_chans_frame_start */
    0, /* link_chans_frame_end */
    0, /* link_chans_center_start */
    0, /* link_chans_center_end */
    (uint32_t *)(0xC3FC8000 + 0x1000), /* p_result_queue */
    &as_signal_config[0], /* *signal_config */
    0xF, /* signal_mask_frame */
    0x0, /* signal_mask_center */
	FS_ETPU_AS_PHASE_CURRENTS_OFF, /* phase_current_processing */
    0, /* phase_current_idx_a */
    1, /* phase_current_idx_b */
    2, /* phase_current_idx_c */
    0, /* cfifo_update */
    0, /* *p_cfifo */
    0, /* cmd_ia_adc_0 */
    0, /* cmd_ia_adc_1 */
    0, /* cmd_ib_adc_0 */
    0, /* cmd_ib_adc_1 */
    0, /* cmd_ic_adc_0 */
    0 /* cmd_ic_adc_1 */
};

/** A structure to represent outputs of AS processig. */
struct as_outputs_t as_outputs;


/**************************************************************************//*!
* @brief    my_system_etpu_init
*
* @details  This function initialize the eTPU module:
*          -# Initialize global setting using fs_etpu_init function
*             and the my_etpu_config structure
*          -# On eTPU2, initialize the additional eTPU2 setting using
*             fs_etpu2_init function
*          -# Initialize channel setting using channel function APIs
*
* @return  Zero or an error code is returned.
*******************************************************************************/
int32_t my_system_etpu_init(void)
{
  int32_t err_code;

  /* Initialization of eTPU DATA RAM */
  fs_memset32((uint32_t*)fs_etpu_data_ram_start, 0, fs_etpu_data_ram_end - fs_etpu_data_ram_start);

  /* Initialization of eTPU global settings */
  err_code = fs_etpu_init(
    my_etpu_config,
    (uint32_t *)etpu_code, sizeof(etpu_code),
    (uint32_t *)etpu_globals, sizeof(etpu_globals));
  if(err_code != 0) return(err_code);

#ifdef FS_ETPU_ARCHITECTURE
 #if FS_ETPU_ARCHITECTURE == ETPU2
  /* Initialization of additional eTPU2-only global settings */
  err_code = fs_etpu2_init(
    my_etpu_config,
  #ifdef FS_ETPU_ENGINE_MEM_SIZE
    FS_ETPU_ENGINE_MEM_SIZE);
  #else
    0);
  #endif
  if(err_code != FS_ETPU_ERROR_NONE) return(err_code);
 #endif
#endif

  /* Initialization of eTPU channel settings */
  err_code = fs_etpu_pwmm_init(&pwmm_instance, &pwmm_config);
  if(err_code != FS_ETPU_ERROR_NONE) return(err_code + (ETPU_PWMM_MASTER_CH<<16));

  err_code = fs_etpu_as_init(&as_instance, &pwmm_instance, &as_config);
  if(err_code != FS_ETPU_ERROR_NONE) return(err_code + (ETPU_AS_CH<<16));

  err_code = fs_etpu_resolver_init(&resolver_instance, &resolver_config);
  if(err_code != FS_ETPU_ERROR_NONE) return(err_code + (ETPU_RESOLVER_EXC_CHAN<<16));

  return(0);
}

/**************************************************************************//*!
* @brief    my_system_etpu_start
*
* @details  This function enables channel interrupts, DMA requests and "output
*           disable" feature on selected channels and starts TCR time bases using
*           Global Timebase Enable (GTBE) bit.
*
* @warning  This function should be called after all device modules, including
*           the interrupt and DMA controller, are configured.
*******************************************************************************/
void my_system_etpu_start(void)
{
  /* Initialization of Interrupt Enable, DMA Enable
     and Output Disable channel options */
  fs_etpu_set_interrupt_mask_a(ETPU_CIE_A);
  fs_etpu_set_interrupt_mask_b(ETPU_CIE_B);
  fs_etpu_set_dma_mask_a(ETPU_DTRE_A);
  fs_etpu_set_dma_mask_b(ETPU_DTRE_B);
  fs_etpu_set_output_disable_mask_a(ETPU_ODIS_A, ETPU_OPOL_A);
  fs_etpu_set_output_disable_mask_b(ETPU_ODIS_B, ETPU_OPOL_B);

  /* Synchronous start of all TCR time bases */
  fs_timer_start();
}

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
