/******************************************************************************
*
*   Copyright 2018,2020 NXP
*
****************************************************************************//*!
* @file     PMSM_appfremmaster_TSA.h
*
* @date     October-20-2020
*
* @brief    TSA table for PMSM demo board
*
*******************************************************************************/

#ifndef _PMSM_APPFREEMASTER_TSA_H_
#define _PMSM_APPFREEMASTER_TSA_H_

#include "motor_structure.h"

/******************************************************************************
* Global Variables
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

/* PMSM Control Variable */
extern 		pmsmDrive_t 					drvFOC;
extern  	driveStates_t					cntrState;
extern  	appFaultStatus_t				tempfaults;
extern  	appFaultStatus_t				permFaults;
extern 		fm_scale_t 						fmScale;
extern  	measModule_t					meas;
extern      MC33937_T		                MC33937;
extern  	MC33937Status_t   				MC33937Status;
extern      tBool 			                statePWM;
extern      tBool			                fieldWeakOnOff;
extern 		resolverPospe_t					resolverPospe;

extern 		volatile tFloat					OL_SpeedRampInc, CL_SpeedRampInc, CL_SpeedRampDec;

extern		volatile tFloat					UDQVectorSum;
extern		volatile tFloat					FW_PropGainControl;
extern		volatile tFloat					FW_IntegGainControl;
extern 		tFloat 							minZeroPulseCnt;
extern 		tFloat 							minSamplingPulseCnt;
extern 		tU16							adcRawResultArray[];
extern		tU16 							pdbTriggerOffset;

extern		SWLIBS_3Syst_U16 				pwmDutyCnt;
extern		SWLIBS_3Syst_U16 				pwmCenterPulseHalfWidthCnt;
extern		tU16							pdbPretrigDelay[];

/******************************************************************************
* begin TSA table - MPC5775E_PMSM
******************************************************************************/
FMSTR_TSA_TABLE_BEGIN(MPC5775E_PMSM)
	FMSTR_TSA_RW_VAR(drvFOC, FMSTR_TSA_USERTYPE(struct pmsmDrive_t))
	FMSTR_TSA_RW_VAR(cntrState, FMSTR_TSA_USERTYPE(struct driveStates_t))
	FMSTR_TSA_RW_VAR(tempfaults, FMSTR_TSA_USERTYPE(struct appFaultStatus_t))
	FMSTR_TSA_RW_VAR(permFaults, FMSTR_TSA_USERTYPE(struct appFaultStatus_t))
	FMSTR_TSA_RW_VAR(fmScale, FMSTR_TSA_USERTYPE(struct fm_scale_t))
	FMSTR_TSA_RW_VAR(meas, FMSTR_TSA_USERTYPE(struct measModule_t))
	FMSTR_TSA_RW_VAR(MC33937, FMSTR_TSA_USERTYPE(struct MC33937_T))
	FMSTR_TSA_RW_VAR(MC33937Status, FMSTR_TSA_USERTYPE(struct MC33937Status_t))
	FMSTR_TSA_RW_VAR(statePWM, FMSTR_TSA_USERTYPE(struct tBool))
	FMSTR_TSA_RW_VAR(fieldWeakOnOff, FMSTR_TSA_USERTYPE(struct tBool))
	FMSTR_TSA_RW_VAR(resolverPospe, FMSTR_TSA_USERTYPE(struct resolverPospe_t))
    FMSTR_TSA_RO_VAR(OL_SpeedRampInc, FMSTR_TSA_FLOAT)
    FMSTR_TSA_RO_VAR(CL_SpeedRampInc, FMSTR_TSA_FLOAT)
    FMSTR_TSA_RO_VAR(CL_SpeedRampDec, FMSTR_TSA_FLOAT)
    FMSTR_TSA_RO_VAR(UDQVectorSum, FMSTR_TSA_FLOAT)
    FMSTR_TSA_RO_VAR(FW_PropGainControl, FMSTR_TSA_FLOAT)
    FMSTR_TSA_RO_VAR(FW_IntegGainControl, FMSTR_TSA_FLOAT)
    FMSTR_TSA_RO_VAR(minZeroPulseCnt, FMSTR_TSA_FLOAT)
    FMSTR_TSA_RO_VAR(adcRawResultArray[], FMSTR_TSA_UINT16)
    FMSTR_TSA_RO_VAR(pdbTriggerOffset, FMSTR_TSA_UINT16)
    FMSTR_TSA_RO_VAR(pdbPretrigDelay[], FMSTR_TSA_UINT16)
	FMSTR_TSA_RW_VAR(pwmDutyCnt, FMSTR_TSA_USERTYPE(struct SWLIBS_3Syst_U16))
	FMSTR_TSA_RW_VAR(pwmCenterPulseHalfWidthCnt, FMSTR_TSA_USERTYPE(struct SWLIBS_3Syst_U16))
FMSTR_TSA_TABLE_END()

FMSTR_TSA_TABLE_BEGIN(fmstr_tsa_table_etpu_load)
    FMSTR_TSA_RO_VAR(etpu_a_engine_load, FMSTR_TSA_UINT32)
    FMSTR_TSA_RO_VAR(etpu_b_engine_load, FMSTR_TSA_UINT32)
FMSTR_TSA_TABLE_END()

FMSTR_TSA_TABLE_BEGIN(fmstr_tsa_table_etpu_pwmm)
	FMSTR_TSA_RW_VAR(fmstr_etpu_start, FMSTR_TSA_UINT8)
    FMSTR_TSA_RW_VAR(pwmm_enable_disable, FMSTR_TSA_UINT8)
    FMSTR_TSA_RW_VAR(pwmm_config, FMSTR_TSA_USERTYPE(struct pwmm_config_t))
    FMSTR_TSA_RW_VAR(pwmm_inputs, FMSTR_TSA_USERTYPE(struct pwmm_inputs_t))
    FMSTR_TSA_RO_VAR(pwmm_states, FMSTR_TSA_USERTYPE(struct pwmm_states_t))

    FMSTR_TSA_STRUCT(struct pwmm_config_t)
    FMSTR_TSA_MEMBER(struct pwmm_config_t, update_option, FMSTR_TSA_UINT8)
    FMSTR_TSA_MEMBER(struct pwmm_config_t, modulation, FMSTR_TSA_UINT8)
    FMSTR_TSA_MEMBER(struct pwmm_config_t, mode, FMSTR_TSA_UINT8)
    FMSTR_TSA_MEMBER(struct pwmm_config_t, period, FMSTR_TSA_UINT32)
    FMSTR_TSA_MEMBER(struct pwmm_config_t, dead_time, FMSTR_TSA_UINT32)
    FMSTR_TSA_MEMBER(struct pwmm_config_t, min_pulse_width, FMSTR_TSA_UINT32)
    FMSTR_TSA_MEMBER(struct pwmm_config_t, update_time, FMSTR_TSA_UINT32)

    FMSTR_TSA_STRUCT(struct pwmm_inputs_t)
    FMSTR_TSA_MEMBER(struct pwmm_inputs_t, input_a, FMSTR_TSA_UINT32)
    FMSTR_TSA_MEMBER(struct pwmm_inputs_t, input_b, FMSTR_TSA_UINT32)
    FMSTR_TSA_MEMBER(struct pwmm_inputs_t, input_c, FMSTR_TSA_UINT32)

    FMSTR_TSA_STRUCT(struct pwmm_states_t)
    FMSTR_TSA_MEMBER(struct pwmm_states_t, sector, FMSTR_TSA_UINT8)
    FMSTR_TSA_MEMBER(struct pwmm_states_t, state, FMSTR_TSA_UINT8)
    FMSTR_TSA_MEMBER(struct pwmm_states_t, update, FMSTR_TSA_UINT8)
    FMSTR_TSA_MEMBER(struct pwmm_states_t, duty_a, FMSTR_TSA_UINT32)
    FMSTR_TSA_MEMBER(struct pwmm_states_t, duty_b, FMSTR_TSA_UINT32)
    FMSTR_TSA_MEMBER(struct pwmm_states_t, duty_c, FMSTR_TSA_UINT32)
FMSTR_TSA_TABLE_END()

FMSTR_TSA_TABLE_BEGIN(fmstr_tsa_table_etpu_as)
	FMSTR_TSA_RO_VAR(as_outputs,  FMSTR_TSA_USERTYPE(struct as_outputs_t))
	FMSTR_TSA_STRUCT(struct as_outputs_t)
    FMSTR_TSA_MEMBER(struct as_outputs_t, signal_value, FMSTR_TSA_SINT32)
FMSTR_TSA_TABLE_END()

FMSTR_TSA_TABLE_BEGIN(fmstr_tsa_table_resolver)
    FMSTR_TSA_RO_VAR(resolver_instance,    FMSTR_TSA_USERTYPE(struct resolver_instance_t))
    FMSTR_TSA_RW_VAR(resolver_config,      FMSTR_TSA_USERTYPE(struct resolver_config_t))
    FMSTR_TSA_RO_VAR(resolver_outputs_calculated,   FMSTR_TSA_USERTYPE(struct resolver_outputs_t))
    FMSTR_TSA_RO_VAR(resolver_outputs_extrapolated, FMSTR_TSA_USERTYPE(struct resolver_outputs_t))
    FMSTR_TSA_RO_VAR(resolver_outputs_mechanical, FMSTR_TSA_USERTYPE(struct resolver_outputs_t))
    FMSTR_TSA_RO_VAR(resolver_diag_measures, FMSTR_TSA_USERTYPE(struct resolver_diag_measures_t))
    FMSTR_TSA_RO_VAR(resolver_diag_thresholds, FMSTR_TSA_USERTYPE(struct resolver_diag_thresholds_t))
    FMSTR_TSA_RO_VAR(resolver_diag_flags,  FMSTR_TSA_USERTYPE(struct resolver_diag_flags_t))
    FMSTR_TSA_RO_VAR(resolver_states,      FMSTR_TSA_USERTYPE(struct resolver_states_t))
    FMSTR_TSA_RW_VAR(resolver_diag_control, FMSTR_TSA_UINT8)
    FMSTR_TSA_RO_VAR(resolver_diag_flags_basic, FMSTR_TSA_UINT8)
    FMSTR_TSA_RO_VAR(resolver_diag_flags_advanced, FMSTR_TSA_UINT8)

    FMSTR_TSA_STRUCT(struct resolver_instance_t)
    FMSTR_TSA_MEMBER(struct resolver_instance_t, chan_num_exc, FMSTR_TSA_UINT8)
    FMSTR_TSA_MEMBER(struct resolver_instance_t, chan_num_diag, FMSTR_TSA_UINT8)
    FMSTR_TSA_MEMBER(struct resolver_instance_t, priority, FMSTR_TSA_UINT8)
    FMSTR_TSA_MEMBER(struct resolver_instance_t, start_offset, FMSTR_TSA_UINT32)
    FMSTR_TSA_MEMBER(struct resolver_instance_t, cpba, FMSTR_TSA_UINT32)
    FMSTR_TSA_MEMBER(struct resolver_instance_t, signals_pba, FMSTR_TSA_UINT32)
    FMSTR_TSA_STRUCT(struct resolver_config_t)
    FMSTR_TSA_MEMBER(struct resolver_config_t, options, FMSTR_TSA_UINT8)
    FMSTR_TSA_MEMBER(struct resolver_config_t, excitation_period, FMSTR_TSA_UINT32)
    FMSTR_TSA_MEMBER(struct resolver_config_t, ato_p_gain, FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_config_t, ato_i_gain, FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_config_t, exc_p_gain, FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_config_t, exc_i_gain, FMSTR_TSA_SINT32)
    FMSTR_TSA_STRUCT(struct resolver_outputs_t)
    FMSTR_TSA_MEMBER(struct resolver_outputs_t, angle, FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_outputs_t, speed, FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_outputs_t, rev_counter, FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_outputs_t, sin, FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_outputs_t, cos, FMSTR_TSA_SINT32)
    FMSTR_TSA_STRUCT(struct resolver_diag_measures_t)
    FMSTR_TSA_MEMBER(struct resolver_diag_measures_t, sin_ampl_1     , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_measures_t, sin_ampl_2     , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_measures_t, sin_ampl_min   , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_measures_t, sin_ampl_max   , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_measures_t, sin_mean       , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_measures_t, sin_mean_min   , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_measures_t, sin_mean_max   , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_measures_t, cos_ampl_1     , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_measures_t, cos_ampl_2     , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_measures_t, cos_ampl_min   , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_measures_t, cos_ampl_max   , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_measures_t, cos_mean       , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_measures_t, cos_mean_min   , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_measures_t, cos_mean_max   , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_measures_t, vec_min        , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_measures_t, vec_max        , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_measures_t, ampl_diff      , FMSTR_TSA_SINT32)
    FMSTR_TSA_STRUCT(struct resolver_diag_thresholds_t)
    FMSTR_TSA_MEMBER(struct resolver_diag_thresholds_t, ampl_thrs         , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_thresholds_t, low_ampl_thrs     , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_thresholds_t, mean_thrs         , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_thresholds_t, vec_min_thrs      , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_thresholds_t, vec_max_thrs      , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_thresholds_t, ampl_diff_thrs    , FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_thresholds_t, ato_angle_err_thrs, FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_thresholds_t, ato_speed_thrs    , FMSTR_TSA_SINT32)
    FMSTR_TSA_STRUCT(struct resolver_diag_flags_t)
    FMSTR_TSA_MEMBER(struct resolver_diag_flags_t, actual, FMSTR_TSA_UINT32)
    FMSTR_TSA_MEMBER(struct resolver_diag_flags_t, cumulative, FMSTR_TSA_UINT32)
    FMSTR_TSA_STRUCT(struct resolver_states_t)
    FMSTR_TSA_MEMBER(struct resolver_states_t, exc_phase_shift, FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_states_t, exc_zero_cross_err, FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_states_t, exc_pi_acc, FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_states_t, ato_sig_sin, FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_states_t, ato_sig_cos, FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_states_t, ato_acc, FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_states_t, ato_angle_error, FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_states_t, ato_angle, FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_states_t, ato_speed, FMSTR_TSA_SINT32)
    FMSTR_TSA_MEMBER(struct resolver_states_t, upd, FMSTR_TSA_UINT8)
    FMSTR_TSA_MEMBER(struct resolver_states_t, state, FMSTR_TSA_UINT8)
FMSTR_TSA_TABLE_END()

/* TSA table list */
FMSTR_TSA_TABLE_LIST_BEGIN()
FMSTR_TSA_TABLE(fmstr_tsa_table_etpu_load)
FMSTR_TSA_TABLE(fmstr_tsa_table_etpu_pwmm)
FMSTR_TSA_TABLE(fmstr_tsa_table_resolver)
FMSTR_TSA_TABLE(fmstr_tsa_table_etpu_as)
	FMSTR_TSA_TABLE(MPC5775E_PMSM)
FMSTR_TSA_TABLE_LIST_END()



#endif /* _PMSM_APPFREEMASTER_TSA_H_ */
