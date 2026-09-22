
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

//
// REFACTORED APPLICATION.C WITH MODULAR ARCHITECTURE
// This file demonstrates the clean separation of FOC control into modular functions
// ARM M33 compatible - uses TMU math functions via fastrts_tmu.h
//

#include <application.h>
#include "ipd.h"              // Needed for IPD functions
#include "motor_common.h"     // Needed for motor control functions
#include "user.h"
#include "zephyr/kernel.h"
#include <string.h>

#define BUFFER_LENGTH 50
volatile uint8_t buffer[BUFFER_LENGTH];
volatile uint32_t data1, data2;
volatile uint16_t idx = 10;

extern bool tx_complete;

volatile uint32_t application_bufupdate_count = 0;
volatile uint32_t application_bufwait_count = 0;

// ============================================================================
// SECTION 1: FOC FEEDBACK - Input Acquisition and Transforms
// ============================================================================

/**
 * @brief Acquires sensor data and performs feedback transforms (Clarke on currents/voltages)
 * @param motorHandle Handle to motor control structure
 *
 * This function handles:
 * - ADC interrupt acknowledgment
 * - Three-phase current sensing
 * - DC bus and phase voltage sensing
 * - Voltage reconstruction (if phase sensing unavailable)
 * - Clarke transform on currents (abc -> alpha-beta)
 */
static inline void FOC_runFeedback(MOTOR_Vars_t *obj)
{

    // Read three-phase currents
    HAL_ReadPhaseCurrents(obj->halInterface);


    // #TAG_FAST_OBS_START#

    // #TAG_READ_INV_VOLT_START#

    // Read DC bus voltage and three-phase voltages
    HAL_ReadInverterVoltages(obj->halInterface);

    // #TAG_READ_INV_VOLT_END#

    // #TAG_RECONST_INV_VOLT_START#

    // Reconstruct voltage if phase voltage sensing is not available

    VOLREC_run(obj->volrecHandle, obj->adcData->VdcBus_V,
               obj->pwmVabc_pu,
               &(obj->estInputData.Vab_V));

    // #TAG_RECONST_INV_VOLT_END#
    // #TAG_FAST_OBS_END#

    // Clarke transform on currents (abc -> alpha-beta)
    CLARKE_run(&obj->clarke_I, &obj->adcData->I_A,
               &obj->estInputData.Iab_A);

    obj->oneOverDcBus_invV = 1.0f / obj->adcData->VdcBus_V;

}

// ============================================================================
// SECTION 2: OBSERVER/ESTIMATOR - Angle and Speed Estimation
// ============================================================================

/**
 * @brief Runs FAST estimator for angle and speed estimation
 * @param motorHandle Handle to motor control structure
 */
static inline void FOC_runFASTEstimator(MOTOR_Vars_t *obj)
{
    USER_Params *objUser = (USER_Params *)(obj->userParamsHandle);

    obj->estInputData.dcBus_V = obj->adcData->VdcBus_V;

    obj->estInputData.speed_ref_Hz = obj->speed_int_Hz;

    // Run FAST estimator
    EST_run(obj->estHandle, &obj->estInputData, &obj->estOutputData);

    // Compute angle with delay compensation
    obj->angleDelta_rad = objUser->angleDelayed_sf_sec * obj->estOutputData.fm_lp_rps;
    obj->angleEST_rad = MATH_incrAngle(obj->estOutputData.angle_rad, obj->angleDelta_rad);

    // Get estimated speed
    obj->speedEST_Hz = EST_getFm_lp_Hz(obj->estHandle);
    obj->oneOverDcBus_invV = obj->estOutputData.oneOverDcBus_invV;

    obj->speed_Hz = obj->speedEST_Hz;
    obj->angleFOC_rad = obj->angleEST_rad;

    obj->IdRated_A = EST_getIdRated_A(obj->estHandle);

}

/**
 * @brief Runs ESMO (Enhanced Sliding Mode Observer) for sensorless control
 * @param motorHandle Handle to motor control structure
 */
static inline void FOC_runESMOEstimator(MOTOR_Vars_t *obj)
{
    // Set speed reference for ESMO
    ESMO_setSpeedRef(obj->esmoHandle, obj->speed_int_Hz);

    ESMO_run(obj->esmoHandle, obj->adcData->VdcBus_V,
             obj->pwmVabc_pu,
             &(obj->estInputData.Iab_A));

    // Compute PLL angle with delay compensation
    obj->anglePLLComp_rad = obj->speedPLL_Hz * obj->angleDelayed_sf;
    obj->anglePLL_rad = MATH_incrAngle(ESMO_getAnglePLL(obj->esmoHandle), obj->anglePLLComp_rad);

    // Run speed calculation from PLL angle
    SPDFR_run(&obj->spdfr, obj->anglePLL_rad);
    obj->speedPLL_Hz = SPDFR_getSpeedHz(&obj->spdfr);

    obj->speed_Hz = obj->speedPLL_Hz;
    obj->angleFOC_rad = obj->anglePLL_rad;

}

/**
 * @brief Runs both estimators (FAST and ESMO) and manages speed references
 * @param motorHandle Handle to motor control structure
 */
static inline void FOC_runObservers(MOTOR_Vars_t *obj)
{

    if(obj->estimatorMode == ESTIMATOR_MODE_FAST)
    {

        // #TAG_FAST_OBS_START#
        // Start Tag used to identify FAST - Observer Code

        //TODO: FAST Lib Dependency
        //FOC_runFASTEstimator(obj);

        // End Tag used to identify FAST - Observer Code
        // #TAG_FAST_OBS_END#   End Tag used to identify FAST - Observer Code

    }
    else
    {
        // #TAG_ESMO_START#
        // Start Tag used to identify ESMO - Observer Code

        FOC_runESMOEstimator(obj);

        // End Tag used to identify FAST - Observer Code
        // #TAG_ESMO_START#
    }

}

// ============================================================================
// SECTION 4: FOC FEEDFORWARD - Control and PWM Generation
// ============================================================================

/**
 * @brief Computes sin/cos phasor from FOC angle
 * @param motorHandle Handle to motor control structure
 *
 * This function calculates sine and cosine of the FOC angle.
 * Computing it once and reusing for both Park and iPark eliminates redundant calculations.
 */
static inline void FOC_computePhasor(MOTOR_Vars_t *obj)
{

    // Compute sin/cos phasor for Park/iPark transforms
    obj->phasor.value[0] = __cos(obj->angleFOC_rad);
    obj->phasor.value[1] = __sin(obj->angleFOC_rad);
}

/**
 * @brief Runs Park transform on currents (alpha-beta -> dq)
 * @param motorHandle Handle to motor control structure
 *
 */
static inline void FOC_runParkTransform(MOTOR_Vars_t *obj)
{

    // Set phasor and run Park transform (phasor already computed)
    PARK_setPhasor(&obj->park_I, &obj->phasor);
    PARK_run(&obj->park_I, &(obj->estInputData.Iab_A),
             (MATH_vec2 *)&(obj->Idq_in_A));
}

/**
 * @brief Runs inverse Park transform and SVPWM generation
 * @param motorHandle Handle to motor control structure
 */
static inline void FOC_runPWMGeneration(MOTOR_Vars_t *obj)
{
    // Set phasor for inverse Park transform
    IPARK_setPhasor(&obj->ipark_V, &obj->phasor);

    // Run inverse Park transform (dq -> alpha-beta)
    IPARK_run(&obj->ipark_V, &obj->Vdq_out_V, &obj->Vab_out_V);

    if(obj->flagMotorTiTuneRun)
    {
        /* Link the output Alpha Beta Voltages */
        obj->Vab_out_V.value[0] = obj->tiTuneHandle->Ti_Tune_Out_Vab_V.value[0];

        obj->Vab_out_V.value[1] = obj->tiTuneHandle->Ti_Tune_Out_Vab_V.value[1];
    }
    // Setup and run space vector generator
    SVGEN_setup(&obj->svgen, obj->oneOverDcBus_invV);
    SVGEN_run(&obj->svgen, &obj->Vab_out_V, obj->pwmVabc_pu);

    // Clear PWM data if PWMs are disabled
    if(HAL_getPwmEnableStatus(obj->pwmData) == false)
    {
        obj->pwmVabc_pu->value[0] = 0.0f;
        obj->pwmVabc_pu->value[1] = 0.0f;
        obj->pwmVabc_pu->value[2] = 0.0f;
    }

    // Write PWM values to hardware
    HAL_SetPhaseVoltages(obj->halInterface);
}

/**
 * @brief Main FOC feedforward execution
 * @param motorHandle Handle to motor control structure
 *
 * OPTIMIZATION: Phasor computed ONCE and reused for both Park and iPark transforms
 */
static inline void FOC_runFeedforward(MOTOR_Vars_t *obj)
{

    // Compute sin/cos phasor ONCE for both Park and iPark transforms
    FOC_computePhasor(obj);

    // Run Park transform on currents (uses phasor computed above)
    FOC_runParkTransform(obj);

    // Run speed controller and generate current references
    FOC_runSpeedController(obj);

    // Run current controllers
    FOC_runCurrentControllers(obj);

    // Run inverse Park and PWM generation (reuses same phasor)
    FOC_runPWMGeneration(obj);

}

// ============================================================================
// SECTION 5: DATA LOGGING AND DIAGNOSTICS
// ============================================================================

/**
 * @brief Handles RMS data collection for power calculations
 * @param motorHandle Handle to motor control structure
 */
static inline void FOC_collectRMSData(MOTOR_Vars_t *obj)
{
    collectRMSData((MOTOR_Handle)obj);
}

/**
 * @brief Handles datalog updates via DMA
 * @param motorHandle Handle to motor control structure
 */
static inline void FOC_updateDatalog(MOTOR_Vars_t *obj)
{
    if(DATALOGIF_enable(datalogHandle) == true)
    {
        DATALOGIF_updateWithDMA(datalogHandle);

        // Force trigger DMA channels to save data
        HAL_trigDMAforDLOG(halHandle, 0);
        HAL_trigDMAforDLOG(halHandle, 1);
    }
}

/**
 * @brief Handles serial plot UART updates (diagnostic output)
 * @param motorHandle Handle to motor control structure
 *
 * NOTE: This should ideally be moved to background loop for better ISR performance
 */
static inline void FOC_updateSerialPlot(SYSTEM_Vars_t *obj)
{
    static uint8_t plotCount = 1;

    if(plotCount >= 1)
    {

        memcpy((void *)&data1,(void *) obj->dacCtrl1.pDacOutAddr, sizeof(float));
        memcpy((void *)&data2,(void *) obj->dacCtrl2.pDacOutAddr, sizeof(float));

//
//        data1 = (uint32_t)1024;
//        data2 = (uint32_t)2048;
//
//        if(idx >=4)
//        {
//          buffer[0] = data1 & 0xFF;
//          buffer[1] = data1 >> 8 & 0xFF;
//          buffer[2] = data1 >> 16 & 0xFF;
//          buffer[3] = data1 >> 24 & 0xFF;
//          idx = 0;
//        }
//
        if(tx_complete == true)
        {
            tx_complete = false;

// #ifdef SEND_BINARY_DATA
            buffer[0] = 0xAA;
            buffer[1] = 0x55;
            buffer[2] = data1 & 0xFF;
            buffer[3] = data1 >> 8 & 0xFF;
            buffer[4] = data1 >> 16 & 0xFF;
            buffer[5] = data1 >> 24 & 0xFF;
            buffer[6] = data2 & 0xFF;
            buffer[7] = data2 >> 8 & 0xFF;
            buffer[8] = data2 >> 16 & 0xFF;
            buffer[9] = data2 >> 24 & 0xFF;

// #else
            //idx = 0;


            // float data1_f, data2_f;
            // memcpy(&data1_f, (void *)obj->dacCtrl1.pDacOutAddr, sizeof(float));
            //memcpy(&data2_f, (void *)obj->dacCtrl2.pDacOutAddr, sizeof(float));

            // snprintf(buffer, BUFFER_LENGTH, "0: %2.2f, %2.2f\n", (float)*(obj->dacCtrl1.pDacOutAddr), (float)*(obj->dacCtrl2.pDacOutAddr));
// #endif

            application_bufupdate_count++;

            //printk("0: %2.2f\n", data1_f);
            //printk("1: %2.2f\r\n", data2_f);

            // k_sem_give(&buf_ready_sem);
        } else {
            application_bufwait_count++;
        }

//       SCI_writeBytesNonBlocking(SERIAL_DATA_PLOT_INST, &buffer[0], &idx, 10);
        plotCount = 0;
    }
    else
    {
        plotCount++;
    }
}

/**
 * @brief Handles DAC output updates for oscilloscope monitoring
 * @param motorHandle Handle to motor control structure
 *
 * NOTE: This should ideally be moved to background loop for better ISR performance
 */
static inline void FOC_updateDACOutputs(SYSTEM_Vars_t *obj)
{

    // DAC Channel 1
    if(obj->dacCtrl1.dacEn == 1)
    {
        if(obj->dacCtrl1.mode == 0)  // Bipolar
        {
            obj->dacCtrl1.dacData = (uint32_t)roundf(
                (*obj->dacCtrl1.pDacOutAddr * obj->dacCtrl1.dacScalingFactor) + 2048.0f);
        }
        else  // Unipolar
        {
            obj->dacCtrl1.dacData = (uint32_t)(*obj->dacCtrl1.pDacOutAddr *
                                               obj->dacCtrl1.dacScalingFactor);
        }
        if(obj->dacCtrl1.dacData < 4095 && obj->dacCtrl1.dacData > 0)
        {
            // DL_CMPSSLITE_setDACValueLow(FOC_DAC_OUT_INST, obj->dacCtrl1.dacData);
        }
    }

    // DAC Channel 2
    if(obj->dacCtrl2.dacEn == true)
    {
        if(obj->dacCtrl2.mode == 0)  // Bipolar
        {
            obj->dacCtrl2.dacData = (uint32_t)roundf(
                (*obj->dacCtrl2.pDacOutAddr * obj->dacCtrl2.dacScalingFactor) + 2048.0f);
        }
        else  // Unipolar
        {
            obj->dacCtrl2.dacData = (uint32_t)(*obj->dacCtrl2.pDacOutAddr *
                                               obj->dacCtrl2.dacScalingFactor);
        }
        //TODO : Syscfg Dependency
//        DL_CMPSSLITE_setDACValueLow(FOC_DAC_OUT_INST, obj->dacCtrl2.dacData);
    }
}

/**
 * @brief Main data logging and diagnostics execution
 * @param motorHandle Handle to motor control structure
 */
static inline void FOC_runDataLogging(MOTOR_Vars_t *obj)
{

    // Collect RMS data
    FOC_collectRMSData(obj);

//    // Update datalog via DMA
//    FOC_updateDatalog(obj);
//

//
    // Update DAC outputs (should be moved to background)
    FOC_updateDACOutputs(&systemVars);

    //    // Update serial plot (should be moved to background)
    FOC_updateSerialPlot(&systemVars);
}

// ============================================================================
// MAIN APPLICATION RUN - REFACTORED WITH CLEAN MODULAR STRUCTURE
// ============================================================================

/**
 * @brief Main FOC control loop - REFACTORED VERSION
 *
 * This is the high-priority ISR function that executes every PWM cycle (10-20 kHz).
 * It has been refactored into clean, modular sections:
 *
 * 1. FOC Feedback      - Input acquisition and Clarke transform
 * 2. Observers         - FAST and ESMO estimators for angle/speed
 * 3. State Machine     - Motor state management and transitions
 * 4. FOC Feedforward   - Park transform, controllers, inverse Park, SVPWM
 * 5. Data Logging      - RMS collection, datalog, diagnostics
 *
 * @param motorHandle Handle to motor control structure
 */
void application_run(MOTOR_Handle motorHandle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)motorHandle;

    obj->ISRCount++;

    // ========================================================================
    // SECTION 1: FOC FEEDBACK - Input Acquisition
    // ========================================================================
    FOC_runFeedback(obj);

//    HAL_setGPIOHigh((uint32_t)MTR1_FOC_CTRL_ISR_TIMING_PORT,MTR1_FOC_CTRL_ISR_TIMING_PIN);

    // ========================================================================
    // SECTION 2: OBSERVERS - Angle and Speed Estimation
    // ========================================================================
    FOC_runObservers(obj);

//    HAL_setGPIOLow((uint32_t)MTR1_FOC_CTRL_ISR_TIMING_PORT,MTR1_FOC_CTRL_ISR_TIMING_PIN);

    // ========================================================================
    // SECTION 3: STATE MACHINE - Motor State Management
    // ========================================================================
    FOC_runStateMachine(obj);


    // ========================================================================
    // SECTION 4: FOC FEEDFORWARD - Control and PWM Generation
    // ========================================================================
    FOC_runFeedforward(obj);


    // ========================================================================
    // SECTION 5: DATA LOGGING - Diagnostics and Monitoring
    // ========================================================================
    FOC_runDataLogging(obj);
}

/**
 * @brief Handles motor Fault Stop Exit condition
 * @param motorHandle Handle to motor control structure
 */
static inline void LowPriority_handleMotorFaults(MOTOR_Handle motorHandle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)motorHandle;
    if(obj->flagClearFaults == true)
    {
        HAL_clearMtrFaultStatus(obj->halInterface);
        obj->faultMtrNow.all &= MTR_FAULT_CLEAR;
        obj->flagClearFaults = false;

        obj->motorState = MOTOR_NORM_STOP;
    }
    HAL_disablePWM(&obj->halInterface->halFOCConfigHandle->pwmCfg,
                  &obj->halInterface->halFOCDataHandle->pwmData);
}
/**
 * @brief Handles motor trip fault checking
 * @param motorHandle Handle to motor control structure
 */
static inline void LowPriority_handleMotorCheckFaults(MOTOR_Handle motorHandle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)motorHandle;
    MOTOR_SetVars_t *objSets = (MOTOR_SetVars_t *)(motorHandle->motorSetsHandle);

    /* check for Motor Trip Fault Status */
    if(HAL_getPwmEnableStatus(&obj->halInterface->halFOCDataHandle->pwmData) == true)
    {
        if((HAL_getMtrTripFaults(&obj->halInterface->halFOCConfigHandle->pwmCfg) != 0) &&
                (obj->motorState != MOTOR_IPD))
        {
            obj->faultMtrNow.bit.moduleOverCurrent = 1;
        }

    }

    if((HAL_getGDTripFaults(&obj->halInterface->halFOCConfigHandle->pwmCfg) != 0))
    {
        obj->faultMtrNow.bit.gateDriverFault = 1;
    }

    obj->faultMtrPrev.all |= obj->faultMtrNow.all;
    obj->faultMtrUse.all = obj->faultMtrNow.all & obj->faultMtrMask.all;


    // Had some faults to stop the motor
    if(obj->faultMtrUse.all != 0)
    {
        obj->motorState = MOTOR_FAULT_STOP;

        obj->stopWaitTimeCnt = objSets->restartWaitTimeSet;
    }
    //TODO: Check for the Restart logic..
//        obj->restartTimesCnt++;
//
//        if(obj->flagEnableRestart == false)
//        {
//            obj->stopWaitTimeCnt = 0;
//        }
//
//        if(obj->stopWaitTimeCnt == 0)
//        {
//            if(obj->restartTimesCnt < objSets->restartTimesSet)
//            {
//                obj->flagClearFaults = 1;
//            }
//            else
//            {
//                obj->flagEnableRunAndIdentify = false;
//            }
//        }
//    }
    if(obj->flagClearFaults == true)
    {
        HAL_clearMtrFaultStatus(obj->halInterface);
        obj->faultMtrNow.all &= MTR_FAULT_CLEAR;
        obj->flagClearFaults = false;
        obj->motorState = MOTOR_NORM_STOP;

    }
}

/**
 * @brief Handles Normal Stop Operation
 * @param motorHandle Handle to motor control structure
 */
static inline void LowPriority_handleMotorCheckStop(MOTOR_Handle motorHandle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)motorHandle;
    MOTOR_SetVars_t *objSets = (MOTOR_SetVars_t *)(motorHandle->motorSetsHandle);

    /* If Stop Motor Command is issued, move the motor to Stop State */
    if((obj->flagEnableRunAndIdentify == false) && ( obj->motorState >= MOTOR_SEEK_POS))
    {

        if(obj->flagEnableFlyingStart == false)
        {
            obj->stopWaitTimeCnt = objSets->stopWaitTimeSet;
        }
        else
        {
            obj->stopWaitTimeCnt = 0;
        }

        obj->motorState = MOTOR_NORM_STOP;

    }
}
/**
 * @brief Handles fault detection and motor restart logic
 * @param motorHandle Handle to motor control structure
 */
static inline void LowPriority_handleMotorRestart(MOTOR_Handle motorHandle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)motorHandle;

    if(motorHandle->flagEnableRunAndIdentify == true)
    {
        if(HAL_getPwmEnableStatus(&obj->halInterface->halFOCDataHandle->pwmData) == false)
        {
            // enable the PWM
            HAL_enablePWM(&obj->halInterface->halFOCConfigHandle->pwmCfg,
                         &obj->halInterface->halFOCDataHandle->pwmData);
        }


        restartMotorControl(motorHandle);

        //TODO: Move to Estimator routine

        //TODO: FAST Lib Dependency
        // enable the estimator
        //#TAG_FAST_OBS_START#
//        EST_enable(obj->estHandle);
//
//        // enable the trajectory generator
//        EST_enableTraj(obj->estHandle);
        //#TAG_FAST_OBS_END#
    }


}

/**
 * @brief Handles estimator and PWM enable logic
 * @param motorHandle Handle to motor control structure
 */
static inline void LowPriority_runInstaSpinParameterID(MOTOR_Handle motorHandle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)motorHandle;

    USER_Params *objUser = (USER_Params *)(motorHandle->userParamsHandle);

    //TODO : Identify the purpose and requirement
    EST_updateTrajState(obj->estHandle);

    if(obj->flagEstStateChanged == true)
    {
        //During InstaSpin based Motor Parameter Identification,
        // configure the trajectory generator
        EST_configureTraj(obj->estHandle);

            // configure the controllers
        EST_configureTrajState(obj->estHandle, obj->userParamsHandle,
                               &obj->pi_spd,
                               &obj->pi_Id, &obj->pi_Iq);

        /* Exit condition for Instaspin based Motor Parameter Identification */
        /* Move to Parameter Identification */
        if(objUser->flag_bypassMotorId == false)
        {
            if(((EST_isMotorIdentified(obj->estHandle) == true)
                      && (EST_isIdle(obj->estHandle) == true)))
            {
                if(EST_isMotorIdentified(obj->estHandle) == true)
                {
                    obj->flagMotorIdentified = true;

                    // clear the flag
                    obj->motorState = MOTOR_NORM_STOP;

                    obj->flagEnableRunAndIdentify = false;

                    // disable the estimator
                    EST_disable(obj->estHandle);

                    // enable the trajectory generator
                    EST_disableTraj(obj->estHandle);
                }
            }

        }
        if((EST_isOnLine(obj->estHandle) == true) && (obj->flagEnableRsRecalc == true))
        {
            obj->flagEnableRsRecalc = false;
            obj->motorState = MOTOR_STOP_IDLE;
        }
    }
}

/**
 * @brief Updates saturation inductance based on current
 * @param motorHandle Handle to motor control structure
 *
 * This function updates the d-axis and q-axis inductances based on the
 * stator current magnitude, accounting for saturation effects.
 */
static inline void LowPriority_updateSaturationInductance(MOTOR_Handle motorHandle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)motorHandle;
    USER_Params *objUser = (USER_Params *)(motorHandle->userParamsHandle);
    MOTOR_SetVars_t *objSets = (MOTOR_SetVars_t *)(motorHandle->motorSetsHandle);

    // Calculate the Ld and Lq which reduce with current
    objSets->Ls_d_comp_H = objUser->motor_Ls_d_H *
        (1.0f - obj->Is_A * objSets->Ls_d_Icomp_coef);
    objSets->Ls_q_comp_H = objUser->motor_Ls_q_H *
        (1.0f - obj->Is_A * objSets->Ls_q_Icomp_coef);

    // Enforce minimum inductance limits
    if(objSets->Ls_d_comp_H < objSets->Ls_min_H)
    {
        objSets->Ls_d_comp_H = objSets->Ls_min_H;
    }
    if(objSets->Ls_q_comp_H < objSets->Ls_min_H)
    {
        objSets->Ls_q_comp_H = objSets->Ls_min_H;
    }

    //TODO: FAST Lib Dependency
    //#TAG_FAST_OBS_START#
    // Update the Ld and Lq for motor control
//    EST_setLs_d_H(obj->estHandle, objSets->Ls_d_comp_H);
//    EST_setLs_q_H(obj->estHandle, objSets->Ls_q_comp_H);
    //#TAG_FAST_OBS_END#
}

/**
 * @brief Handles motor identification and trajectory setup
 * @param motorHandle Handle to motor control structure
 */
static inline void LowPriority_handleMotorIDAndTrajectory(MOTOR_Handle motorHandle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)motorHandle;
    USER_Params *objUser = (USER_Params *)(motorHandle->userParamsHandle);

    /* Motor ID will be true if Motor ID is bypassed / Motor ID completes */
    /* Trajectory is set for Openloop & Closed Loop */
    /* Set the SVM Mode , Min max Current , Set the Ramp rate */
    if(obj->flagMotorIdentified == true)
    {
        if(obj->speedRef_Hz > 0.0f)
        {
            obj->direction = 1.0f;
        }
        else
        {
            obj->direction = -1.0f;
        }
        //TODO: FAST Lib Dependency
        //#TAG_FAST_OBS_START#
        // enable or disable force angle
//        EST_setFlag_enableForceAngle(obj->estHandle, obj->flagEnableForceAngle);
        //#TAG_FAST_OBS_END#

        if(obj->motorState >= MOTOR_CL_RUNNING)
        {
            TRAJ_setTargetValue(&obj->traj_spd, obj->speedRef_Hz);
        }
        else
        {
            TRAJ_setTargetValue(&obj->traj_spd,
                                (obj->speedForce_Hz * obj->direction));
        }

        if((fabsf(obj->speed_Hz) > obj->speedStart_Hz) || (obj->motorState == MOTOR_CTRL_RUN))
        {
            TRAJ_setMaxDelta(&obj->traj_spd,
              (obj->accelerationMax_Hzps * objUser->ctrlPeriod_sec));

            if(obj->flagEnableLsUpdate == true)
            {
                // Update saturation inductance based on current
                LowPriority_updateSaturationInductance(motorHandle);
            }

            PI_setMinMax(&obj->pi_spd, -obj->maxCurrent_A, obj->maxCurrent_A);

            SVGEN_setMode(&obj->svgen, obj->svmMode);

            if(obj->motorState == MOTOR_CL_RUNNING)
            {
                obj->stateRunTimeCnt++;

                if(obj->stateRunTimeCnt == obj->fwcTimeDelay)
                {
                    obj->Idq_out_A.value[0] = 0.0f;
                    obj->motorState = MOTOR_CTRL_RUN;
                }
            }
        }
        else
        {
            TRAJ_setMaxDelta(&obj->traj_spd,
              (obj->accelerationStart_Hzps * objUser->ctrlPeriod_sec));

            if(obj->speed_int_Hz >= 0.0f)
            {
                PI_setMinMax(&obj->pi_spd, 0.0f, obj->startCurrent_A);
            }
            else
            {
                PI_setMinMax(&obj->pi_spd, -obj->startCurrent_A, 0.0f);
            }
        }
    }
}

/**
 * @brief Handles estimator error checking
 * @param motorHandle Handle to motor control structure
 */
static inline void LowPriority_handleCheckInstaSpinEstimatorErrors(MOTOR_Handle motorHandle)
{
//    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)motorHandle;

    //TODO: FAST Lib Dependency
    //#TAG_FAST_OBS_START#
    // check for the error in the fast estimator
//    if((EST_isError(obj->estHandle) == true) || (EST_isTrajError(obj->estHandle) == true))
//    {
//        // disable the PWM
//        HAL_disablePWM(&obj->halInterface->halFOCConfigHandle->pwmCfg,
//                      &obj->halInterface->halFOCDataHandle->pwmData);
//        obj->faultMtrNow.bit.estimError = 1;
//
//    }
    //#TAG_FAST_OBS_END#
}


/**
 * @brief Handles controller setup and updates
 * @param motorHandle Handle to motor control structure
 */
static inline void LowPriority_handleControllerSetup(MOTOR_Handle motorHandle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)motorHandle;

    obj->flagEstStateChanged = false;

    //TODO: FAST Lib Dependency
     //#TAG_FAST_OBS_START#
//    obj->flagEstStateChanged = EST_updateState(obj->estHandle, 0.0f);
//
//    obj->flagMotorIdentified = EST_isMotorIdentified(obj->estHandle);
    //#TAG_FAST_OBS_END#

    // TODO: Only based on updated state
    if(obj->flagMotorIdentified == true)
    {
        if(obj->flagSetupController == true)
        {
            // update the controller
            updateControllers(motorHandle);
        }
        else
        {
            obj->flagSetupController = true;

            setupControllers(motorHandle);
        }
    }
}

/**
 * @brief Handles dead time compensation setup
 * @param motorHandle Handle to motor control structure
 */
static inline void LowPriority_handleDeadTimeCompensation(MOTOR_Handle motorHandle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)motorHandle;
    USER_Params *objUser = (USER_Params *)(motorHandle->userParamsHandle);

    if(obj->deadTimeCompHandle->DTC_Enable == true)
    {
       deadTimeCompSetCoff(obj->deadTimeCompHandle, &obj->speed_Hz, &objUser->ctrlPeriod_sec,
                           &obj->halInterface->halFOCDataHandle->adcData.VdcBus_V);
    }
}

/**
 * @brief Handles global variables update
 * @param motorHandle Handle to motor control structure
 */
static inline void LowPriority_handleGlobalVariablesUpdate(MOTOR_Handle motorHandle)
{

    // update the global variables
    updateGlobalVariables(motorHandle);
}

/**
 * @brief Handles ESMO parameter updates
 * @param motorHandle Handle to motor control structure
 */
static inline void LowPriority_handleESMOParameterUpdates(MOTOR_Handle motorHandle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)motorHandle;

    // ESMO : Update ESMO Params
    ESMO_updateFilterParams(obj->esmoHandle);
    ESMO_updatePLLParams(obj->esmoHandle);
}

/**
 * @brief Main low-priority state machine execution -
 * @param motorHandle Handle to motor control structure
 *
 *
 * ARCHITECTURE:
 * - High Priority (ISR): FOC_runStateMachine() - Time-critical FOC control
 * - Low Priority (Background): FOC_runLowPriorityStateMachine() - Management & control
 * - Protection (Background): runMotorMonitor() - Monitoring & protection (case 1)
 */
void FOC_runLowPriorityStateMachine(MOTOR_Handle motorHandle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)motorHandle;


    LowPriority_handleMotorCheckFaults(motorHandle);

    /* Check for Motor Stop Command if Motor is in Active State */

    LowPriority_handleMotorCheckStop(motorHandle);

    /* Handle Insta Spin Estimator releated Errors */
    LowPriority_handleCheckInstaSpinEstimatorErrors(motorHandle);

    LowPriority_handleMotorIDAndTrajectory(motorHandle);

    runMotorMonitor(motorHandle);
    // State-specific tasks
    switch(obj->motorState)
    {
        case MOTOR_STOP_IDLE:
            // During Motor Idle State, continuously reset motor Control
            //TODO: Only Once when exiting from Fault or Stop State
            resetMotorControl(motorHandle);

            HAL_SetCompOCLimits(obj->halInterface);

            /* Restart the Motor if Ru Command is Received */
            LowPriority_handleMotorRestart(motorHandle);

            break;

        case MOTOR_FAULT_STOP:
            // Handle fault stop state
            LowPriority_handleMotorFaults(motorHandle);

            break;

        case MOTOR_NORM_STOP:

            stopMotorControl(motorHandle);

            // Handle brake stop state
            break;

        case MOTOR_BRAKE_STOP:
            // Handle brake stop state
            break;


        case MOTOR_PARAM_IDENT:
            // Handle parameter identification state
//            LowPriority_handleMotorIDAndTrajectory(motorHandle);

            //#TAG_FAST_OBS_START#
            //TODO: FAST Lib Dependency

//            LowPriority_runInstaSpinParameterID(motorHandle);
            //#TAG_FAST_OBS_END#
            break;


        case MOTOR_CL_RUNNING:
        case MOTOR_CTRL_RUN:
            // Handle closed-loop running state - full management

            LowPriority_handleDeadTimeCompensation(motorHandle);

            calculateRMSData(motorHandle);

            HAL_SetCompOCLimits(obj->halInterface);

            //TODO: FAST Lib Dependency
            //#TAG_FAST_OBS_START#
            // Monitor Rs online calibration timing

//            runRsOnLine(motorHandle);

            //#TAG_FAST_OBS_END#

            // Power and mechanical faults
            monitorOverLoad(motorHandle);
            monitorMotorStall(motorHandle);
            monitorLostPhase(motorHandle);

            // Current and speed faults (only when current is above threshold)
            if(motorHandle->Is_A > motorHandle->motorSetsHandle->IsFailedChekSet_A)
            {
                monitorCurrentUnbalance(motorHandle);
                monitorOverSpeed(motorHandle);
                monitorStartupFailure(motorHandle);
            }

            break;

        default:
            break;
    }


    LowPriority_handleControllerSetup(motorHandle);

    LowPriority_handleESMOParameterUpdates(motorHandle);

    LowPriority_handleGlobalVariablesUpdate(motorHandle);

}
