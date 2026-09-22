/*
 * Copyright (c) 2025 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef _MOTOR_COMMON_H_
#define _MOTOR_COMMON_H_

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

/* Forward declaration for SYSTEM_Vars_t (defined in user.h) */
struct _SYSTEM_Vars_t_;
typedef struct _SYSTEM_Vars_t_ SYSTEM_Vars_t;

// Platform types for boolean and standard types (TRUE/FALSE definitions)
#include "Platform_Types.h"

// ARM M33 math library with TMU support
// Provides __sin, __cos, __sqrt macros mapped to TMU-accelerated functions

// ti_math.h internally includes <math.h> and fastrts_tmu.h for TMU acceleration
#include "ti_math.h"

#include "userParams.h"
#include "est.h"
#include "dclink_ss.h"
#include "clarke.h"
#include "filter_fo.h"
#include "ipark.h"
#include "park.h"
#include "pi.h"
#include "svgen.h"
#include "svgen_current.h"
#include "traj.h"
#include "fwc.h"
#include "mtpa.h"
#include "vsf.h"
#include "vs_freq.h"
#include "angle_gen.h"
#include "volt_recons.h"
#include "dead_time_comp.h"
#include "ipd.h"
#include "esmo.h"
#include "speedfr.h"
#include "datalogIF.h"
#include "Ti_Tune.h"

//*****************************************************************************
#define M_OVER_VOLTAGE_BIT          0x0001    // DC Bus Over Voltage Fault
#define M_UNDER_VOLTAGE_BIT         0x0002    // DC Bus Under Voltage Fault
#define M_MOTOR_OVER_TEMPER_BIT     0x0004    // Motor over temperature Fault
#define M_MODULE_OVER_TEMPER_BIT    0x0008    // Module over temperature Fault

#define M_MODULE_OVER_CURRENT_BIT   0x0010    // Hardware Over Current Fault
#define M_OVER_PEAK_CURRENT_BIT     0x0020    // internal CMPSS Over Current Fault
#define M_MOTOR_OVER_LOAD_BIT       0x0040    // Over Load Error
#define M_MOTOR_LOST_PHASE_BIT      0x0080    // Motor Lost Phase

#define M_CURRENT_UNBALANCE_BIT     0x0100    // Motor Phase Current Unbalance
#define M_MOTOR_STALL_BIT           0x0200    // Motor Stall
#define M_STARTUP_FAILE_BIT         0x0400    // Startup failed
#define M_MOTOR_OVER_SPEED_BIT      0x0800    // Motor Over Speed

#define M_RESERVE_12_BIT            0x1000    // Reserved for further use
#define M_RESERVE_13_BIT            0x2000    // Reserved for further use
#define M_CURRENT_OFFSET_BIT        0x4000    // Current offsets
#define M_VOLTAGE_OFFSET_BIT        0x8000    // voltage offsets

#define M_MASK_ALL_FAULT_BITS       0x0000
#define M_ENABLE_ALL_FAULT_BITS     0xFFFF

//
// Block all fault protection except current, voltage and temperature faults
//
#define MTR_FAULT_OV_BRAKE             M_OVER_VOLTAGE_BIT

#define MTR_FAULT_ENABLE_OC             M_MODULE_OVER_CURRENT_BIT              \
                                      + M_OVER_PEAK_CURRENT_BIT

#define MTR_FAULT_ENABLE_OC_OUV         M_OVER_VOLTAGE_BIT                     \
                                      + M_UNDER_VOLTAGE_BIT                    \
                                      + M_MODULE_OVER_CURRENT_BIT              \
                                      + M_OVER_PEAK_CURRENT_BIT                \
                                      + M_CURRENT_OFFSET_BIT                   \
                                      + M_VOLTAGE_OFFSET_BIT
//
// Enable all fault protection
//
#define MTR_FAULT_ENABLE_ALL            M_OVER_VOLTAGE_BIT                     \
                                      + M_UNDER_VOLTAGE_BIT                    \
                                      + M_MOTOR_OVER_TEMPER_BIT                \
                                      + M_MODULE_OVER_TEMPER_BIT               \
                                      + M_MODULE_OVER_CURRENT_BIT              \
                                      + M_OVER_PEAK_CURRENT_BIT                \
                                      + M_MOTOR_OVER_LOAD_BIT                  \
                                      + M_MOTOR_LOST_PHASE_BIT                 \
                                      + M_CURRENT_UNBALANCE_BIT                \
                                      + M_MOTOR_STALL_BIT                      \
                                      + M_STARTUP_FAILE_BIT                    \
                                      + M_MOTOR_OVER_SPEED_BIT                 \
                                      + M_CURRENT_OFFSET_BIT                   \
                                      + M_VOLTAGE_OFFSET_BIT

// Clear all fault protection except over/under voltage and offset error
//
#define MTR_FAULT_CLEAR                 M_OVER_VOLTAGE_BIT                     \
                                      + M_UNDER_VOLTAGE_BIT                    \
                                      + M_MOTOR_OVER_TEMPER_BIT                \
                                      + M_MODULE_OVER_TEMPER_BIT

#define MTR_FAULT_DISABLE_ALL           0x0000

#define MTR1_FAULT_MASK_SET             MTR_FAULT_ENABLE_OC_OUV
//#define MTR1_FAULT_MASK_SET           MTR_FAULT_ENABLE_ALL


//
//! \brief Enumeration for the estimator mode
//

typedef enum
{
    ESTIMATOR_MODE_FAST  = 0,              //!< FAST estimator
    ESTIMATOR_MODE_ESMO  = 1               // ESMO Estimator
} ESTIMATOR_Mode_e;
//
//! \brief Enumeration for the using estimator algorithm
//

typedef enum
{
    EST_TYPE_FAST      = 0,         //!< the estimator is only FAST
    EST_TYPE_ESMO      = 1,         //!< the estimator is only ESMO
    EST_TYPE_ENC       = 2,         //!< the estimator is only ENC
    EST_TYPE_HALL      = 3,         //!< the estimator is only HALL
    EST_TYPE_FAST_ENC  = 4,         //!< the estimator is FAST and ENC
    EST_TYPE_FAST_ESMO = 5,         //!< the estimator is FAST and ESMO
    EST_TYPE_FAST_HALL = 6,         //!< the estimator is FAST and HALL
    EST_TYPE_ESMO_ENC  = 7,         //!< the estimator is ESMO and ENC
    EST_TYPE_ISBLDC    = 8          //!< the estimator is only ISBLDC
} EST_Type_e;

typedef enum
{
    FAST_TYPE_SOFTLIB_FLASH  = 0,   //!< the FAST software library and run in Flash
    FAST_TYPE_ROMLIB_FLASH   = 1,   //!< the FAST ROM library and run in Flash
    FAST_TYPE_ROMLIB_RAM     = 2,   //!< the FAST ROM library and run in RAM
    FAST_TYPE_NONFAST_FLASH  = 3,   //!< the non FAST and run in Flash
    FAST_TYPE_NONFAST_RAM    = 4    //!< the non FAST and run in RAM
} FASTLIB_Type_e;


//
//! \brief typedefs for the fault
//
typedef struct _FAULT_MTR_BITS_
{             // bits  description
    uint16_t overVoltage:1;         // 0  DC Bus Over Voltage Fault
    uint16_t underVoltage:1;        // 1  DC Bus Under Voltage Fault
    uint16_t motorOverTemp:1;       // 2  Motor over temperature Fault
    uint16_t moduleOverTemp:1;      // 3  Power module over temperature Fault

    uint16_t moduleOverCurrent:1;   // 4  Hardware Over Current Fault Flag
    uint16_t overPeakCurrent:1;     // 5  internal CMPSS Over Current Fault Flag
    uint16_t overLoad:1;            // 6  Over Load Error
    uint16_t motorLostPhase:1;      // 7  Motor Lost Phase

    uint16_t currentUnbalance:1;    // 8  Motor Phase Current imbalance
    uint16_t motorStall:1;          // 9  Motor Stall
    uint16_t startupFailed:1;       // 10 Startup failed
    uint16_t overSpeed:1;           // 11 Motor Over Speed

    uint16_t estimError:1;           // 12 Error in Estimator
    uint16_t gateDriverFault:1;           // 13 gateDriver Fault
    uint16_t currentOffset:1;       // 14 Current offset check
    uint16_t voltageOffset:1;       // 15 voltage offset check
} FAULT_MTR_BITS;

typedef union _FAULT_MTR_REG_t
{
    uint16_t        all;
    FAULT_MTR_BITS  bit;
}FAULT_MTR_REG_t;

/*! @brief DAC_CNTRL register */
typedef struct
{
    /*! DAC enable */
    uint16_t dacEn;
    /*! Unipolar ouput Left or right shift, used if dacScalingFactor is 0 */
    int16_t mode;
    /*! Dac scale factor in iq 27 */
    float32_t dacScalingFactor;
    /*! Variable address to output through DAC */
    float32_t *pDacOutAddr;

    uint32_t dacData;
}RAM_DAC_CNTRL_T;

typedef struct _MOTOR_SetVars_t_
{
    uint16_t overCurrentTimesSet;
    uint16_t voltageFaultTimeSet;
    uint16_t motorStallTimeSet;
    uint16_t startupFailTimeSet;
    uint16_t overSpeedTimeSet;
    uint16_t overLoadTimeSet;
    uint16_t unbalanceTimeSet;
    uint16_t lostPhaseTimeSet;
    uint16_t RsOnlineWaitTimeSet;
    uint16_t RsOnlineWorkTimeSet;
    uint16_t stopWaitTimeSet;
    uint16_t restartWaitTimeSet;
    uint16_t restartTimesSet;
    uint16_t bootChargeTimeSet;

    float32_t Rr_Ohm;
    float32_t Rs_Ohm;
    float32_t Ls_d_H;
    float32_t Ls_q_H;
    float32_t flux_VpHz;
    float32_t flux_Wb;
    float32_t RoverL_rps;

    float32_t RsOnLine_Ohm;

    float32_t Ls_d_comp_H;
    float32_t Ls_q_comp_H;

    float32_t Ls_min_H;

    float32_t Ls_d_Icomp_coef;
    float32_t Ls_q_Icomp_coef;

    float32_t Kp_spd;
    float32_t Ki_spd;

    float32_t Kp_Id;
    float32_t Ki_Id;

    float32_t Kp_Iq;
    float32_t Ki_Iq;

    float32_t Kp_fwc;
    float32_t Ki_fwc;
    float32_t angleFWCMax_rad;

    float32_t overModulation;
    float32_t RsOnLineCurrent_A;
    float32_t magneticCurrent_A;

    float32_t lostPhaseSet_A;
    float32_t unbalanceRatioSet;
    float32_t overLoadSet_W;

    float32_t toqueFailMinSet_Nm;
    float32_t speedFailMaxSet_Hz;

    float32_t speedFailMinSet_Hz;
    float32_t stallCurrentSet_A;
    float32_t IsFailedChekSet_A;

    float32_t maxPeakCurrent_A;
    float32_t overCurrent_A;

    float32_t overVoltageFault_V;
    float32_t overVoltageNorm_V;
    float32_t underVoltageFault_V;
    float32_t underVoltageNorm_V;

} MOTOR_SetVars_t;

//! \brief Defines the MOTOR_SetVars_t handle
//!
typedef struct _MOTOR_SetVars_t_ *MOTORSETS_Handle;

//******************************************************************************
// typedefs
typedef struct _MOTOR_Vars_t_
{
    bool flagEnableRunAndIdentify;
    bool flagRunIdentAndOnLine;
    bool flagEnableRestart;
    bool flagMotorIdentified;
    bool flagEnableForceAngle;
    bool flagEnableAlignment;
    bool flagEnableContAlign;
    bool flagSetupController;
    bool flagEnableSpeedCtrl;
    bool flagEnableCurrentCtrl;
    bool flagMotorTiTuneEnable;
    bool flagMotorTiTuneRun;

    bool flagEnableFlyingStart;
    bool flagEnableBraking;
    bool flagBrakeDone;

    bool flagEnableIPD;
    bool flagEnableFWC;
    bool flagEnableMTPA;
    bool flagUpdateMTPAParams;

    bool flagClearFaults;
    bool flagVIrmsCal;

    bool enableSpeedCtrl;
    bool enableCurrentCtrl;

    bool flagEnableRsRecalc;
    bool flagEnableLsUpdate;

    bool flagEnableMotorIdentify;
    bool flagEnablePowerWarp;
    bool flagBypassLockRotor;
    bool flagEnableRsOnLine;
    bool flagStartRsOnLine;
    bool flagRsOnLineContinue;

    bool flagEstStateChanged;

    bool flagEnableAlignDebug;
    bool flagEnableOpenLoopDebug;
    bool forceOpenLoopVoltageMode;

    uint16_t RsOnlineTimeCnt;

    EST_State_e estState;
    EST_Traj_State_e trajState;

    FAULT_MTR_REG_t faultMtrNow;
    FAULT_MTR_REG_t faultMtrUse;
    FAULT_MTR_REG_t faultMtrMask;
    FAULT_MTR_REG_t faultMtrPrev;

    ESTIMATOR_Mode_e estimatorMode;
    SAMPLE_Mode_e sampleMode;
    MOTOR_Status_e motorState;

    FlyingStart_Mode_e flyingStartMode;
    BRAKE_Mode_e brakingMode;
    OPERATE_Mode_e operateMode;

    SVM_Mode_e svmMode;
    MotorNum_e motorNum;

    uint16_t overCurrentTimesCnt;
    uint16_t overVoltageTimeCnt;
    uint16_t underVoltageTimeCnt;

    uint16_t motorStallTimeCnt;
    uint16_t startupFailTimeCnt;

    uint16_t bootChargeTimeCnt;
    uint16_t stopWaitTimeCnt;
    uint16_t restartTimesCnt;
    uint16_t startSumTimesCnt;

    uint16_t overSpeedTimeCnt;
    uint16_t overLoadTimeCnt;
    uint16_t unbalanceTimeCnt;
    uint16_t lostPhaseTimeCnt;

    uint16_t VIrmsIsrSet;
    uint16_t VIrmsIsrCnt;

    uint32_t brakingTimeDelay;
    uint32_t brakingTimeCnt;
    uint32_t stateRunTimeCnt;

    uint16_t forceRunTimeDelay;
    uint32_t alignTimeDelay;
    uint16_t fwcTimeDelay;
    uint16_t flyingStartTimeDelay;

    uint16_t counterSpeed;
    uint16_t counterTrajSpeed;

    uint32_t ISRCount;

    float32_t debugData;

    float32_t direction;                    // 1.0f->forward, -1.0f->reserve

    float32_t speedRef_Hz;
    float32_t speed_int_Hz;                 // Speed reference value, Hz
    float32_t speed_Hz;
    float32_t speed_rps;

    float32_t speedStart_Hz;
    float32_t speedForce_Hz;
    float32_t speedAbs_Hz;
    float32_t speedFilter_Hz;
    float32_t speedFlyingStart_Hz;

    float32_t accelerationMax_Hzps;
    float32_t accelerationStart_Hzps;

    float32_t angleFWC_rad;
    float32_t angleCurrent_rad;

    float32_t angleOffsetIPD_rad;
    float32_t angleDetectIPD_rad;

    float32_t Is_A;
    float32_t Vs_V;
    float32_t VsRef_pu;
    float32_t VsRef_V;
    float32_t oneOverDcBus_invV;    //!< the DC Bus inverse, 1/V


    float32_t IdRated_A;
    float32_t IsRef_A;
    float32_t IsSet_A;

    float32_t fluxCurrent_A;
    float32_t alignCurrent_A;
    float32_t startCurrent_A;
    float32_t maxCurrent_A;
    float32_t brakingCurrent_A;

    float32_t torque_Nm;

    MATH_Vec2 Vab_out_V;             // the output control voltage on alpha&beta axis

    MATH_Vec2 Vdq_out_V;            // the output control voltage on d&q axis

    MATH_Vec2 Vdq_offset_V;         // the output offset voltage on d&q axis

    MATH_Vec2 Vdq_OpenLoop;         // the output offset voltage on Forced Open Loop Mode

    MATH_Vec2 Iab_A;                // the alpha/beta current values, A

    MATH_Vec2 Vab_V;                // the alpha/beta current values, V

    // the d&q axis current are converter from 3-phase sampling input current of motor
    MATH_Vec2 Idq_in_A;

    // the reference current on d&q rotation axis
    MATH_Vec2 IdqRef_A;

    // the reference output current on d&q rotation axis
    MATH_Vec2 Idq_out_A;

    float32_t frswPos_sf;

    float32_t angleDelayed_sf;

    // sensorless-FOC
    MATH_Vec2 phasor;

    // the rotor angle compensation value during OL Transition
    float32_t angleAlign_rad;

    // the rotor angle compensation value during OL Transition
    float32_t angleOLStep_rad;

    // the rotor angle compensation value during OL Transition
    float32_t angleOLDelta_rad;

    // the rotor angle compensation value
    float32_t angleDelta_rad;

    // the rotor angle delay compensation value
    float32_t angleComp_rad;

    // the rotor angle from Generator modules
    float32_t angleGen_rad;

    // the rotor angle from FOC modules
    volatile float32_t angleFOC_rad;

    // the rotor angle from EST modules
    float32_t angleEST_rad;

    // the rotor angle from SMO modules
    float32_t angleSMO_rad;

    // the rotor angle from PLL modules
    float32_t anglePLL_rad;

    // the rotor angle from Encoder modules
    float32_t angleENC_rad;

    // the rotor angle from Hall Sensor
    float32_t angleHall_rad;

    // the rotor angle for braking
    float32_t angleBrake_rad;

    // the speed from EST module
    float32_t speedEST_Hz;

    // the speed from PLL module
    float32_t speedPLL_Hz;

    // the speed from Encoder module
    float32_t speedENC_Hz;

    // the speed from Hall Sensor
    float32_t speedHall_Hz;

    // the speed from ISBLDC module
    float32_t speedINT_Hz;

    float32_t VIrmsIsrScale;
    float32_t IrmsCalSF;

    float32_t IrmsCalSum[3];
    float32_t IrmsPrdSum[3];
    float32_t Irms_A[3];

    float32_t unbalanceRatio;

    float32_t power_sf;
    float32_t powerReal_W;

    float32_t powerInvtOut_W;
    float32_t powerActive_W;

    // the handle for the hardware abstraction layer to motor control
    const HAL_FOC_Interface_t *const halInterface;

    /* object for Gate Driver */

    // The handle for Gate Driver Configurations
    const HAL_GD_Interface_t  *gateDriveHandle;

    MOTORSETS_Handle motorSetsHandle;

    //#TAG_FAST_OBS_START#
    userParams_Handle   userParamsHandle;

    // the handle for the estimator (keep as handle - external library)
    EST_Handle    estHandle;
    //#TAG_FAST_OBS_END#

    // ========== FOC Transform Objects (EMBEDDED) ==========
    // the voltage Clarke transform object
    CLARKE_Obj clarke_V;

    // the current Clarke transform object
    CLARKE_Obj clarke_I;

    // the inverse Park transform object
    IPARK_Obj  ipark_V;

    // the current Park transform object
    PARK_Obj   park_I;

    // the voltage Park transform object
    PARK_Obj   park_V;

    // ========== Controller Objects (EMBEDDED) ==========
    // the speed PI controller object
    PI_Obj     pi_spd;

    // the Id PI controller object
    PI_Obj     pi_Id;

    // the Iq PI controller object
    PI_Obj     pi_Iq;

    // the field weakening PI controller object
    PI_Obj     pi_fwc;

    // ========== Reference Generation (EMBEDDED) ==========
    // the speed reference trajectory object
    TRAJ_Obj   traj_spd;

    // the Angle Generate object for open loop control
    ANGLE_GEN_Obj angleGen;

    // the Vs per Freq object for open loop control
    VS_FREQ_Obj VsFreq;

    // ========== Space Vector Generation (EMBEDDED) ==========
    // the space vector generator object
    SVGEN_Obj  svgen;

    //#TAG_MTPA_START#
    // ========== Advanced Features (HANDLES - Optional) ==========
    // the Maximum torque per ampere (MTPA) handle
    MTPA_Handle   mtpaHandle;
    //#TAG_MTPA_END#

    // #TAG_DEAD_TIME_COMP_START#
    // Dead Time Compensation handle
    DTC_Handle    deadTimeCompHandle;
    // #TAG_DEAD_TIME_COMP_END#

    //#TAG_READ_INV_VOLT_START#
    // the voltage reconstruct handle
    VOLREC_Handle volrecHandle;
    //#TAG_READ_INV_VOLT_END#

    // #TAG_IPD_START#
    // ========== Position/Angle Detection (HANDLES - Optional) ==========
    // the Initial position Detection (IPD) handle
    IPD_Handle    ipdHandle;
    // #TAG_IPD_END#

    // #TAG_ESMO_START#
    // ========== Estimators/Observers (HANDLES - Optional) ==========
    // the Enhanced Sliding Mode Observer (ESMO) handle
    ESMO_Handle   esmoHandle;
    // #TAG_ESMO_END#

    // the speed frequency calculator object
    SPDFR_Obj  spdfr;

    // ========== MTPA and Online Parameters ==========
    float32_t angleMTPA_rad;
    float32_t mtpaKconst;
    float32_t LsOnline_d_H;
    float32_t LsOnline_q_H;
    float32_t fluxOnline_Wb;

    float32_t sector;

    // EST input/output data structures
    EST_InputData_t estInputData;
    EST_OutputData_t estOutputData;

    // the rotor angle delay compensation value
    float32_t anglePLLComp_rad;


    HAL_FOC_Data_Obj *halFOCData;


    HAL_ADCData_t *adcData;


    HAL_PWMData_t *pwmData;


    MATH_Vec3 *pwmVabc_pu;

    //#TAG_TI_TUNE_START#
    TiTune_Handle tiTuneHandle;
    //#TAG_TI_TUNE_END#

}MOTOR_Vars_t;

//! \brief Defines the MOTOR_Vars_t handle
//!
typedef struct _MOTOR_Vars_t_ *MOTOR_Handle;

//*****************************************************************************
// the function prototypes

//! \brief checks motor faults
//! \param[in] handle The motor control handle
extern void checkMotorFaults(MOTOR_Handle handle);

//! \brief     Sets the number of current sensors
//! \param[in] handle             The Clarke (CLARKE) handle
//! \param[in] numCurrentSensors  The number of current sensors
extern void setupClarke_I(CLARKE_Handle handle, const uint16_t numCurrentSensors);

//! \brief     Sets the number of voltage sensors
//! \param[in] handle             The Clarke (CLARKE) handle
//! \param[in] numVoltageSensors  The number of voltage sensors
extern void setupClarke_V(CLARKE_Handle handle, const uint16_t numVoltageSensors);

//! \brief  Update the controllers
static inline void updateControllers(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;
    MOTOR_SetVars_t *objSets = (MOTOR_SetVars_t *)(handle->motorSetsHandle);

    if((obj->motorState == MOTOR_CTRL_RUN) && (obj->flagMotorIdentified == true))
    {
        // update the Id controller
        PI_setGains(&obj->pi_Id, objSets->Kp_Id, objSets->Ki_Id);

        // update the Iq controller
        PI_setGains(&obj->pi_Iq, objSets->Kp_Iq, objSets->Ki_Iq);

        // update the speed controller
        PI_setGains(&obj->pi_spd, objSets->Kp_spd, objSets->Ki_spd);
    }
}

//! \brief  Get the controllers Parameters
static inline void getControllers(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;
    MOTOR_SetVars_t *objSets = (MOTOR_SetVars_t *)(handle->motorSetsHandle);

    // Get the Id controller parameters
    objSets->Kp_Id = PI_getKp(&obj->pi_Id);
    objSets->Ki_Id = PI_getKi(&obj->pi_Id);

    // Get the Iq controller parameters
    objSets->Kp_Iq = PI_getKp(&obj->pi_Iq);
    objSets->Ki_Iq = PI_getKi(&obj->pi_Iq);

    // Get the speed controller parameters
    objSets->Kp_spd = PI_getKp(&obj->pi_spd);
    objSets->Ki_spd = PI_getKi(&obj->pi_spd);
}

//! \brief  Sets up control parameters for stopping motor
extern void stopMotorControl(MOTOR_Handle handle);

//! \brief  Sets up control parameters for restarting motor
extern void restartMotorControl(MOTOR_Handle handle);

//! \brief  Resets motor control parameters for restarting motor
extern void resetMotorControl(MOTOR_Handle handle);

//! \brief  Sets up the current controllers
extern void setupCurrentControllers(MOTOR_Handle handle);

//! \brief  Sets up the controllers
extern void setupControllers(MOTOR_Handle handle);

//! \brief  Collect the current and voltage data to calculate the RMS
extern void collectRMSData(MOTOR_Handle handle);

//! \brief  Calculate the RMS data
extern void calculateRMSData(MOTOR_Handle handle);

//! \brief run motor monitor in main loop timer
extern void runMotorMonitor(MOTOR_Handle handle);

//! \brief Rs online calibration
extern void runRsOnLine(MOTOR_Handle handle);

//! \brief      Updates the global motor variables
//! \param[in]  estHandle   The estimator (EST) handle
extern void updateGlobalVariables(MOTOR_Handle handle);

//! \brief      Updates the FWC parameters
extern void updateFWCParams(MOTOR_Handle handle);

//! \brief      Updates the MTPA parameters
extern void updateMTPAParams(MOTOR_Handle handle);

//! \brief Initialize motor control system (interrupts, PWM, gate driver, parameters, offset calibration)
//! \param[in] motorHandle The motor control handle
//! \param[in] isrFunc Pointer to the motor's interrupt service routine
void initMotorControlSystem(MOTOR_Handle motorHandle, void (*isrFunc)(void));

//! \brief Initialize datalogging and debug outputs
//! \param[in] motorHandle The motor control handle
//! \param[in] halHandle The HAL handle
//! \param[in] datalogHandle Pointer to datalog handle
//! \param[in] datalog Pointer to datalog object
//! \param[in] datalogBuff1 Pointer to datalog buffer 1
//! \param[in] datalogBuff2 Pointer to datalog buffer 2
extern void initDatalogging(MOTOR_Handle motorHandle,
                           HAL_Handle halHandle,
                           DATALOG_Handle *datalogHandle,
                           DATALOG_Obj *datalog,
                           float32_t *datalogBuff1,
                           float32_t *datalogBuff2);

extern void monitorOverLoad(MOTOR_Handle motorHandle);
extern void monitorMotorStall(MOTOR_Handle motorHandle);
extern void monitorLostPhase(MOTOR_Handle motorHandle);
extern void monitorCurrentUnbalance(MOTOR_Handle motorHandle);
extern void monitorOverSpeed(MOTOR_Handle motorHandle);
extern void monitorStartupFailure(MOTOR_Handle motorHandle);

//! \brief Initialize DAC outputs for real-time debugging
//! \param[in] sysVarsPtr Pointer to the system variables structure
//! \param[in] motorHandle The motor control handle
extern void initDACOutputs(SYSTEM_Vars_t *sysVarsPtr,
                           MOTOR_Handle motorHandle);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif // extern "C"

#endif // end of MOTOR_COMMON_H definition
