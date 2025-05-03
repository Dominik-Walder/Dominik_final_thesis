/**********************************************************************/
// File Name: {FM_project_loc}/PMSM_appconfig.h 
//
// Date:  19. June, 2020
//
// Automatically generated file by MCAT
// - static configuration of the PMSM FOC application
/**********************************************************************/

#ifndef __PMSM_APPCONFIG_H
#define __PMSM_APPCONFIG_H


//Motor Parameters                      
//----------------------------------------------------------------------
//Pole-pair number                      = 3 [-]
//Stator resistance                     = 0.376 [Ohms]
//Direct axis inductance                = 0.00022 [H]
//Quadrature axis inductance            = 0.00027 [H]
//Back-EMF constant                     = 0.0070665 [V.sec/rad]
//Drive inertia                         = 0.000001 [kg.m2]
//Nominal current                       = 5 [A]
//Nominal speed                         = 3000 [rpm]

#define MOTOR_PP                        (3.0F)
#define MOTOR_PP_GAIN                   FRAC32(0.75)
#define MOTOR_PP_SHIFT                  (2)

//Application scales                    
//----------------------------------------------------------------------
#define I_MAX                           (10.0F)
#define U_DCB_MAX                       (36.0F)
#define N_MAX                           (3300.0F)
#define WEL_MAX                         (1036.73F)

//Application Fault Triggers            
//----------------------------------------------------------------------
#define U_DCB_TRIP                      (28.8F)
#define U_DCB_UNDER                     (9.0F)
#define U_DCB_OVER                      (28.8F)
#define I_PH_OVER                       (6.1F)
#define TEMP_OVER                       (100.0F)

//DC Bus voltage IIR1 filter constants  
//----------------------------------------------------------------------
//Cut-off frequency                     = 100 [Hz]
//Sample time                           = 0.0001 [sec]
//----------------------------------------------------------------------
#define UDCB_IIR_B0                     (0.030459027951421222F)
#define UDCB_IIR_B1                     (0.030459027951421222F)
#define UDCB_IIR_A1                     (-0.9390819440971575F)
//Mechanical Alignment                  
#define ALIGN_VOLTAGE                   (0.5F)
#define ALIGN_DURATION                  (30000)

//Current Loop Control                  
//----------------------------------------------------------------------
//Loop Bandwidth                        = 200 [Hz]
//Loop Attenuation                      = 0.707 [-]
//Loop sample time                      = 0.0001 [sec]
//----------------------------------------------------------------------
//Current Controller Output Limit       
#define CLOOP_LIMIT                     (0.9F)
//D-axis Controller - Recurrent type    
#define D_CC1SC                         (0.032285160817402424F)
#define D_CC2SC                         (0.002455846674432126F)
//Q-axis Controller - Recurrent type    
#define Q_CC1SC                         (0.1250772428213575F)
#define Q_CC2SC                         (-0.08244055180865146F)

//Speed Loop Control                    
//----------------------------------------------------------------------
//Loop Bandwidth                        = 20 [Hz]
//Loop Attenuation                      = 1 [-]
//Loop sample time                      = 0.001 [sec]
//----------------------------------------------------------------------
//Speed Controller - Parallel type      
#define SPEED_PI_PROP_GAIN              (0.04355761044838534F)
#define SPEED_PI_INTEG_GAIN             (0.001368402689925734F)
#define SPEED_LOOP_HIGH_LIMIT           (7.65F)
#define SPEED_LOOP_LOW_LIMIT            (-7.65F)

#define SPEED_RAMP_UP                   (0.94248F)
#define SPEED_RAMP_DOWN                 (0.94248F)

#define SPEED_LOOP_CNTR                 (10)

#define POSPE_SPEED_FILTER_MA_LAMBDA    (0.1F)

//Position & Speed Sensors Module       
//Loop sample time                      = 0.0001 [sec]
//----------------------------------------------------------------------
//Resolver Loop Bandwidth               = 110 [Hz]
//Resolver Loop Attenuation             = 0.8 [-]
#define POSPE_RES_TO_CC1                (1129.7250567142432F)
#define POSPE_RES_TO_CC2                (-1081.956171412971F)
#define POSPE_RES_TO_INTEG_GAIN         (0.00005F)
#define RES_PP_GAIN                     (1.0F)

//Control Structure Module - Scalar Control
//----------------------------------------------------------------------
#define SCALAR_VHZ_FACTOR_GAIN          (0.026356058576017863F)
#define SCALAR_INTEG_GAIN               FRAC32(0.0165)
#define SCALAR_INTEG_SHIFT              (0)
#define SCALAR_RAMP_UP                  (0.09425F)
#define SCALAR_RAMP_DOWN                (0.09425F)

//FreeMASTER Scale Variables            
//----------------------------------------------------------------------
//Note: Scaled at input = 1000          
//----------------------------------------------------------------------
#define FM_SPEED_RAD_MEC_SCALE          (1000)
#define FM_SPEED_RPM_EL_SCALE           (9549)
#define FM_SPEED_RPM_MEC_SCALE          (3183)
#define FM_POSITION_DEG_SCALE           (57296)

#endif

//End of generated file                 
/**********************************************************************/
