/******************************************************************************
*
*   Copyright 2006-2016 Freescale Semiconductor, Inc.
*   Copyright 2017-2020 NXP
*
/****************************************************************************//*!
*
* @file     main.c
*
* @date     October-20-2020
*
* @brief    PMSM Field Oriented Control demo for MPC5775E Motor control kit
*
*******************************************************************************/

/******************************************************************************
* Included files
*******************************************************************************/
#include "derivative.h"
#include "peripherals\etpu_set\etpu\pwmm\etpu_pwmm.h"
#include "peripherals\etpu_set\etpu\resolver\etpu_resolver.h"
#include "peripherals\etpu_set\etpu\as\etpu_as.h"
#include "peripherals\etpu_set\etpu_gct.h"
#include "freemaster.h"

#include "peripherals\eqadc.h"
#include "peripherals\edma_init.h"
#include "peripherals\esci.h"
#include "peripherals\pit.h"
#include "peripherals\sd_adc.h"
#include "peripherals\siu_init.h"
#include "peripherals\dspi.h"
#include "peripherals\sys.h"

#include "mlib.h"
#include "gflib.h"
#include "gmclib.h"
#include "gdflib.h"

#include "Config\PMSM_appconfig.h"
#include "actuate_mpc57xx.h"
#include "meas_mpc57xx.h"
#include "motor_structure.h"
#include "state_machine.h"
#include "pospe_sensor.h"
#include "amclib.h"

/******************************************************************************
* Defines
******************************************************************************/
#define DEBUG_PIN_0     	    157
#define DEBUG_PIN_1     	    178
#define BTN_UP                  453
#define BTN_DOWN                454
#define APP_SWITCH              137

#define ETPU_AS_IRQ_SOURCE	    96
#define ETPU_AS_IRQ_PRIORITY	9
#define SIU_EIRQ4_15_IRQ_SOURCE	    50
#define SIU_EIRQ4_15_IRQ_PRIORITY	10
#define ESCI_A_IRQ_SOURCE	146
#define ESCI_A_IRQ_PRIORITY	10

/******************************************************************************
* Global function prototypes
******************************************************************************/
extern void xcptn_xmpl(void);
extern void mcu_mode_clock_init(void);
extern void my_interrupt_init(uint32_t source, uint8_t priority);
extern void eTPU_AS_Isr (void);
extern void SIU_EIRQ4_15_Isr (void);
extern void dspi_init(uint8_t module, uint8_t mode, uint8_t com_mode, uint8_t ctar);
extern void MC33937_config(void);
extern void esci_init(uint8_t module, uint8_t mode);
extern void SIU_init(void);
extern void SIU_PWM_pin_config(void);
extern void eqadc_init(void);
extern void eqadc_start(void);

/******************************************************************************
* Local variables
******************************************************************************/
uint32_t etpu_a_engine_load;  /* eTPU Engine A load as a percentage */
uint32_t etpu_b_engine_load;  /* eTPU Engine B load as a percentage */

int32_t ia = 0, ib = 0, ic = 0;
uint32_t dcbv = 0;
int32_t angle = 0;
int32_t speed = 0;
//MOJE PROMÌNNÉ
volatile int32_t dom_trb_counter = 0;
int dom_SDADC2_read = 0;
int dom_trb_flag = 10;
int dom_trb_res = 10; //výsledek pøevodu SDADC2
int dom_trb_res2 = 10; //výsledek pøevodu SDADC3
unsigned int dom_delay = 0;

float dcbv1 = 0;
float fltuAlBearg1 = 0;
float fltuAlBearg2 = 0;
float ia_float=0;
float ib_float=0;
float ic_float=0;
float angle_float = 0;
float speed_float = 0;
float dcbv_float =  0;


pmsmDrive_t       drvFOC;           /* Field Oriented Control Variables */
driveStates_t 	  cntrState;	    /* Responsible for stateMachine state propagation */
appFaultStatus_t  tempfaults;		/* Temporary faults to be indicated inhere */
appFaultStatus_t  permFaults;		/* Permanent faults to be indicated inhere */
fm_scale_t		  fmScale;		    /* Scales for freeMASTER interface */
measModule_t	  meas;			    /* Variables measured by ADC modules */
MC33937_T		  MC33937;		    /* ACTUATOR - 3-ph. pre-driver s/w data type */
MC33937Status_t   MC33937Status;	/* MC33937 status variables */
tBool 			  statePWM;		    /* Status of the PWM update */
tBool			  fieldWeakOnOff;	/* Enable/Disable Field Weakening */
resolverPospe_t   resolverPospe;	/* Resolver position and speed */

static volatile tU8	appRunState, appRunState_last;
static volatile tU8	btSpeedDown;

static volatile tU8	btSpeedUp;
static volatile tU8	appSwStateCnt, appRunState_Lo, appRunState_Hi;

/* Open Loop and Closed loop speed ramp variants */
volatile tFloat  	OL_SpeedRampInc = 0.0F, CL_SpeedRampInc = 0.0F, CL_SpeedRampDec = 0.0F;
volatile tFloat		FW_PropGainControl = 0.0F, FW_IntegGainControl = 0.0F;
volatile tFloat		UDQVectorSum = 0.0F;

static volatile tFrac32		fm_speed_n_m;
static volatile tFrac32		fm_position_deg;

/******************************************************************************
* Local function Prototypes
******************************************************************************/

uint32_t get_etpu_load_a(void);
uint32_t get_etpu_load_b(void);
vuint8_t GPDI_read(volatile tU16 PcrNo);
static void MCAT_Init();
static tBool FocFastLoop(void);
static tBool FocSlowLoop(void);
static tBool FaultDetection();
static tBool CalcOpenLoop(openLoopPospe_t *openLoop, tFloat speedReqRamp);


int main(void)
{
	uint32_t i,j;
	/* MCU initialization */
	mcu_mode_clock_init();
	/* Configure and Enable Interrupts */
	xcptn_xmpl();
	/* Configure custom interrupt */
	my_interrupt_init(ETPU_AS_IRQ_SOURCE,ETPU_AS_IRQ_PRIORITY);
	my_interrupt_init(SIU_EIRQ4_15_IRQ_SOURCE,SIU_EIRQ4_15_IRQ_PRIORITY);

	/* Initialize eSCI for UART */
	esci_init(ESCI_A_MODULE,MODE_SCI);
	/* SIUL2 pin configuration */
	SIU_init();
	/* FreeMASTER initialization */
	FMSTR_Init();
	/* initialize eTPU */
	my_system_etpu_init();
	/* Initialize eDMA */
	edma_init();
	/* SDADC init*/
	sdadc_init(OSR_24, GAIN_1, FTHLD_8, SINGLE);
	/* SDADC2 init*/
	SDADC2_init();
	/* EQADC init */
	eqadc_init();
	/* DSPI initialization */
	dspi_init(DSPI_B_MODULE, DSPI_MASTER, DSPI_SPI, CTAR0);
	/* MCAT variables initialization */
	MCAT_Init();
	/* Clear measured variables */
	MEAS_Clear(&meas);

	/* Application starts from init state */
	cntrState.state   	= init;
	cntrState.event 	= e_init;

	/* Default setting of a FOC Control Mode */
	cntrState.usrControl.FOCcontrolMode		= speedControl;

	/* Configure eTPU functionality on dedicated PWM output pins */
	SIU_PWM_pin_config();

	/* Run the eTPU */
	my_system_etpu_start();
	/* Start the DMA and ADCs */
	edma_start();
	eqadc_start();
	sdadc_start();

	/* wait till SW Resolver is properly started */
	while(resolver_states.state < 4){
		FMSTR_Poll();
	}

	/* Measure i_abc dc-offsets */
	for(j = 0; j < 1000; j++)
	{
	  fs_etpu_as_measure_dc_offsets(&as_instance, &as_config, 0x7);

	  for(i=0; i<1000; i++)
	  {
		FMSTR_Poll();
	  }
	}

	/* Loop forever */
	//Tahle smyèka více ménì nastavuje HW pro správnou funkci
	for(;;)
	{
		/* Resolver function calls */
		fs_etpu_resolver_get_outputs_calculated(&resolver_instance, &resolver_outputs_calculated);
		fs_etpu_resolver_get_states(&resolver_instance, &resolver_states);
		/* PWMM function calls */
		fs_etpu_pwmm_config(&pwmm_instance, &pwmm_config);
		fs_etpu_pwmm_get_states(&pwmm_instance, &pwmm_states);
		/* AS function calls */
		fs_etpu_as_config(&as_instance, &as_config);
		/* Check the eTPU engine load */
		etpu_a_engine_load = get_etpu_load_a();
		etpu_b_engine_load = get_etpu_load_b();

		/* Read MC33937 Status register 0 and Status register 1, if there is interrupt */
		//MC33937 je budiè tranzistoru s možností komunikace
		if(MC33937Status.B.MC33937IntFlag)
		{
			MC33937Status.B.MC33937IntFlag = false;
			MC33937_get_status(&MC33937);
		}

		/* Clear MC33937 Errors */
		if(MC33937Status.B.MC33937ClearErr)
		{
			MC33937Status.B.MC33937ClearErr = false;
			permFaults.mc33937 = false;
			MC33937.status.sr0.R = 0;
			MC33937_clear_faults(&MC33937);
		}
		/* FreeMASTER processing on background */
		FMSTR_Poll();
	}
}

/******************************************************************************
* Interrupt handlers
******************************************************************************/

/**************************************************************************//*!
* @brief    SIU external interrupt service routine
*
* @details  The ISR is called by an interrupt pin from MC33937.
*
* @return   N/A
*
*******************************************************************************/
//Zde pouze nastavím MC33937Status.B.MC33937IntFlag = true; abych nezdržoval hlavní program a tento problém vyøešil až na nìj bude èas
//ISR - Interrupt Service Routine
//*******************************************************************************
void SIU_EIRQ4_15_Isr(void)
{
    /* Check if IRQ12 flag has been set */
	if(SIU.EISR.B.EIF12)
	{
	    if (MC33937.status.sr0.R)//pokud 1, tak MC33937 má novou informaci k pøeètení (sr0 = stavový registr 0)
	    {
	    	MC33937Status.B.MC33937IntFlag = true;
	    }
		/* Clear the interrupt flag */
		SIU.EISR.B.EIF12 = 1;
	}
}

/**************************************************************************//*!
* @brief    eTPU AS interrupt service routine
*
* @details  The ISR is triggered by eTPU AS function and runs at control loop
*           frequency which is the same as PWM switching frequency. ISR
*           periodically performs: all ADC quantities measurement, state machine
*           calling, user button/switch control
*
* @return   N/A
*
*******************************************************************************/
void eTPU_AS_Isr(void)
{
	static tBool getFcnStatus;
	//AS_ISR_counter++; //moje promìnná, poèítající pøerušení
	/* Debug pin ON */
    //SIU.GPDO[DEBUG_PIN_0].R = 1;

    /* Read input of user buttons/switch states */
	appRunState		= GPDI_read(APP_SWITCH);
	btSpeedUp 		= GPDI_read(BTN_UP);
	btSpeedDown		= GPDI_read(BTN_DOWN);

	/* Routine for reading the application On/Off state, resistant for overshoot of the HW button */
	//^ = XOR
	if (appRunState ^ appRunState_last)
	{
		appSwStateCnt++;
		if (appRunState)
			{
				appRunState_Hi = appRunState_Hi + 1;
			}
		else
			{
				appRunState_Lo = appRunState_Lo + 1;
			}
	}
	/* If 7 Hi/Lo states is red out of 10 reading, perform the switch On/Off event */
	if (appSwStateCnt >= 10)
	{
		/* SW_APP_RUN_L is switched off - logic 1 */
		cntrState.usrControl.switchAppOnOff  = (appRunState_Hi > 6) ? FALSE: cntrState.usrControl.switchAppOnOff;
		/* SW_APP_RUN_L is switched on - logic 0 */
		cntrState.usrControl.switchAppOnOff  = (appRunState_Lo > 6) ? TRUE : cntrState.usrControl.switchAppOnOff;
		/* Re-init the flip/flop s/w count states */
		appRunState_last    = appRunState;
		appSwStateCnt       = 0;
		appRunState_Hi      = 0;
		appRunState_Lo      = 0;
	}

	/* User accessible Flip/Flop switch for starting/stopping the application. */
	if (cntrState.usrControl.switchAppOnOff ^ cntrState.usrControl.switchAppOnOffState)
	{
		cntrState.usrControl.switchAppOnOffState	= cntrState.usrControl.switchAppOnOff;

		if(cntrState.usrControl.switchAppOnOff)
		{
			cntrState.event = e_app_on;
		}
		else
		{
			cntrState.event	= e_app_off;
		}
	}

	/* Get the extrapolated values from Resolver  */
    fs_etpu_resolver_get_outputs_extrapolated(&resolver_instance, &resolver_outputs_extrapolated);
	/* Get the measured feedback values from AS  */
    fs_etpu_as_get_outputs(&as_instance, &as_outputs);

    ia = as_outputs.signal_value[0]<<8;
    ib = as_outputs.signal_value[1]<<8;
    ic = as_outputs.signal_value[2]<<8;

    dcbv = as_outputs.signal_value[3]<<8;
    angle = (resolver_outputs_extrapolated.angle)<<8;  /* *180/(0x7FFFFF) */
    speed = (resolver_outputs_calculated.speed)<<8;    /* *10000*60/3000 */

    dcbv_float  = MLIB_Mul(MLIB_ConvertPU_FLTF32(dcbv), 26000.0F);
    ia_float    = MLIB_Mul(MLIB_ConvertPU_FLTF32(ia), -13000.0F);
    ib_float    = MLIB_Mul(MLIB_ConvertPU_FLTF32(ib), -13000.0F);
    ic_float    = MLIB_Mul(MLIB_ConvertPU_FLTF32(ic), -13000.0F);
   // angle_float = MLIB_Mul(MLIB_ConvertPU_FLTF32(angle), FLOAT_PI);
   // speed_float = MLIB_Mul(MLIB_ConvertPU_FLTF32(speed), 200.0F);   // 10000*60/3000

	getFcnStatus    =    true;

	/* DCB voltage & phase currents */
	getFcnStatus  = MEAS_GetUdcVoltage(&meas, &drvFOC.uDcbFilter);
	getFcnStatus &= MEAS_Get3PhCurrent(&meas, &drvFOC.iAbcFbck);
	drvFOC.fltUdcb = meas.measured.fltUdcb.filt;

	/* Get rotor position and speed from resolver sensor */
	POSPE_GetPospeElRes(&resolverPospe);

	/* Fault detection routine, has to be executed prior application state machine */
	getFcnStatus &= FaultDetection();

	if (getFcnStatus)    cntrState.event = e_fault;

	/* Execute State table with newly measured data */
	StateTable[cntrState.event][cntrState.state]();

    FMSTR_Recorder(0);

    /* Clear interrupt flag */
    fs_etpu_clear_chan_interrupt_flag(ETPU_AS_CH);
}

/***************************************************************************//*!
*
* @brief    Evaluate eTPU engine A load.
*
* @warning  This function is applicable on eTPU2 only. The first call of this
*           function does not return a correct result. This function presumes
*           the TCR1 clock is configured for system clock div 1.
*
* @return   eTPU engine A load percentage (0 - 10000).
*
******************************************************************************/
uint32_t get_etpu_load_a(void)
{
  static uint24_t tcr1;
         uint24_t tcr1_last;
         uint24_t idle_cnt;
         uint24_t time_cnt;

  tcr1_last = tcr1;
  tcr1 = eTPU->TB1R_A.R;
  idle_cnt = fs_etpu_get_idle_cnt_a();
  fs_etpu_clear_idle_cnt_a();

  time_cnt = 0xFFFFFF & (tcr1 - tcr1_last);
  time_cnt = time_cnt/2;
  return(10000*(time_cnt - idle_cnt)/time_cnt);
}

/***************************************************************************//*!
*
* @brief    Evaluate eTPU engine B load.
*
* @warning  This function is applicable on eTPU2 only. The first call of this
*           function does not return a correct result. This function presumes
*           the TCR1 clock is configured for system clock div 1.
*
* @return   eTPU engine A load percentage (0 - 10000).
*
******************************************************************************/
uint32_t get_etpu_load_b(void)
{
  static uint24_t tcr1;
         uint24_t tcr1_last;
         uint24_t idle_cnt;
         uint24_t time_cnt;

  tcr1_last = tcr1;
  tcr1 = eTPU->TB1R_B.R;
  idle_cnt = fs_etpu_get_idle_cnt_b();
  fs_etpu_clear_idle_cnt_b();

  time_cnt = 0xFFFFFF & (tcr1 - tcr1_last);
  time_cnt = time_cnt/2;
  return(10000*(time_cnt - idle_cnt)/time_cnt);
}


/***************************************************************************//*!
*
* @brief    General Purpose Input pin status read routine
*
* @param    PcrNo - PCR number
*
* @return   status of the pin
*
******************************************************************************/
vuint8_t GPDI_read(volatile tU16 PcrNo)
{
	return(SIU.GPDI[PcrNo].B.PDIn);
}

/***************************************************************************//*!
*
* @brief    MCAT Initialization (motor control application tuning)
*
******************************************************************************/
void MCAT_Init()
{
	/* Freemaster variables */
	fmScale.speed_n_m						= FM_SPEED_RPM_MEC_SCALE;
	fmScale.position						= FM_POSITION_DEG_SCALE;

    drvFOC.alignCntr						= ALIGN_DURATION;
	drvFOC.alignVoltage						= ALIGN_VOLTAGE;

    /* Currents */
    drvFOC.iAbcFbck.fltArg1					= 0.0F;
    drvFOC.iAbcFbck.fltArg2                	= 0.0F;
    drvFOC.iAbcFbck.fltArg3                	= 0.0F;

    drvFOC.iAlBeFbck.fltArg1               	= 0.0F;
    drvFOC.iAlBeFbck.fltArg2               	= 0.0F;

    drvFOC.iDQReqInLoop.fltArg1             = 0.0F;
    drvFOC.iDQReqInLoop.fltArg2             = 0.0F;

    drvFOC.iDQReqOutLoop.fltArg1            = 0.0F;
    drvFOC.iDQReqOutLoop.fltArg2            = 0.0F;

    drvFOC.iDQFbck.fltArg1                 	= 0.0F;
    drvFOC.iDQFbck.fltArg2                	= 0.0F;

    /* Voltages */
    drvFOC.uAlBeReq.fltArg1                 = 0.0F;
    drvFOC.uAlBeReq.fltArg2                	= 0.0F;

    drvFOC.uAlBeReqDCB.fltArg1             	= 0.0F;
    drvFOC.uAlBeReqDCB.fltArg2             	= 0.0F;

    drvFOC.uDQReq.fltArg1                 	= 0.0F;
    drvFOC.uDQReq.fltArg2                  	= 0.0F;

    /* Speed/Position */
    drvFOC.thTransform.fltArg1             	= 0.0F;
    drvFOC.thTransform.fltArg2             	= 0.0F;

    /* SVC-PWM variables */
    drvFOC.svmSector                       	= 1;
    drvFOC.pwmflt.fltArg1                   = 0.5F;
    drvFOC.pwmflt.fltArg2                   = 0.5F;
    drvFOC.pwmflt.fltArg3                   = 0.5F;

    /* MC33937 SW initialization */
    MC33937.configData.deadtime                   	= 750;
    MC33937.configData.mode.B.enableFullOn        	= 0;
    MC33937.configData.mode.B.lock                	= 1;
    MC33937.configData.mode.B.disableDesat        	= 1;

    MC33937.configData.interruptEnable.R          	= 0;    /* Clear MASK0/1 registers - no interrupts enabled */
    MC33937.configData.interruptEnable.B.overCurrent= 1;    /* Enable interrupt by over-current */
    MC33937.configData.interruptEnable.B.lowVls		= 1;    /* Enable interrupt by low-VLS */
    MC33937.configData.interruptEnable.B.desaturation= 1;   /* Enable interrupt by Desaturation Detection */

    MC33937_sw_init(&MC33937);
    MC33937_get_status(&MC33937);

	/* FOC variables */

    /* D-axis PI controller */
    drvFOC.CurrentLoop.pPIrAWD.fltCC1sc                	= D_CC1SC;
    drvFOC.CurrentLoop.pPIrAWD.fltCC2sc                	= D_CC2SC;
    drvFOC.CurrentLoop.pPIrAWD.fltLowerLimit           	= -CLOOP_LIMIT;
    drvFOC.CurrentLoop.pPIrAWD.fltUpperLimit           	= CLOOP_LIMIT;

    /* Q-axis PI controller */
    drvFOC.CurrentLoop.pPIrAWQ.fltCC1sc                	= Q_CC1SC;
    drvFOC.CurrentLoop.pPIrAWQ.fltCC2sc                	= Q_CC2SC;
    drvFOC.CurrentLoop.pPIrAWQ.fltLowerLimit           	= -CLOOP_LIMIT;
    drvFOC.CurrentLoop.pPIrAWQ.fltUpperLimit           	= CLOOP_LIMIT;

    drvFOC.CurrentLoop.pIDQReq  						= &drvFOC.iDQReqInLoop;
    drvFOC.CurrentLoop.pIDQFbck 						= &drvFOC.iDQFbck;

    /* Clear AMCLIB_CurrentLoop state variables */
	AMCLIB_CurrentLoopInit_FLT(&drvFOC.CurrentLoop);

    /* DCBus 1st order filter; Fcut = 100Hz, Ts = 100e-6 */
    drvFOC.uDcbFilter.fltLambda = MLIB_Div(1.0F, 8.0F);
    GDFLIB_FilterMAInit_FLT(&drvFOC.uDcbFilter);
    drvFOC.uDcbFilter.fltAcc = 12.0F;

    drvFOC.elimDcbRip.fltModIndex          				= 0.866025403784439F;
    drvFOC.elimDcbRip.fltArgDcBusMsr       				= 0.0F;

    CL_SpeedRampInc = SPEED_RAMP_UP;
    CL_SpeedRampDec = SPEED_RAMP_DOWN;

	drvFOC.FwSpeedLoop.pRamp.fltRampUp					= OL_SpeedRampInc;
	drvFOC.FwSpeedLoop.pRamp.fltRampDown				= OL_SpeedRampInc;

	drvFOC.FwSpeedLoop.pPIpAWQ.fltPropGain				= SPEED_PI_PROP_GAIN;
	drvFOC.FwSpeedLoop.pPIpAWQ.fltIntegGain				= SPEED_PI_INTEG_GAIN;
	drvFOC.FwSpeedLoop.pPIpAWQ.fltUpperLimit			= SPEED_LOOP_HIGH_LIMIT;
	drvFOC.FwSpeedLoop.pPIpAWQ.fltLowerLimit			= SPEED_LOOP_LOW_LIMIT;
	drvFOC.FwSpeedLoop.pFilterW.fltLambda				= POSPE_SPEED_FILTER_MA_LAMBDA;

	/* Field weakening FilterMA */
	drvFOC.FwSpeedLoop.pFilterFW.fltLambda			= 1.0F;
	/* Field weakening PI controller */
	drvFOC.FwSpeedLoop.pPIpAWFW.fltPropGain			= SPEED_PI_PROP_GAIN;
	drvFOC.FwSpeedLoop.pPIpAWFW.fltIntegGain		= SPEED_PI_INTEG_GAIN;
	FW_PropGainControl								= SPEED_PI_PROP_GAIN;
	FW_IntegGainControl								= SPEED_PI_INTEG_GAIN;
	drvFOC.FwSpeedLoop.pPIpAWFW.fltUpperLimit		= 0.0F;
	drvFOC.FwSpeedLoop.pPIpAWFW.fltLowerLimit		= -FLOAT_PI_DIVBY_2;

	/* Input/output pointers */
	drvFOC.FwSpeedLoop.pIQFbck						= &(drvFOC.iDQFbck.fltArg2);
	drvFOC.FwSpeedLoop.pUQLim						= &(drvFOC.CurrentLoop.pPIrAWQ.fltUpperLimit);
	drvFOC.FwSpeedLoop.pUQReq						= &(drvFOC.uDQReq.fltArg2);
	/* Field weakening PI controller */
	drvFOC.FwSpeedLoop.fltUmaxDivImax				= MLIB_Div(U_DCB_MAX, I_MAX);
	/* Clear state variables */
	drvFOC.pospeControl.wRotElReq                   = 0;

	/* Clear AMCLIB_SpeedLoop state variables */
  	AMCLIB_FWSpeedLoopInit_FLT(&drvFOC.FwSpeedLoop);

    /* Resolver ATO observer - Controller parameters */
    resolverPospe.TrackObsrv.pParamPI.fltCC1sc					= POSPE_RES_TO_CC1;
    resolverPospe.TrackObsrv.pParamPI.fltCC2sc					= POSPE_RES_TO_CC2;
    resolverPospe.TrackObsrv.pParamPI.fltUpperLimit				= FLOAT_MAX;
    resolverPospe.TrackObsrv.pParamPI.fltLowerLimit				= FLOAT_MIN;
    /* resolver observer - Integrator parameters */
    resolverPospe.TrackObsrv.pParamInteg.fltC1					= POSPE_RES_TO_INTEG_GAIN;

    /* Clear ATO observer state variables */
    AMCLIB_TrackObsrvInit_FLT(&resolverPospe.TrackObsrv);


    drvFOC.pospeOpenLoop.integ.f32InK1 		= 0;
    drvFOC.pospeOpenLoop.integ.f32State 	= 0;
    drvFOC.pospeOpenLoop.integ.f32C1 		= SCALAR_INTEG_GAIN;
    drvFOC.pospeOpenLoop.integ.u16NShift	= SCALAR_INTEG_SHIFT;

    drvFOC.pospeOpenLoop.thRotEl			= 0.0F;
    drvFOC.pospeOpenLoop.wRotEl				= 0.0F;

    drvFOC.pospeOpenLoop.iQLowerLimit		= MLIB_Neg(drvFOC.pospeOpenLoop.iQUpperLimit);
}

/***************************************************************************//*!
*
* @brief    FAULT state
*
******************************************************************************/
void StateFault()
{
    /* Application State Machine - state identification  */
    /* Entering state fault */
	cntrState.state   = fault;
	cntrState.event   = e_fault;

	/* Turn off PWM and disable output of MC33937 driver */
    //MC33937_disable_output(&MC33937);
	statePWM = ACTUATE_DisableOutput();

	/* Indicate State Machine fault state invoked by irrelevant event call */
	if((permFaults.mcu.R == 0)&&(permFaults.motor.R == 0)&&(permFaults.stateMachine.R == 0)&&(permFaults.mc33937 == 0))
		permFaults.stateMachine.B.FOCError 		= 1;

    /* Disable user application switch */
    cntrState.usrControl.switchAppOnOff			= false;
    cntrState.usrControl.switchAppOnOffState	= false;

    /* Clear manually Fault state by pushing both user buttons on power stage board */
    if (!btSpeedUp && !btSpeedDown)
    	cntrState.usrControl.switchFaultClear = 1;

    if (cntrState.usrControl.switchFaultClear)
    {
		/* Clear permanent and temporary SW faults */
		permFaults.mcu.R          		= 0;    		/* Clear mcu faults */
		permFaults.motor.R		  		= 0;	  		/* Clear motor faults */
		permFaults.stateMachine.R 		= 0;	  		/* Clear state machine faults */
		MC33937Status.B.MC33937ClearErr	= true;			/* Clear MC33937 faults */

		/* When all Faults cleared prepare for transition to next state */
		cntrState.usrControl.readFault             = true;
		cntrState.usrControl.switchFaultClear      = false;
		cntrState.event                            = e_fault_clear;
    }
    //dom_trb_counter++;
    //dom_AN29_read = read_an29_voltage_raw();
}

/***************************************************************************//*!
*
* @brief    INIT state
*
******************************************************************************/
void StateInit()
{
    /* Application State Machine - state identification */
    tBool InitFcnStatus;
    cntrState.state                             	= init;
    cntrState.event								 	= e_init;

    /* Turn off PWM output */
    statePWM = ACTUATE_DisableOutput();
    //MC33937_disable_output(&MC33937);

    /*  General use variables */
    InitFcnStatus                                	= false;

    /* Application state machine variables */
    /* Reset state of all user control variables */
    cntrState.usrControl.switchAppOnOff           	= false;
    cntrState.usrControl.switchAppOnOffState      	= false;
    cntrState.usrControl.switchFaultClear          	= false;
    cntrState.usrControl.switchAppReset            	= false;

    cntrState.usrControl.ledCounter					= 0;
    cntrState.usrControl.ledFlashing				= 1250;


    drvFOC.pospeControl.speedLoopCntr               = 0;

    drvFOC.alignCntr								= ALIGN_DURATION;

    InitFcnStatus = MEAS_Clear(&meas);

    /* Currents */
    drvFOC.iAbcFbck.fltArg1			= 0.0F;
    drvFOC.iAbcFbck.fltArg2			= 0.0F;
    drvFOC.iAbcFbck.fltArg3			= 0.0F;

    drvFOC.iAlBeFbck.fltArg1		= 0.0F;
    drvFOC.iAlBeFbck.fltArg2		= 0.0F;

    drvFOC.iDQReqInLoop.fltArg1		= 0.0F;
    drvFOC.iDQReqInLoop.fltArg2		= 0.0F;

    drvFOC.iDQReqOutLoop.fltArg1	= 0.0F;
    drvFOC.iDQReqOutLoop.fltArg2	= 0.0F;


    drvFOC.iDQFbck.fltArg1			= 0.0F;
    drvFOC.iDQFbck.fltArg2			= 0.0F;

    /* Voltages */
    drvFOC.uAlBeReq.fltArg1			= 0.0F;
    drvFOC.uAlBeReq.fltArg2			= 0.0F;

    drvFOC.uAlBeReqDCB.fltArg1		= 0.0F;
    drvFOC.uAlBeReqDCB.fltArg2		= 0.0F;

    drvFOC.uDQReq.fltArg1			= 0.0F;
    drvFOC.uDQReq.fltArg2			= 0.0F;

    /* Speed/Position */
    drvFOC.thTransform.fltArg1		= 0.0F;
    drvFOC.thTransform.fltArg2		= 0.0F;

    /* SVC-PWM variables */
    drvFOC.svmSector				= 1.0F;

    drvFOC.pwmflt.fltArg1			= 0.0F;
    drvFOC.pwmflt.fltArg2			= 0.0F;
    drvFOC.pwmflt.fltArg3			= 0.0F;

    /* FOC variables re-init */

    /* D-axis PI controller */
    drvFOC.CurrentLoop.pPIrAWD.fltCC1sc                		= D_CC1SC;
    drvFOC.CurrentLoop.pPIrAWD.fltCC2sc                		= D_CC2SC;
    drvFOC.CurrentLoop.pPIrAWD.fltLowerLimit           		= -CLOOP_LIMIT;
    drvFOC.CurrentLoop.pPIrAWD.fltUpperLimit           		= CLOOP_LIMIT;

    /* Q-axis PI controller */
    drvFOC.CurrentLoop.pPIrAWQ.fltCC1sc                		= Q_CC1SC;
    drvFOC.CurrentLoop.pPIrAWQ.fltCC2sc                		= Q_CC2SC;
    drvFOC.CurrentLoop.pPIrAWQ.fltLowerLimit           		= -CLOOP_LIMIT;
    drvFOC.CurrentLoop.pPIrAWQ.fltUpperLimit           		= CLOOP_LIMIT;

    drvFOC.CurrentLoop.pIDQReq  							= &drvFOC.iDQReqInLoop;
    drvFOC.CurrentLoop.pIDQFbck 							= &drvFOC.iDQFbck;

    /* Clear AMCLIB_CurrentLoop state variables */
	AMCLIB_CurrentLoopInit_FLT(&drvFOC.CurrentLoop);

    /* DCBus 1st order filter; Fcut = 100Hz, Ts = 100e-6 */
    drvFOC.uDcbFilter.fltLambda = MLIB_Div(1.0F, 8.0F);
    GDFLIB_FilterMAInit_FLT(&drvFOC.uDcbFilter);
    drvFOC.uDcbFilter.fltAcc = 12.0F;

    drvFOC.elimDcbRip.fltModIndex          					= 0.866025403784439F;
    drvFOC.elimDcbRip.fltArgDcBusMsr       					= 0.0F;
    drvFOC.FwSpeedLoop.pRamp.fltRampUp	   					= OL_SpeedRampInc;
    drvFOC.FwSpeedLoop.pRamp.fltRampDown	   				= OL_SpeedRampInc;
    drvFOC.FwSpeedLoop.pFilterW.fltLambda	   				= POSPE_SPEED_FILTER_MA_LAMBDA;
    fieldWeakOnOff = false;

    AMCLIB_FWSpeedLoopInit_FLT(&drvFOC.FwSpeedLoop);

    drvFOC.pospeControl.wRotEl			   					= 0.0F;

    if (!InitFcnStatus)
    {
    	/* Fault in initialization state */
        tempfaults.stateMachine.B.InitError = 1;		/* Mark the initialization fault */
        cntrState.event                     = e_fault;	/* Prepare for transition to fault state */
    }
    else
    {
    	/* Initialization phase successfully done */
    	cntrState.event   = e_init_done;  				/* Prepare for transition to next state */
    }
}

/***************************************************************************//*!
*
* @brief    READY state
*
******************************************************************************/
void StateReady()
{
    /* Application State Machine - state identification */

	/* Turn off PWM output */
	statePWM = ACTUATE_DisableOutput();
    //MC33937_disable_output(&MC33937);

	if(cntrState.loadDefSetting)
	{
		cntrState.loadDefSetting = false;
		MCAT_Init();
	}


	drvFOC.pospeControl.wRotElReq = 0;
	cntrState.state   = ready;
    cntrState.event   = e_ready;
    //dom_trb_counter++;
    //dom_SDADC2_read = SDADC2_torque();	//ètení napìtí na pinu AN5 v èíselné formì
    dom_trb_flag = SDADC_2.SFR.B.DFEF; 		//flag FIFO (1 = prázdný FIFO)						//pøiètení do delay promìnné
    if(!dom_trb_flag){
    	dom_trb_res = SDADC_2.CDR.B.CDATA; //ètu výslkedek z SDADC2
    	//dom_trb_res2 = SDADC_3.CDR.B.CDATA;
    	dom_trb_counter++;					//pøiètení promìnné kterou sleduji, jestli se program vnoøil
    	SDADC_2.RKR.B.RESET_KEY = 0x5AF0; 	//reset SDADC
    	//SDADC_3.RKR.B.RESET_KEY = 0x5AF0; 	//reset SDADC
    }
}

/***************************************************************************//*!
*
* @brief    CALIBRATION state - ADC calibration state
*
******************************************************************************/
void StateCalib()
{
    /* Application State Machine - state identification */
	    cntrState.state    = calib;
		statePWM = ACTUATE_EnableOutput();
		cntrState.event = e_calib_done;
}

/***************************************************************************//*!
*
* @brief    ALIGNMENT state - motor control d-axes alignment
*
******************************************************************************/
void StateAlign( )
{
    /* Application State Machine - state identification */
    tBool           AlignStatus;

    cntrState.state		= align;
    cntrState.event		= e_align;

    /* Align sequence is at the beginning */
    AlignStatus			= true;

    drvFOC.uDQReq.fltArg1      	= drvFOC.alignVoltage;
    drvFOC.uDQReq.fltArg2      	= 0.0F;

	GFLIB_SinCos_FLT(0.0F, &drvFOC.thTransform, GFLIB_SINCOS_DEFAULT_FLT);

    GMCLIB_ParkInv_FLT(&(drvFOC.uAlBeReq),&(drvFOC.thTransform),&(drvFOC.uDQReq));

    /* When alignment time elapse */
    if (--(drvFOC.alignCntr)<=0)
    {
    	drvFOC.CurrentLoop.pIDQReq->fltArg1 	= 0.0F;
        drvFOC.CurrentLoop.pIDQReq->fltArg2 	= 0.0F;

        AMCLIB_CurrentLoopInit_FLT(&drvFOC.CurrentLoop);

        drvFOC.uDQReq.fltArg1 					= 0.0F;
        drvFOC.uDQReq.fltArg2 					= 0.0F;

        drvFOC.CurrentLoop.pPIrAWD.fltInErrK1  	= 0.0F;
        drvFOC.CurrentLoop.pPIrAWD.fltAcc      	= 0.0F;

        drvFOC.CurrentLoop.pPIrAWQ.fltInErrK1  	= 0.0F;
        drvFOC.CurrentLoop.pPIrAWQ.fltAcc      	= 0.0F;

        /* Clear resolver position and speed variables */
        POSPE_ClearPospeElRes(&resolverPospe);

        resolverPospe.offset = angle;


        if (!AlignStatus)
        {
        	tempfaults.stateMachine.B.AlignError = 1;
        }
        else
        {
        	cntrState.event           = e_align_done;
        }
    }

    drvFOC.elimDcbRip.fltArgDcBusMsr = meas.measured.fltUdcb.raw;
    GMCLIB_ElimDcBusRip_FLT(&drvFOC.uAlBeReqDCB,&drvFOC.uAlBeReq,&drvFOC.elimDcbRip);

    drvFOC.svmSector = GMCLIB_SvmStd_FLT(&(drvFOC.pwmflt),&drvFOC.uAlBeReqDCB);

   statePWM = ACTUATE_SetDutycycle(&drvFOC.pwmflt);
}



/***************************************************************************//*!
*
* @brief    RUN state
*
******************************************************************************/
void StateRun( )
{
	static tBool stateRunStatus;
    stateRunStatus = false;
    /* Application State Machine - state identification */
    cntrState.state   = run;
    cntrState.event   = e_run;

    //MOJE
    //drvFOC.pospeControl.wRotElReq = -3;//rad/min
    //resolverPospe.thRotMec < 0 &&
    /*if(resolver_outputs_extrapolated.rev_counter >= 10 && !dom_trb_counter){
    	if(cntrState.usrControl.FOCcontrolMode != currentControl){
    		cntrState.usrControl.FOCcontrolMode = currentControl;
    	}
    	drvFOC.iDQReqInLoop.fltArg2 = 0;
    	if(modeSwitchDelay == 0){
        	cntrState.usrControl.FOCcontrolMode = speedControl;
        	drvFOC.pospeControl.wRotElReq = 0;
        	drvFOC.iDQReqInLoop.fltArg2 = 2;
        	dom_trb_counter = 1;
    	}
    }
    else if(!btSpeedUp && dom_trb_counter){
    	drvFOC.pospeControl.wRotElReq = -50;
    	if(resolver_outputs_extrapolated.rev_counter <= 0){
    		drvFOC.pospeControl.wRotElReq = 0;
    	}
    	if(!btSpeedUp){
    		dom_trb_counter = 0;
    		cntrState.usrControl.FOCcontrolMode = currentControl;
    		drvFOC.iDQReqInLoop.fltArg2 = 0.5;
    	}
    }
    if (modeSwitchDelay > 0) {//delay counter
        modeSwitchDelay--;
    }*/


	/*if (cntrState.usrControl.FOCcontrolMode == scalarControl)
	{
		if (!btSpeedUp)
		{
			drvFOC.pospeControl.wRotElReq = (drvFOC.pospeControl.wRotElReq + 0.05F);
		}
		if (!btSpeedDown)
		{
			drvFOC.pospeControl.wRotElReq = (drvFOC.pospeControl.wRotElReq - 0.05F);
		}
	}
	else
	{ 	/*  Manual speed control using Up/Down switch buttons SW2/SW3 *//*
		if (!btSpeedUp)
		{
			drvFOC.pospeControl.wRotElReq = (drvFOC.pospeControl.wRotElReq + 0.05F);
		}
		if (!btSpeedDown)
		{
			drvFOC.pospeControl.wRotElReq = (drvFOC.pospeControl.wRotElReq - 0.05F);
		}
	}*/

    /* Calculate positions */
	CalcOpenLoop(&drvFOC.pospeOpenLoop,drvFOC.FwSpeedLoop.pRamp.fltState);

	/* Get positions according to selected mode */
	drvFOC.pospeControl.thRotEl 				= resolverPospe.thRotEl.filt;
	drvFOC.pospeControl.wRotEl					= resolverPospe.wRotEl.raw;

	drvFOC.FwSpeedLoop.pRamp.fltRampDown		= CL_SpeedRampDec;
	drvFOC.FwSpeedLoop.pRamp.fltRampUp			= CL_SpeedRampInc;

	if (cntrState.usrControl.FOCcontrolMode != scalarControl)
	{
		drvFOC.pospeOpenLoop.integ.f32State 	= MLIB_ConvertPU_F32FLT(MLIB_Div(resolverPospe.thRotEl.filt, FLOAT_PI));
	}

	/* Calculate Field Oriented Control FOC */
    if (++drvFOC.pospeControl.speedLoopCntr>=SPEED_LOOP_CNTR)
    {
        drvFOC.pospeControl.speedLoopCntr    	= 0;
        stateRunStatus  = FocSlowLoop();
        if (!stateRunStatus)
        {
        	tempfaults.stateMachine.B.RunError 	= 1;
        }
    }

    stateRunStatus = FocFastLoop();

    if (!stateRunStatus)
    {
    	tempfaults.stateMachine.B.RunError 		= 1;
    }

    /* Voltage vector sum calculation to check if DC bus voltage is used appropriately */
    UDQVectorSum = GFLIB_Sqrt_FLT(MLIB_Add(MLIB_Mul(drvFOC.uDQReq.fltArg1,drvFOC.uDQReq.fltArg1),MLIB_Mul(drvFOC.uDQReq.fltArg2,drvFOC.uDQReq.fltArg2)));
    statePWM = ACTUATE_SetDutycycle(&drvFOC.pwmflt);
}

/***************************************************************************//*!
*
* @brief    Field Oriented Control - slow loop calculations
*
******************************************************************************/
static tBool FocSlowLoop()
{

	 //SIU.GPDO[DEBUG_PIN_0].R = 1; /* Debug */

	if (cntrState.usrControl.FOCcontrolMode != speedControl)
	{
		if (cntrState.usrControl.FOCcontrolMode == voltageControl && drvFOC.uDQReq.fltArg2==0.0F)
		{
			drvFOC.pospeControl.wRotElReq = 0.0F;
		}
		else if (cntrState.usrControl.FOCcontrolMode == currentControl && drvFOC.CurrentLoop.pIDQReq->fltArg2==0.0F)
		{
			drvFOC.pospeControl.wRotElReq = 0.0F;
		}
		else if (cntrState.usrControl.FOCcontrolMode != scalarControl)
		{
			drvFOC.pospeControl.wRotElReq = MLIB_Mul_FLT(0.75F,WEL_MAX);
		}
	}
	else
	{
		if ((!fieldWeakOnOff) && (drvFOC.pospeControl.wRotElReq >= MLIB_Mul_FLT(0.75F,WEL_MAX)))
		{
			drvFOC.pospeControl.wRotElReq = MLIB_Mul_FLT(0.75F,WEL_MAX);
		}
		else if ((!fieldWeakOnOff) && (drvFOC.pospeControl.wRotElReq <= MLIB_Mul_FLT(-0.75F,WEL_MAX)))
		{
			drvFOC.pospeControl.wRotElReq = MLIB_Mul_FLT(-0.75F,WEL_MAX);
		}
	}

	if (fieldWeakOnOff)
	{
		drvFOC.FwSpeedLoop.pPIpAWFW.fltPropGain			= FW_PropGainControl;
		drvFOC.FwSpeedLoop.pPIpAWFW.fltIntegGain		= FW_IntegGainControl;
	}
	else
	{
		// ToDo field weakening
		drvFOC.FwSpeedLoop.pPIpAWFW.fltPropGain			= 0.0F;
		drvFOC.FwSpeedLoop.pPIpAWFW.fltIntegGain		= 0.0F;
		drvFOC.FwSpeedLoop.pPIpAWFW.fltIntegPartK_1 	= 0.0F;
	}

   	AMCLIB_FWSpeedLoop_FLT(drvFOC.pospeControl.wRotElReq, drvFOC.pospeControl.wRotEl, &drvFOC.iDQReqOutLoop, &drvFOC.FwSpeedLoop);

    /* Speed FO control mode */
    if(cntrState.usrControl.FOCcontrolMode == speedControl)
    {
    	/* In Speed control mode, FOC Outer Loop (Speed Loop & Field Weakening) output is interconnected with FOC Inner Loop (Current Loop) input */
    	drvFOC.iDQReqInLoop.fltArg1 = drvFOC.iDQReqOutLoop.fltArg1;
    	drvFOC.iDQReqInLoop.fltArg2 = drvFOC.iDQReqOutLoop.fltArg2;
    }
     //SIU.GPDO[DEBUG_PIN_0].R = 0; // Debug
    return true;
}

/***************************************************************************//*!
*
* @brief    Field Oriented Control - fast loop calculations
*
******************************************************************************/
static tBool FocFastLoop()
{
	// SIU.GPDO[DEBUG_PIN_0].R = 1; /* debug pin */

	GMCLIB_Clark_FLT(&drvFOC.iAlBeFbck,&drvFOC.iAbcFbck);

	/* Scalar control mode */
	if(cntrState.usrControl.FOCcontrolMode == scalarControl)
	{
		/* Generated electrical position for scalar control purpose */

		/* Required voltage = VHzRatio * Required Frequency */
		drvFOC.scalarControl.UmReq	  	= MLIB_Mul(drvFOC.scalarControl.VHzRatioReq, drvFOC.pospeControl.wRotElReq);

		/* thRotEl is calculated in CalcOpenLoop executed in focSlowLoop */
		GFLIB_SinCos_FLT(drvFOC.pospeOpenLoop.thRotEl, &drvFOC.thTransform, GFLIB_SINCOS_DEFAULT_FLT);
		drvFOC.uDQReq.fltArg1          = 0.0F;
		drvFOC.uDQReq.fltArg2          = drvFOC.scalarControl.UmReq;
	}

	/* DQ Voltage FO control mode */
	if(cntrState.usrControl.FOCcontrolMode == voltageControl)
	{
		if(drvFOC.uDQReq.fltArg2!=0.0F)
		{
			GFLIB_SinCos_FLT(drvFOC.pospeControl.thRotEl, &drvFOC.thTransform, GFLIB_SINCOS_DEFAULT_FLT);
		}
		else
		{
			GFLIB_SinCos_FLT(0.0F, &drvFOC.thTransform, GFLIB_SINCOS_DEFAULT_FLT);
		}

		GMCLIB_Park_FLT(&drvFOC.iDQFbck,&drvFOC.thTransform,&drvFOC.iAlBeFbck);
	}

	/* DQ Current and Speed FO control mode */
	if (cntrState.usrControl.FOCcontrolMode == currentControl || cntrState.usrControl.FOCcontrolMode == speedControl)
	{
		if (cntrState.usrControl.FOCcontrolMode == speedControl)
		{
			GFLIB_SinCos_FLT(drvFOC.pospeControl.thRotEl, &drvFOC.thTransform, GFLIB_SINCOS_DEFAULT_FLT);
		}
		else
		{
			if (drvFOC.iDQReqOutLoop.fltArg2!=0.0F)
			{
				GFLIB_SinCos_FLT(drvFOC.pospeControl.thRotEl, &drvFOC.thTransform, GFLIB_SINCOS_DEFAULT_FLT);
			}
			else
			{
				GFLIB_SinCos_FLT(0.0F, &drvFOC.thTransform, GFLIB_SINCOS_DEFAULT_FLT);
			}
		}

		GMCLIB_Park_FLT(&drvFOC.iDQFbck,&drvFOC.thTransform,&drvFOC.iAlBeFbck);

		/* 90% of available DCbus recalculated to phase voltage = 0.90*uDCB/sqrt(3) */
		AMCLIB_CurrentLoop_FLT(drvFOC.fltUdcb, &drvFOC.uDQReq, &drvFOC.CurrentLoop);
	}

    GMCLIB_ParkInv_FLT(&drvFOC.uAlBeReq,&drvFOC.thTransform,&drvFOC.uDQReq);

    drvFOC.elimDcbRip.fltArgDcBusMsr = meas.measured.fltUdcb.raw;

    GMCLIB_ElimDcBusRip_FLT(&drvFOC.uAlBeReqDCB,&drvFOC.uAlBeReq,&drvFOC.elimDcbRip);

    drvFOC.AlBeReqDCBLim.fltLimit = 0.9F;

    GFLIB_VectorLimit_FLT(&drvFOC.uAlBeReqDCBLim,&drvFOC.uAlBeReqDCB,&drvFOC.AlBeReqDCBLim);

    drvFOC.svmSector = GMCLIB_SvmStd_FLT(&(drvFOC.pwmflt),&drvFOC.uAlBeReqDCB);

    /* Debug */
    // SIU.GPDO[DEBUG_PIN_0].R = 0;
    return (true);
}
/***************************************************************************//*!
*
* @brief    Fault Detection function
*
******************************************************************************/
static tBool FaultDetection()
{
    tBool faultDetectiontEvent;

    faultDetectiontEvent = false;

    /* Actual Faults */
	/* TRIP:   Phase A over-current detected */
	tempfaults.motor.B.OverPhaseACurrent = (drvFOC.iAbcFbck.fltArg1 > MLIB_Mul(I_PH_OVER, 0.9F)) ? true : false;

	/* TRIP:   Phase B over-current detected */
	tempfaults.motor.B.OverPhaseBCurrent = (drvFOC.iAbcFbck.fltArg2 > MLIB_Mul(I_PH_OVER, 0.9F)) ? true : false;

	/* TRIP:   Phase C over-current detected */
	tempfaults.motor.B.OverPhaseCCurrent = (drvFOC.iAbcFbck.fltArg3 > MLIB_Mul(I_PH_OVER, 0.9F)) ? true : false;

	/* TRIP:   DC-bus over-voltage */
	tempfaults.motor.B.OverDCBusVoltage  = (meas.measured.fltUdcb.raw > U_DCB_TRIP) ? true : false;

	/* TRIP:   DC-bus under-voltage */
	tempfaults.motor.B.UnderDCBusVoltage = (meas.measured.fltUdcb.raw < MLIB_Div(U_DCB_UNDER,0.91F)) ? true : false;

	/* Activate braking resistor, if there is DC-bus over-voltage */

	if(tempfaults.motor.B.OverDCBusVoltage)
	{
		/* Activate braking resistor */
		SIU.PGPDO[129].R = 1;
	}
	else
	{
		/* Deactivate braking resistor */
		SIU.PGPDO[129].R = 0;
	}


	/* Pending Faults */
	if (cntrState.state != fault)
	{
		/* Fault:   Phase A over-current detected */
		permFaults.motor.B.OverPhaseACurrent   = (drvFOC.iAbcFbck.fltArg1 > I_PH_OVER) ? true : permFaults.motor.B.OverPhaseACurrent;

		/* Fault:   Phase B over-current detected */
		permFaults.motor.B.OverPhaseBCurrent   = (drvFOC.iAbcFbck.fltArg2 > I_PH_OVER) ? true : permFaults.motor.B.OverPhaseBCurrent;

		/* Fault:   Phase C over-current detected */
		permFaults.motor.B.OverPhaseCCurrent   = (drvFOC.iAbcFbck.fltArg3 > I_PH_OVER) ? true : permFaults.motor.B.OverPhaseCCurrent;

		/* Fault:   DC-bus over-voltage */
		permFaults.motor.B.OverDCBusVoltage    = (meas.measured.fltUdcb.raw > U_DCB_OVER) ? true : permFaults.motor.B.OverDCBusVoltage;

		/* Fault:   DC-bus under-voltage */
		permFaults.motor.B.UnderDCBusVoltage   = (meas.measured.fltUdcb.raw < U_DCB_UNDER) ? true : permFaults.motor.B.UnderDCBusVoltage;
	}

	/* Check the status of the MC33937 pre-driver */
    if (MC33937.status.sr0.R)
    {
    	permFaults.mc33937 = true;
    	faultDetectiontEvent = true;
    }
    if ((permFaults.motor.R != 0x0))
    	faultDetectiontEvent 	= true;
    if ((permFaults.mcu.R != 0x0))
        faultDetectiontEvent 	= true;
    if ((permFaults.stateMachine.R != 0x0))
        faultDetectiontEvent 	= true;

    return faultDetectiontEvent;
}


/***************************************************************************//*!
*
* @brief    Open loop generator of open loop position and speed
*
* @param    pointer to structure of openLoopPospe_t type
* @param    speed ramp
*
* @return   none
*
******************************************************************************/
static tBool CalcOpenLoop(openLoopPospe_t *openLoop, tFloat speedReqRamp)
{
	openLoop->wRotEl	= speedReqRamp;
	openLoop->thRotEl 	= MLIB_Mul(MLIB_ConvertPU_FLTF32(GFLIB_IntegratorTR_F32(MLIB_ConvertPU_F32FLT(MLIB_Div(speedReqRamp, WEL_MAX)),
								 &(openLoop->integ))), FLOAT_PI);

	return(true);
}

