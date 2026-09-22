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

#ifndef APPLICATION_H
#define APPLICATION_H

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

// ARM M33 math library with TMU support
// Provides __sin, __cos, __sqrt macros mapped to TMU-accelerated functions
// ti_math.h internally includes <math.h> and fastrts_tmu.h for TMU acceleration
#include "ti_math.h"

#include "motor_common.h"


// *****************************************************************************
// the function prototypes

extern void application_run(MOTOR_Handle handle);

//! \brief Low priority state machine for background monitoring and protection
extern void FOC_runLowPriorityStateMachine(MOTOR_Handle handle);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif // extern "C"

// ============================================================================
// STATE MACHINE FUNCTIONS - STATIC INLINE (for use in monolithic version)
// ============================================================================

/**
 * @brief Handles IPD (Initial Position Detection) state
 * @param obj Pointer to motor control structure
 */
static inline void StateMachine_handleIPD(MOTOR_Vars_t *obj)
{
    ipdRun(obj->ipdHandle);

    if(getIpdComplete(obj->ipdHandle))
    {
        if(getIpdTestMode(obj->ipdHandle))
        {
            obj->motorState = MOTOR_ALIGNMENT;
            obj->angleFOC_rad = obj->ipdHandle->angle * MATH_TWO_PI;
            obj->flagEnableAlignment = true;
            obj->stateRunTimeCnt  = 0;
        }
        else
        {
            // IPD complete, transition to open-loop start
            obj->motorState = MOTOR_OL_START;
            obj->Idq_out_A.value[0] = 0;

            if(obj->direction > 0.0f)
            {
                obj->Idq_out_A.value[1] = obj->startCurrent_A;
                obj->IsRef_A = obj->startCurrent_A;
                obj->angleFOC_rad = obj->ipdHandle->angle * MATH_TWO_PI;
            }
            else
            {
                obj->IsRef_A = -obj->startCurrent_A;
                obj->Idq_out_A.value[1] = -obj->startCurrent_A;
                obj->angleFOC_rad = obj->ipdHandle->angle * MATH_TWO_PI;
            }

            //TODO: FAST Lib Dependency
//            //#TAG_FAST_OBS_START#
//            // Initialize estimators with detected angle
//            EST_setAngle_rad(obj->estHandle, obj->angleFOC_rad);
//            EST_setAngle_est_rad(obj->estHandle, obj->angleFOC_rad);
//            // #TAG_FAST_OBS_END#

            TRAJ_setIntValue(&obj->traj_spd, 0.0);

            //#TAG_ESMO_START#
            ESMO_setAnglePu(obj->esmoHandle, obj->angleFOC_rad);
            //#TAG_ESMO_END#

            ANGLE_GEN_setAngle(&obj->angleGen, obj->angleFOC_rad);
            PI_setUi(&obj->pi_spd, obj->Idq_out_A.value[1]);
        }
    }

    if(getIpdFaultStatus(obj->ipdHandle))
    {
        obj->faultMtrNow.bit.motorLostPhase = 1;
        obj->motorState = MOTOR_FAULT_STOP;
    }
}


/**
 * @brief Handles motor Parameter Identification state for tiTune
 * @param obj Pointer to motor control structure
 */
static inline void StateMachine_handleTiTune(MOTOR_Vars_t *obj)
{
        /* TiTune Input Speed estimated from estimator */
        obj->tiTuneHandle->TiTune_Speed_in_Hz = obj->speed_Hz;

        obj->tiTuneHandle->TiTune_FOC_Angle_in_rad = obj->angleFOC_rad;


        /* Input the DC Bus Voltage ADC data */
        obj->tiTuneHandle->VdcBus_V = (obj->halFOCData->adcData.VdcBus_V);

        /* Link the Alpha Current */
        obj->tiTuneHandle->TiTune_Iab_in_A.value[0] =  obj->estInputData.Iab_A.value[0];

        /* Link the Beta Current */
        obj->tiTuneHandle->TiTune_Iab_in_A.value[1] =  obj->estInputData.Iab_A.value[1];

        //  Run the TI_Tune Run
        TiTune_Run_stateMachine(obj->tiTuneHandle);

//        obj->Vdq_out_V.value[0] = obj->tiTuneHandle->Ti_Tune_Out_Vdq_V.value[0];
//
//        obj->Vdq_out_V.value[1] = obj->tiTuneHandle->Ti_Tune_Out_Vdq_V.value[1];
//
//        obj->angleFOC_rad = obj->tiTuneHandle->TiTune_FOC_Angle_in_rad;

        /* If the TI tune method is completed , switch to regular operation*/

//        /* Update the Observer Parameters based on the Estimated Rs & Ld,Lq */
//        if(obj->tiTuneHandle->TiTune_state == TiTune_STATE_OBSERVER_SETUP)
//        {
//            setObserverParams(obj->tiTuneHandle,motorHandle_M1);
//        }

        if((obj->tiTuneHandle->TiTune_state == TiTune_STATE_DONE) ||
                (obj->flagMotorTiTuneEnable == false))
        {
            /* Set the Output Voltage to Zero */
            obj->Vab_out_V.value[0] = 0;
            obj->Vab_out_V.value[1] = 0;

            obj->flagMotorTiTuneRun = false;
            obj->flagMotorTiTuneEnable = false;
            obj->flagRunIdentAndOnLine = false;
            obj->flagEnableRunAndIdentify = false;
            obj->motorState = MOTOR_STOP_IDLE;
            obj->tiTuneHandle->TiTune_state = TiTune_STATE_IDLE;
        }

}
/**
 * @brief Handles motor Parameter Identification state
 * @param obj Pointer to motor control structure
 */
static inline void StateMachine_handleParameterID(MOTOR_Vars_t *obj)
{
    //TODO: FAST Lib Dependency
//    // Handle motor identification or Rs recalibration
//    if(((EST_isMotorIdentified(obj->estHandle) == false) ||
//        (EST_getState(obj->estHandle) == EST_STATE_RS)) &&
//        (EST_isEnabled(obj->estHandle) == true))
//    {
//        obj->Idq_out_A.value[0] = 0.0f;
//        obj->motorState = MOTOR_PARAM_IDENT;
//
//        // Setup trajectory generator for identification
//        EST_setupTrajState(obj->estHandle, obj->Idq_out_A.value[1],
//                          obj->speedRef_Hz, 0.0);
//        EST_runTraj(obj->estHandle);
//
//        obj->IdRated_A = EST_getIntValue_Id_A(obj->estHandle);
//        obj->estInputData.speed_ref_Hz = EST_getIntValue_spd_Hz(obj->estHandle);
//        obj->speed_int_Hz = obj->estInputData.speed_ref_Hz;
//
//        obj->enableSpeedCtrl = EST_doSpeedCtrl(obj->estHandle);
//        obj->enableCurrentCtrl = EST_doCurrentCtrl(obj->estHandle);
//    }
//
//    obj->angleFOC_rad = obj->angleEST_rad;
}
/**
 * @brief Handles motor alignment state
 * @param obj Pointer to motor control structure
 */
static inline void StateMachine_handleAlignment(MOTOR_Vars_t *obj)
{
    obj->enableSpeedCtrl = false;
    obj->enableCurrentCtrl = true;

    obj->IsRef_A = 0.0f;
    obj->Idq_out_A.value[0] = obj->alignCurrent_A;
    obj->Idq_out_A.value[1] = 0.0f;

    if(obj->flagEnableAlignDebug == true)
    {
        obj->enableCurrentCtrl = false;
        obj->Vdq_out_V.value[0] = obj->alignCurrent_A * obj->userParamsHandle->motor_Rs_Ohm;
        obj->Vdq_out_V.value[1] = 0.0f;
    }
    TRAJ_setIntValue(&obj->traj_spd, 0.0f);

    // #TAG_ESMO_START#
    ESMO_setAnglePu(obj->esmoHandle, obj->angleAlign_rad);
    // #TAG_ESMO_END#

    ANGLE_GEN_setAngle(&obj->angleGen, obj->angleAlign_rad);

    obj->angleFOC_rad = obj->angleAlign_rad;

    if(((obj->stateRunTimeCnt > obj->alignTimeDelay) ||
       (obj->flagEnableAlignment == false)) && (!obj->flagEnableContAlign))
    {
        // Alignment complete, transition to open-loop start
        obj->stateRunTimeCnt = 0;
        obj->motorState = MOTOR_OL_START;
        if(obj->direction > 0.0f)
        {
            obj->Idq_out_A.value[0] = 0;
            obj->IsRef_A = obj->startCurrent_A;
            obj->Idq_out_A.value[1] = obj->startCurrent_A;
        }
        else
        {
            obj->Idq_out_A.value[0] = 0;
            obj->IsRef_A = -obj->startCurrent_A;
            obj->Idq_out_A.value[1] = -obj->startCurrent_A;
        }
        obj->enableSpeedCtrl = false;
        obj->enableCurrentCtrl = true;
        //TODO: FAST Lib Dependency
//        //#TAG_FAST_OBS_START#
//        EST_setAngle_rad(obj->estHandle, obj->angleFOC_rad);
//        //#TAG_FAST_OBS_END#

        PI_setUi(&obj->pi_spd, obj->Idq_out_A.value[1]);
    }
}

/**
 * @brief Runs angle generation for open-loop control
 * @param obj Pointer to motor control structure
 */
static inline void FOC_runAngleGeneration(MOTOR_Vars_t *obj)
{
    TRAJ_run(&obj->traj_spd);
    obj->speed_int_Hz = TRAJ_getIntValue(&obj->traj_spd);

    // Ramp angle based on speed (used in open-loop)
    ANGLE_GEN_run(&obj->angleGen, obj->speed_int_Hz);
    obj->angleGen_rad = ANGLE_GEN_getAngle(&obj->angleGen);
}

/**
 * @brief Handles open-loop start state
 * @param obj Pointer to motor control structure
 */
static inline void StateMachine_handleOpenLoopStart(MOTOR_Vars_t *obj)
{
    obj->enableSpeedCtrl = false;

    if(obj->estimatorMode == ESTIMATOR_MODE_ESMO)
    {
        obj->enableCurrentCtrl = true;

        obj->Idq_out_A.value[0] = 0;

        if(obj->direction > 0.0f)
        {
            obj->Idq_out_A.value[1] = obj->startCurrent_A;
            obj->IsRef_A = obj->startCurrent_A;
        }
        else
        {
            obj->Idq_out_A.value[1] = -obj->startCurrent_A;
            obj->IsRef_A = -obj->startCurrent_A;
        }

        obj->angleFOC_rad = obj->angleGen_rad;

        // Check if speed reached force threshold
        if((fabsf(obj->speed_int_Hz) >= obj->speedForce_Hz) &&
                (!obj->flagEnableOpenLoopDebug))
        {
            obj->angleOLDelta_rad = obj->angleFOC_rad - obj->anglePLL_rad;
            obj->motorState = MOTOR_OL_END;
        }
    }
    else
    {
        obj->enableSpeedCtrl = true;
        obj->enableCurrentCtrl = true;
        obj->angleFOC_rad = obj->angleEST_rad;
        obj->motorState = MOTOR_CTRL_RUN;
    }
}

/**
 * @brief Handles open-loop to closed-loop transition state
 * @param obj Pointer to motor control structure
 */
static inline void StateMachine_handleOpenLoopEnd(MOTOR_Vars_t *obj)
{
    float32_t sineAngleOffset = __sin(obj->angleOLDelta_rad);

    // Gradually reduce angle offset
    if(sineAngleOffset > 0)
    {
        obj->angleOLDelta_rad = obj->angleOLDelta_rad - obj->angleOLStep_rad;
    }
    else
    {
        obj->angleOLDelta_rad = obj->angleOLDelta_rad + obj->angleOLStep_rad;
    }

    obj->angleFOC_rad = MATH_incrAngle(obj->anglePLL_rad, obj->angleOLDelta_rad);

    if(fabsf(sineAngleOffset) < __sin(obj->angleOLStep_rad * 5.0f))
    {
        obj->motorState = MOTOR_CTRL_RUN;
        obj->enableSpeedCtrl = true;
        PI_setUi(&obj->pi_spd, (obj->frswPos_sf * obj->Idq_in_A.value[1]));
    }
}

/**
 * @brief Handles closed-loop running state
 * @param obj Pointer to motor control structure
 */
static inline void StateMachine_handleClosedLoopRunning(MOTOR_Vars_t *obj)
{
    TRAJ_run(&obj->traj_spd);
    obj->speed_int_Hz = TRAJ_getIntValue(&obj->traj_spd);

    if(obj->estimatorMode == ESTIMATOR_MODE_FAST)
    {
        // #TAG_FAST_OBS_START#
        obj->angleFOC_rad = obj->angleEST_rad;
        //  #TAG_FAST_OBS_END#
    }
    if(obj->estimatorMode == ESTIMATOR_MODE_ESMO)
    {
        // #TAG_ESMO_START#
        ESMO_updateKslide(obj->esmoHandle);
        obj->angleFOC_rad = obj->anglePLL_rad;
        /// #TAG_ESMO_END#
    }
}

/**
 * @brief Handles flying start (seek position) state
 * @param obj Pointer to motor control structure
 */
static inline void StateMachine_handleFlyingStart(MOTOR_Vars_t *obj)
{
    obj->IsRef_A = 0.0f;
    obj->Idq_out_A.value[0] = 0.0f;
    obj->Idq_out_A.value[1] = 0.0f;
    obj->angleFOC_rad = 0.0;

    if(obj->stateRunTimeCnt > obj->flyingStartTimeDelay)
    {
        obj->stateRunTimeCnt -= obj->flyingStartTimeDelay >> 2;

        if(obj->speedAbs_Hz > obj->speedFlyingStart_Hz)
        {
            obj->enableSpeedCtrl = true;
            obj->speed_int_Hz = obj->speed_Hz;
            TRAJ_setIntValue(&obj->traj_spd, obj->speed_Hz);
            PI_setUi(&obj->pi_spd, obj->Idq_in_A.value[1]);
            obj->motorState = MOTOR_CTRL_RUN;
        }
        else
        {
            if(obj->flagEnableIPD == TRUE)
            {
                obj->ipdHandle->state = IPD_INIT;
                obj->motorState = MOTOR_IPD;
            }
            else
            {
                obj->motorState = MOTOR_ALIGNMENT;
            }
        }
    }
}

/**
 * @brief Main state machine execution - determines FOC angle based on motor state
 * @param obj Pointer to motor control structure
 */
static inline void FOC_runStateMachine(MOTOR_Vars_t *obj)
{
    obj->stateRunTimeCnt++;

    // Execute state-specific logic
    switch(obj->motorState)
    {
        case MOTOR_STOP_IDLE:
            // Do nothing - motor is stopped
            break;

        case MOTOR_FAULT_STOP:
            break;

        case MOTOR_BRAKE_STOP:
            // Handle braking
            if(obj->brakingTimeCnt >= obj->brakingTimeDelay)
            {
                obj->motorState = MOTOR_STOP_IDLE;
            }
            break;

            //#TAG_IPD_START#
        case MOTOR_IPD:
            StateMachine_handleIPD(obj);
            break;
            //#TAG_IPD_END#

        // # TAG_PARAM_ID_START
        case MOTOR_PARAM_IDENT:
            //# TAG_INSTA_SPIN_START
            if(obj->flagMotorTiTuneRun == false)
            {
                StateMachine_handleParameterID(obj);
            }
            //# TAG_INSTA_SPIN_END
            else
            {
                StateMachine_handleTiTune(obj);
            }
            break;
        // # TAG_PARAM_ID_END



        case MOTOR_ALIGNMENT:
            StateMachine_handleAlignment(obj);
            break;

        case MOTOR_OL_START:
            // Run angle generation (for open-loop)
            FOC_runAngleGeneration(obj);
            StateMachine_handleOpenLoopStart(obj);
            break;

        case MOTOR_OL_END:
            // Run angle generation (for open-loop)
            FOC_runAngleGeneration(obj);
            StateMachine_handleOpenLoopEnd(obj);
            break;

        case MOTOR_CL_RUNNING:
        case MOTOR_CTRL_RUN:
            StateMachine_handleClosedLoopRunning(obj);
            break;

        case MOTOR_SEEK_POS:
            StateMachine_handleFlyingStart(obj);
            break;

        default:
            // Handle other states or faults
            break;
    }

    /* Estimated Speed */
    obj->speedAbs_Hz = fabsf(obj->speed_Hz);
}

// ============================================================================
// CONTROLLER FUNCTIONS - STATIC INLINE (for progressive inlining)
// ============================================================================

/**
 * @brief Runs speed controller and current reference generation
 * @param obj Pointer to motor control structure
 */
static inline void FOC_runSpeedController(MOTOR_Vars_t *obj)
{
    USER_Params *objUser = (USER_Params *)(obj->userParamsHandle);

    obj->counterSpeed++;
    if(obj->motorState >= MOTOR_CL_RUNNING)
    {
        if(obj->counterSpeed >= objUser->numCtrlTicksPerSpeedTick)
        {
            obj->counterSpeed = 0;

            if(obj->enableSpeedCtrl == true)
            {
                PI_run(&obj->pi_spd, obj->speed_int_Hz, obj->speed_Hz,
                       (float32_t *)&obj->IsRef_A);
            }
            else if((obj->motorState >= MOTOR_CL_RUNNING) &&
                    (obj->flagMotorIdentified == true))
            {
                if(obj->speed_int_Hz > 0.0f)
                {
                    obj->IsRef_A = obj->IsSet_A;
                }
                else
                {
                    obj->IsRef_A = -obj->IsSet_A;
                }
                PI_setUi(&obj->pi_spd, obj->IsRef_A);
            }
        }
        else if(obj->counterSpeed == 1)
        {
            // FWC/MTPA angle calculation
            float32_t fwcCos = __cos(obj->angleCurrent_rad);
            float32_t fwcSin = __sin(obj->angleCurrent_rad);

            obj->angleCurrent_rad = (obj->angleFWC_rad > obj->angleMTPA_rad) ?
                                     obj->angleFWC_rad : obj->angleMTPA_rad;

            fwcCos = __cos(obj->angleCurrent_rad);
            fwcSin = __sin(obj->angleCurrent_rad);

            if((obj->flagEnableFWC == true) || (obj->flagEnableMTPA == true))
            {
                obj->Idq_out_A.value[0] = obj->IsRef_A * fwcCos;
            }
            obj->Idq_out_A.value[1] = obj->IsRef_A * fwcSin;
        }
    // #TAG_FIELD_WEAKENING_START#
        else if(obj->counterSpeed == 2)
        {
            // Compute output voltage magnitude
            obj->Vs_V = __sqrt((obj->Vdq_out_V.value[0] * obj->Vdq_out_V.value[0]) +
                              (obj->Vdq_out_V.value[1] * obj->Vdq_out_V.value[1]));
            obj->VsRef_V = obj->VsRef_pu * obj->adcData->VdcBus_V;
        }
        else if(obj->counterSpeed == 3)
        {
            // Field Weakening Control
            if(obj->flagEnableFWC == true)
            {
                float32_t angleFWC;
                PI_run(&obj->pi_fwc, obj->VsRef_V, obj->Vs_V, (float32_t*)&angleFWC);
                obj->angleFWC_rad = MATH_PI_OVER_TWO - angleFWC;
            }
            else
            {
                PI_setUi(&obj->pi_fwc, 0.0f);
                obj->angleFWC_rad = MATH_PI_OVER_TWO;
            }
        }
    }
// #TAG_FIELD_WEAKENING_END#

//#TAG_MTPA_START#
    else if(obj->counterSpeed == 4)
    {
        // MTPA (Maximum Torque Per Ampere)
        if(obj->flagEnableMTPA == true)
        {
            obj->angleMTPA_rad = MTPA_computeCurrentAngle(obj->mtpaHandle, obj->IsRef_A);
        }
        else
        {
            obj->angleMTPA_rad = MATH_PI_OVER_TWO;
        }
    }
//#TAG_MTPA_END#
}

/**
 * @brief Runs current controllers (Id and Iq PI controllers)
 * @param obj Pointer to motor control structure
 */
static inline void FOC_runCurrentControllers(MOTOR_Vars_t *obj)
{
    USER_Params *objUser = (USER_Params *)(obj->userParamsHandle);

    obj->IdqRef_A.value[0] = obj->Idq_out_A.value[0] + obj->IdRated_A;

    //TODO: FAST Lib Dependency
//    //#TAG_FAST_OBS_START#
//    EST_updateId_ref_A(obj->estHandle, &obj->IdqRef_A.value[0]);
//    //#TAG_FAST_OBS_END#

    obj->IdqRef_A.value[1] = obj->Idq_out_A.value[1];

    if(obj->enableCurrentCtrl == true)
    {
        // Maximum voltage output
        objUser->maxVsMag_V = objUser->maxVsMag_pu * obj->adcData->VdcBus_V;

        PI_setMinMax(&obj->pi_Id, -objUser->maxVsMag_V, objUser->maxVsMag_V);

        // Run Id controller
        PI_run(&obj->pi_Id, obj->IdqRef_A.value[0], obj->Idq_in_A.value[0],
               (float32_t*)&obj->Vdq_out_V.value[0]);

        // Calculate Iq controller limits (circular limit)
        float32_t outMax_V = __sqrt((objUser->maxVsMag_V * objUser->maxVsMag_V) -
                                   (obj->Vdq_out_V.value[0] * obj->Vdq_out_V.value[0]));

        PI_setMinMax(&obj->pi_Iq, -outMax_V, outMax_V);

        // Run Iq controller
        PI_run(&obj->pi_Iq, obj->IdqRef_A.value[1], obj->Idq_in_A.value[1],
               (float32_t*)&obj->Vdq_out_V.value[1]);


        //#TAG_DEAD_TIME_COMP_START#
        // Dead time compensation
        if(obj->deadTimeCompHandle->DTC_Enable == true)
        {
            deadTimeCompRun(obj->deadTimeCompHandle, &obj->Idq_in_A);
            obj->Vdq_out_V.value[0] += obj->deadTimeCompHandle->Vdq_DTC_V.value[0];
            obj->Vdq_out_V.value[1] += obj->deadTimeCompHandle->Vdq_DTC_V.value[1];
        }
        //#TAG_DEAD_TIME_COMP_END#

        //TODO: FAST Lib Dependency
//        //#TAG_FAST_OBS_START#
//        // Set reference values in estimator
//        EST_setId_ref_A(obj->estHandle, obj->IdqRef_A.value[0]);
//        EST_setIq_ref_A(obj->estHandle, obj->IdqRef_A.value[1]);
//        //#TAG_FAST_OBS_END#
    }
    if(obj->forceOpenLoopVoltageMode)
    {
        obj->Vdq_out_V.value[0] = obj->Vdq_OpenLoop.value[0];
        obj->Vdq_out_V.value[1] = obj->Vdq_OpenLoop.value[1];
    }
}

// ============================================================================
// MONOLITHIC VERSION - MAXIMUM PERFORMANCE (STATIC INLINE)
// ============================================================================

/**
 * @brief FULLY MONOLITHIC FOC control loop - NO FUNCTION CALLS
 *
 * This version expands ALL logic inline with NO function call overhead.
 * Similar to the fast motor1_drive.c implementation that achieves 700ns (16µs ISR).
 *
 * Being static inline in the header forces the compiler to inline it directly
 * at the call site in the ISR, eliminating ALL function call overhead.
 *
 * Use this for performance comparison to measure the impact of function calls.
 *
 * @param motorHandle Handle to motor control structure
 */
static inline void application_full_run(const MOTOR_Handle motorHandle)
{
    // ========================================================================
    // DEREFERENCE HANDLE ONCE - Use direct pointer throughout
    // ========================================================================
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)motorHandle;
    USER_Params *objUser = (USER_Params *)(obj->userParamsHandle);

    // Cache frequently accessed pointers
    HAL_ADCData_t *adcData = obj->adcData;
    HAL_PWMData_t *pwmData = obj->pwmData;
    MATH_Vec3 *pwmVabc_pu = obj->pwmVabc_pu;

    obj->ISRCount++;

    // ========================================================================
    // SECTION 1: FOC FEEDBACK - FULLY INLINED
    // ========================================================================

    // Acknowledge ADC interrupt
    HAL_ackMtrADCInt(&obj->halInterface->halFOCConfigHandle->intCfg);

    // Read three-phase currents
    HAL_ReadPhaseCurrents(obj->halInterface);

    // Read DC bus voltage and three-phase voltages
    HAL_ReadInverterVoltages(obj->halInterface);

    // Voltage reconstruction
    VOLREC_run(obj->volrecHandle, adcData->VdcBus_V, pwmVabc_pu, &(obj->estInputData.Vab_V));

    // Clarke transform on currents (abc -> alpha-beta)
    CLARKE_run(&obj->clarke_I, &adcData->I_A, &obj->estInputData.Iab_A);

    obj->oneOverDcBus_invV = 1.0f / adcData->VdcBus_V;

    // ========================================================================
    // SECTION 2: OBSERVERS - FULLY INLINED
    // ========================================================================

    if(obj->estimatorMode == ESTIMATOR_MODE_FAST)
    {
        //TODO: FAST Lib Dependency
        //#TAG_FAST_OBS_START#
//        // FAST Estimator - INLINED
//        obj->estInputData.dcBus_V = adcData->VdcBus_V;
//        obj->estInputData.speed_ref_Hz = obj->speed_int_Hz;
//
//        EST_run(obj->estHandle, &obj->estInputData, &obj->estOutputData);
//
//        obj->angleDelta_rad = objUser->angleDelayed_sf_sec * obj->estOutputData.fm_lp_rps;
//        obj->angleEST_rad = MATH_incrAngle(obj->estOutputData.angle_rad, obj->angleDelta_rad);
//        obj->speedEST_Hz = EST_getFm_lp_Hz(obj->estHandle);
//        obj->oneOverDcBus_invV = obj->estOutputData.oneOverDcBus_invV;
//
//        obj->speed_Hz = obj->speedEST_Hz;
//        obj->angleFOC_rad = obj->angleEST_rad;
        //#TAG_FAST_OBS_END#
    }
    else
    {
        // ESMO Estimator - INLINED
        ESMO_setSpeedRef(obj->esmoHandle, obj->speed_int_Hz);
        ESMO_run(obj->esmoHandle, adcData->VdcBus_V, pwmVabc_pu, &(obj->estInputData.Iab_A));

        obj->anglePLLComp_rad = obj->speedPLL_Hz * obj->angleDelayed_sf;
        obj->anglePLL_rad = MATH_incrAngle(ESMO_getAnglePLL(obj->esmoHandle), obj->anglePLLComp_rad);

        SPDFR_run(&obj->spdfr, obj->anglePLL_rad);
        obj->speedPLL_Hz = SPDFR_getSpeedHz(&obj->spdfr);

        obj->speed_Hz = obj->speedPLL_Hz;
        obj->angleFOC_rad = obj->anglePLL_rad;
    }

    // ========================================================================
    // SECTION 3: STATE MACHINE - Keep as function call (not expanded)
    // ========================================================================


    // Call the state machine function - keeps code maintainable
    // State machine overhead is minimal compared to critical path
    FOC_runStateMachine(obj);

    // ========================================================================
    // SECTION 4: FOC FEEDFORWARD - FULLY INLINED (CRITICAL PATH)
    // ========================================================================

    // -------------------------------------------------------------------------
    // COMPUTE PHASOR ONCE - INLINED
    // -------------------------------------------------------------------------
    MATH_Vec2 Phasor;
//    float32_t cosTh = __cos(obj->angleFOC_rad);
//    float32_t sinTh = __sin(obj->angleFOC_rad);
//    obj->phasor.value[0] = cosTh;
//    obj->phasor.value[1] = sinTh;
//
    Phasor.value[0] = __cos(obj->angleFOC_rad);
    Phasor.value[1] = __sin(obj->angleFOC_rad);

    // -------------------------------------------------------------------------
    // PARK TRANSFORM - INLINED
    // -------------------------------------------------------------------------
    PARK_setPhasor(&obj->park_I, &Phasor);
    PARK_run(&obj->park_I, &(obj->estInputData.Iab_A), (MATH_vec2 *)&(obj->Idq_in_A));

    // -------------------------------------------------------------------------
    // SPEED CONTROLLER - INLINED
    // -------------------------------------------------------------------------
    obj->counterSpeed++;

    if(obj->counterSpeed >= objUser->numCtrlTicksPerSpeedTick)
    {
        obj->counterSpeed = 0;

        if(obj->enableSpeedCtrl == true)
        {
            PI_run(&obj->pi_spd, obj->speed_int_Hz, obj->speed_Hz, (float32_t *)&obj->IsRef_A);
        }
        else if((obj->motorState >= MOTOR_CL_RUNNING) && (obj->flagMotorIdentified == true))
        {
            obj->IsRef_A = (obj->speed_int_Hz > 0.0f) ? obj->IsSet_A : -obj->IsSet_A;
            PI_setUi(&obj->pi_spd, obj->IsRef_A);
        }
    }
    else if(obj->counterSpeed == 1)
    {
        // FWC/MTPA angle calculation
        obj->angleCurrent_rad = (obj->angleFWC_rad > obj->angleMTPA_rad) ?
                                 obj->angleFWC_rad : obj->angleMTPA_rad;

        float32_t fwcCos = __cos(obj->angleCurrent_rad);
        float32_t fwcSin = __sin(obj->angleCurrent_rad);

        if((obj->flagEnableFWC == true) || (obj->flagEnableMTPA == true))
        {
            obj->Idq_out_A.value[0] = obj->IsRef_A * fwcCos;
        }
        obj->Idq_out_A.value[1] = obj->IsRef_A * fwcSin;
    }
    else if(obj->counterSpeed == 2)
    {
        // Compute output voltage magnitude
        obj->Vs_V = __sqrt((obj->Vdq_out_V.value[0] * obj->Vdq_out_V.value[0]) +
                          (obj->Vdq_out_V.value[1] * obj->Vdq_out_V.value[1]));
        obj->VsRef_V = obj->VsRef_pu * adcData->VdcBus_V;
    }
    else if(obj->counterSpeed == 3)
    {
        // Field Weakening Control
        if(obj->flagEnableFWC == true)
        {
            float32_t angleFWC;
            PI_run(&obj->pi_fwc, obj->VsRef_V, obj->Vs_V, (float32_t*)&angleFWC);
            obj->angleFWC_rad = MATH_PI_OVER_TWO - angleFWC;
        }
        else
        {
            PI_setUi(&obj->pi_fwc, 0.0f);
            obj->angleFWC_rad = MATH_PI_OVER_TWO;
        }
    }
    else if(obj->counterSpeed == 4)
    {
        // MTPA
        if(obj->flagEnableMTPA == true)
        {
            obj->angleMTPA_rad = MTPA_computeCurrentAngle(obj->mtpaHandle, obj->IsRef_A);
        }
        else
        {
            obj->angleMTPA_rad = MATH_PI_OVER_TWO;
        }
    }

    // -------------------------------------------------------------------------
    // CURRENT CONTROLLERS - INLINED
    // -------------------------------------------------------------------------
    obj->IdqRef_A.value[0] = obj->Idq_out_A.value[0] + obj->IdRated_A;
//    EST_updateId_ref_A(obj->estHandle, &obj->IdqRef_A.value[0]);
    obj->IdqRef_A.value[1] = obj->Idq_out_A.value[1];

    if(obj->enableCurrentCtrl == true)
    {
        // Maximum voltage output
        objUser->maxVsMag_V = objUser->maxVsMag_pu * adcData->VdcBus_V;

        PI_setMinMax(&obj->pi_Id, -objUser->maxVsMag_V, objUser->maxVsMag_V);
        PI_run(&obj->pi_Id, obj->IdqRef_A.value[0], obj->Idq_in_A.value[0],
               (float32_t*)&obj->Vdq_out_V.value[0]);

        // Calculate Iq controller limits (circular limit)
        float32_t outMax_V = __sqrt((objUser->maxVsMag_V * objUser->maxVsMag_V) -
                                   (obj->Vdq_out_V.value[0] * obj->Vdq_out_V.value[0]));

        PI_setMinMax(&obj->pi_Iq, -outMax_V, outMax_V);
        PI_run(&obj->pi_Iq, obj->IdqRef_A.value[1], obj->Idq_in_A.value[1],
               (float32_t*)&obj->Vdq_out_V.value[1]);

        // Dead time compensation
        if(obj->deadTimeCompHandle->DTC_Enable == true)
        {
            deadTimeCompRun(obj->deadTimeCompHandle, &obj->Idq_in_A);
            obj->Vdq_out_V.value[0] += obj->deadTimeCompHandle->Vdq_DTC_V.value[0];
            obj->Vdq_out_V.value[1] += obj->deadTimeCompHandle->Vdq_DTC_V.value[1];
        }

        //TODO: FAST Lib Dependency
        //#TAG_FAST_OBS_START#
//        EST_setId_ref_A(obj->estHandle, obj->IdqRef_A.value[0]);
//        EST_setIq_ref_A(obj->estHandle, obj->IdqRef_A.value[1]);
        //#TAG_FAST_OBS_END#
    }

    // =========================================================================
    // CRITICAL SECTION: INVERSE PARK + SVGEN - FULLY INLINED (700ns section)
    // =========================================================================

    // -------------------------------------------------------------------------
    // INVERSE PARK TRANSFORM - INLINED (reuses cosTh, sinTh from above)
    // -------------------------------------------------------------------------
    IPARK_setPhasor(&obj->ipark_V, &Phasor);
    IPARK_run(&obj->ipark_V, &obj->Vdq_out_V, &obj->Vab_out_V);

    // -------------------------------------------------------------------------
    // SPACE VECTOR GENERATION - INLINED (uses cached pwmVabc_pu pointer)
    // -------------------------------------------------------------------------
    SVGEN_setup(&obj->svgen, obj->oneOverDcBus_invV);
    SVGEN_run(&obj->svgen, &obj->Vab_out_V, pwmVabc_pu);

    // Clear PWM data if PWMs are disabled
    if(HAL_getPwmEnableStatus(pwmData) == false)
    {
        pwmVabc_pu->value[0] = 0.0f;
        pwmVabc_pu->value[1] = 0.0f;
        pwmVabc_pu->value[2] = 0.0f;
    }


    // Write PWM values to hardware
    HAL_SetPhaseVoltages(obj->halInterface);

    collectRMSData(obj);

}

#endif
