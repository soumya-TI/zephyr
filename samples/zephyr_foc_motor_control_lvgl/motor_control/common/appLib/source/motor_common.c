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
//! \file   /solutions/universal_motorcontrol_lab/common/source/motor_comm.c
//!
//! \brief  This project is used to implement motor control with FAST, eSMO
//!         Encoder, and Hall sensors based sensored/sensorless-FOC.
//!         Supports multiple TI EVM boards
//!         ARM M33 compatible - uses TMU math functions via fastrts_tmu.h
//!
//

//
// include the related header files
//

#include "motor_common.h"
#include "hal.h"          // Needed for HAL functions
#include "user.h"         // Needed for user configuration and EST functions

//! \brief checks motor faults
void checkMotorFaults(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;

    if(HAL_getPwmEnableStatus(&obj->halInterface->halFOCDataHandle->pwmData) == true)
    {
        if(HAL_getMtrTripFaults(&obj->halInterface->halFOCConfigHandle->pwmCfg) != 0)
        {
            obj->faultMtrNow.bit.moduleOverCurrent = 1;
        }
    }
    obj->faultMtrPrev.all |= obj->faultMtrNow.all;
    obj->faultMtrUse.all = obj->faultMtrNow.all & obj->faultMtrMask.all;
//    HAL_setMtrCMPSSDACValue(obj->halInterface->halFOCHandle,
//                            objSets->dacCMPValH, objSets->dacCMPValL);
    if(obj->flagClearFaults == true)
    {
        HAL_clearMtrFaultStatus(obj->halInterface);
        obj->faultMtrNow.all &= MTR_FAULT_CLEAR;
        obj->flagClearFaults = false;
    }
    return;
}

// Sets up control parameters for stopping motor
void stopMotorControl(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;

    obj->speed_int_Hz = 0.0f;

    obj->flagRunIdentAndOnLine = false;
    if(obj->motorState == MOTOR_BRAKE_STOP)
    {
        if(obj->brakingMode == HARDSWITCH_BRAKE_MODE)
        {
            // Exit braking PWM mode
            HAL_ResetPWM(&obj->halInterface->halFOCConfigHandle->pwmCfg, &obj->halInterface->halFOCDataHandle->pwmData);
        }

        obj->motorState = MOTOR_STOP_IDLE;
        obj->flagEnableBraking = false;

        obj->IsRef_A = 0.0f;
        PI_setUi(&obj->pi_spd, 0.0f);
        PI_setRefValue(&obj->pi_spd, 0.0f);
    }

    obj->motorState = MOTOR_STOP_IDLE;

    SVGEN_setMode(&obj->svgen, SVM_COM_C);

    obj->restartTimesCnt = 0;

    return;
}

// Sets up control parameters for restarting motor
void restartMotorControl(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;

    USER_Params *objUser = (USER_Params *)(handle->userParamsHandle);

    if(obj->flagEnableMotorIdentify == true)
    {
        if(obj->flagMotorTiTuneEnable == false)
        {
            obj->estimatorMode = ESTIMATOR_MODE_FAST;
            obj->flagMotorIdentified = false;
            obj->flagSetupController = false;
            obj->svmMode = SVM_COM_C;
            obj->flagEnableFWC = false;
            obj->flagEnableMTPA = false;


            obj->speedRef_Hz = objUser->fluxExcFreq_Hz;

            objUser->flag_bypassMotorId = false;

            // #TAG_FAST_OBS_START#
            // disable interrupts
//            __disable_irq();

            //TODO: FAST Lib Dependency
             //#TAG_FAST_OBS_START#
            // set the default estimator parameters
//            EST_setParams(obj->estHandle, obj->userParamsHandle);
            //#TAG_FAST_OBS_END#

            // enable interrupts

//            __enable_irq();

            //#TAG_FAST_OBS_END#

        }
        else
        {
            obj->flagMotorTiTuneRun = true;
            obj->enableSpeedCtrl = false;
            obj->enableCurrentCtrl = false;
        }
        obj->flagEnableMotorIdentify = false;

        obj->motorState = MOTOR_PARAM_IDENT;

    }
//#TAG_FAST_OBS_START#
    //TODO: FAST Lib Dependency
//    else if(obj->flagEnableRsRecalc == true)
//    {
//        obj->estimatorMode = ESTIMATOR_MODE_FAST;
//
//        // disable the estimator
//        EST_disable(obj->estHandle);
//
//        // disable the trajectory generator
//        EST_disableTraj(obj->estHandle);
//
//        // disable interrupts
//#if defined(__TMS320C28XX__)
//        DINT;
//        __asm("  NOP");
//#else
//        __disable_irq();
//#endif
//
//        // set the default estimator parameters
//        EST_setParams(obj->estHandle, obj->userParamsHandle);
//
//        // enable interrupts
//#if defined(__TMS320C28XX__)
//        EINT;
//        __asm("  NOP");
//#else
//        __enable_irq();
//#endif
//
//        // disable the estimator
//        EST_enable(obj->estHandle);
//
//        // disable the trajectory generator
//        EST_enableTraj(obj->estHandle);
//
//        /* Flag Motor Is Identified set to not perform complete ID Routine */
//        EST_setFlag_motorIdentified(obj->estHandle, 1);
//
//        /* Flag EnableRsRecalc is enabled to perform Rs Recalibration */
//        EST_setFlag_enableRsRecalc(obj->estHandle, 1);
//
//        obj->motorState = MOTOR_PARAM_IDENT;
//    }
// #TAG_FAST_OBS_END#

    else if(obj->motorState == MOTOR_BRAKE_STOP)
    {
        if(obj->brakingMode == HARDSWITCH_BRAKE_MODE)
        {
            // Exit braking PWM mode
            HAL_ResetPWM(&obj->halInterface->halFOCConfigHandle->pwmCfg, &obj->halInterface->halFOCDataHandle->pwmData);
        }

        obj->motorState = MOTOR_STOP_IDLE;
        obj->flagEnableBraking = false;

        obj->IsRef_A = 0.0f;
        PI_setUi(&obj->pi_spd, 0.0f);
        PI_setRefValue(&obj->pi_spd, 0.0f);
    }

    else if(obj->flagEnableFlyingStart == true)
    {
        obj->motorState = MOTOR_SEEK_POS;
        obj->enableSpeedCtrl = false;
        obj->enableCurrentCtrl = true;
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

    // #TAG_ESMO_START#
    if( obj->estimatorMode == ESTIMATOR_MODE_ESMO)
    {
        ESMO_resetParams(obj->esmoHandle);

        ESMO_resetPLL(obj->esmoHandle);
    }
    // #TAG_ESMO_END#

    obj->speed_int_Hz = 0.0f;

    SVGEN_setMode(&obj->svgen, SVM_COM_C);

    obj->flagRunIdentAndOnLine = true;
    obj->stateRunTimeCnt = 0;
    obj->startSumTimesCnt++;

    return;
}

// Resets motor control parameters for restarting motor
void resetMotorControl(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;

    //TODO: FAST Lib Dependency
    //#TAG_FAST_OBS_START#
//    if(obj->flagEnableFlyingStart == false)
//    {
//        // disable the estimator
//        EST_disable(obj->estHandle);
//
//        // disable the trajectory generator
//        EST_disableTraj(obj->estHandle);
//    }
//    else
//    {
//        EST_enable(obj->estHandle);
//
//        obj->stateRunTimeCnt = 0;
//    }
//
//    EST_setFlag_enableRsRecalc(obj->estHandle,0);
//
//    EST_setRsOnLineId_mag_A(obj->estHandle, 0.0f);
//    EST_setRsOnLineId_A(obj->estHandle, 0.0f);
//    EST_setRsOnLine_Ohm(obj->estHandle, EST_getRs_Ohm(obj->estHandle));
//
//    EST_setFlag_enableRsOnLine(obj->estHandle, false);
//    EST_setFlag_updateRs(obj->estHandle, false);

    //#TAG_FAST_OBS_END#

    TRAJ_setIntValue(&obj->traj_spd, obj->speed_Hz);

    TRAJ_setTargetValue(&obj->traj_spd, 0.0f);

    // disable the PWM
    HAL_disablePWM(&obj->halInterface->halFOCConfigHandle->pwmCfg, &obj->halInterface->halFOCDataHandle->pwmData);

    // clear integral outputs of the controllers
    PI_setRefValue(&obj->pi_Id, 0.0f);
    PI_setRefValue(&obj->pi_Iq, 0.0f);
    PI_setRefValue(&obj->pi_spd, 0.0f);

    PI_setUi(&obj->pi_Id, 0.0f);
    PI_setUi(&obj->pi_Iq, 0.0f);
    PI_setUi(&obj->pi_spd, 0.0f);

    // clear current references
    obj->Idq_out_A.value[0] = 0.0f;
    obj->Idq_out_A.value[1] = 0.0f;

    obj->IdRated_A = 0.0f;
    obj->IsRef_A = 0.0f;

    obj->angleCurrent_rad = 0.0f;

    PI_setUi(&obj->pi_fwc, 0.0f);

    obj->stateRunTimeCnt = 0;
    obj->motorStallTimeCnt = 0;
    obj->startupFailTimeCnt = 0;

    obj->overSpeedTimeCnt = 0;
    obj->overLoadTimeCnt = 0;
    obj->lostPhaseTimeCnt = 0;
    obj->unbalanceTimeCnt = 0;

    obj->flagMotorIdentified = true;

    SVGEN_setMode(&obj->svgen, SVM_COM_C);
    return;
}


//! \brief Monitor DC bus over-voltage condition
static inline void monitorOverVoltage(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;
    MOTOR_SetVars_t *objSets = (MOTOR_SetVars_t *)(handle->motorSetsHandle);

    if(obj->halInterface->halFOCDataHandle->adcData.VdcBus_V > objSets->overVoltageFault_V)
    {
        if(obj->overVoltageTimeCnt > objSets->voltageFaultTimeSet)
        {
            obj->faultMtrNow.bit.overVoltage = 1;
        }
        else
        {
            obj->overVoltageTimeCnt++;
        }
    }
    else if(obj->halInterface->halFOCDataHandle->adcData.VdcBus_V < objSets->overVoltageNorm_V)
    {
        if(obj->overVoltageTimeCnt == 0)
        {
            obj->faultMtrNow.bit.overVoltage = 0;
        }
        else
        {
            obj->overVoltageTimeCnt--;
        }
    }
}

//! \brief Monitor DC bus under-voltage condition
static inline void monitorUnderVoltage(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;
    MOTOR_SetVars_t *objSets = (MOTOR_SetVars_t *)(handle->motorSetsHandle);

    if(obj->halInterface->halFOCDataHandle->adcData.VdcBus_V < objSets->underVoltageFault_V)
    {
        if(obj->underVoltageTimeCnt > objSets->voltageFaultTimeSet)
        {
            obj->faultMtrNow.bit.underVoltage = 1;
        }
        else
        {
            obj->underVoltageTimeCnt++;
        }
    }
    else if(obj->halInterface->halFOCDataHandle->adcData.VdcBus_V > objSets->underVoltageNorm_V)
    {
        if(obj->underVoltageTimeCnt == 0)
        {
            obj->faultMtrNow.bit.underVoltage = 0;
        }
        else
        {
            obj->underVoltageTimeCnt--;
        }
    }
}

//! \brief Monitor motor over-load condition
void monitorOverLoad(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;
    MOTOR_SetVars_t *objSets = (MOTOR_SetVars_t *)(handle->motorSetsHandle);

    if(obj->powerActive_W > objSets->overLoadSet_W)
    {
        if(obj->overLoadTimeCnt > objSets->overLoadTimeSet)
        {
            obj->faultMtrNow.bit.overLoad = 1;
            obj->overLoadTimeCnt = 0;
        }
        else
        {
            obj->overLoadTimeCnt++;
        }
    }
    else if(obj->overLoadTimeCnt > 0)
    {
        obj->overLoadTimeCnt--;
    }
}

//! \brief Monitor motor stall condition
void monitorMotorStall(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;
    MOTOR_SetVars_t *objSets = (MOTOR_SetVars_t *)(handle->motorSetsHandle);

    if((obj->Is_A > objSets->stallCurrentSet_A) &&
       (obj->speedAbs_Hz < objSets->speedFailMinSet_Hz))
    {
        if(obj->motorStallTimeCnt > objSets->motorStallTimeSet)
        {
            obj->faultMtrNow.bit.motorStall = 1;
            obj->motorStallTimeCnt = 0;
        }
        else
        {
            obj->motorStallTimeCnt++;
        }
    }
    else if(obj->motorStallTimeCnt > 0)
    {
        obj->motorStallTimeCnt--;
    }
}

//! \brief Monitor motor lost phase condition
void monitorLostPhase(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;
    MOTOR_SetVars_t *objSets = (MOTOR_SetVars_t *)(handle->motorSetsHandle);

    if((obj->speedAbs_Hz > objSets->speedFailMinSet_Hz) &&
       ((obj->Irms_A[0] < objSets->lostPhaseSet_A) ||
        (obj->Irms_A[1] < objSets->lostPhaseSet_A) ||
        (obj->Irms_A[2] < objSets->lostPhaseSet_A)))
    {
        if(obj->lostPhaseTimeCnt > objSets->lostPhaseTimeSet)
        {
            obj->faultMtrNow.bit.motorLostPhase = 1;
            obj->lostPhaseTimeCnt = 0;
        }
        else
        {
            obj->lostPhaseTimeCnt++;
        }
    }
    else if(obj->lostPhaseTimeCnt > 0)
    {
        obj->lostPhaseTimeCnt--;
    }
}

//! \brief Monitor phase current unbalance condition
void monitorCurrentUnbalance(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;
    MOTOR_SetVars_t *objSets = (MOTOR_SetVars_t *)(handle->motorSetsHandle);

    if(obj->unbalanceRatio > objSets->unbalanceRatioSet)
    {
        if(obj->unbalanceTimeCnt > objSets->unbalanceTimeSet)
        {
            obj->faultMtrNow.bit.currentUnbalance = 1;
            obj->unbalanceTimeCnt = 0;
        }
        else
        {
            obj->unbalanceTimeCnt++;
        }
    }
    else if(obj->unbalanceTimeCnt > 0)
    {
        obj->unbalanceTimeCnt--;
    }
}

//! \brief Monitor motor over-speed condition
 void monitorOverSpeed(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;
    MOTOR_SetVars_t *objSets = (MOTOR_SetVars_t *)(handle->motorSetsHandle);

    if(obj->speedAbs_Hz > objSets->speedFailMaxSet_Hz)
    {
        if(obj->overSpeedTimeCnt > objSets->overSpeedTimeSet)
        {
            obj->faultMtrNow.bit.overSpeed = 1;
            obj->overSpeedTimeCnt = 0;
        }
        else
        {
            obj->overSpeedTimeCnt++;
        }
    }
    else if(obj->overSpeedTimeCnt > 0)
    {
        obj->overSpeedTimeCnt--;
    }
}

//! \brief Monitor startup failure condition
void monitorStartupFailure(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;
    MOTOR_SetVars_t *objSets = (MOTOR_SetVars_t *)(handle->motorSetsHandle);

    if((obj->Is_A < objSets->stallCurrentSet_A) &&
       (obj->speedAbs_Hz < objSets->speedFailMinSet_Hz))
    {
        if(obj->startupFailTimeCnt > objSets->startupFailTimeSet)
        {
            obj->faultMtrNow.bit.startupFailed = 1;
            obj->startupFailTimeCnt = 0;
        }
        else
        {
            obj->startupFailTimeCnt++;
        }
    }
    else if(obj->startupFailTimeCnt > 0)
    {
        obj->startupFailTimeCnt--;
    }
}

//=============================================================================
// Main Motor Monitor Function (Consolidated)
//=============================================================================

//! \brief Main motor monitoring function - calls all monitoring subfunctions
//! Timer base is 5ms
void runMotorMonitor(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;

    // Decrement stop wait timer
    if(obj->stopWaitTimeCnt > 0)
    {
        obj->stopWaitTimeCnt--;
    }

    // Monitor DC bus voltage conditions (always active)
    monitorOverVoltage(handle);
    monitorUnderVoltage(handle);

    return;
}

void collectRMSData(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;

    obj->IrmsCalSum[0] += obj->halInterface->halFOCDataHandle->adcData.I_A.value[0] * obj->halInterface->halFOCDataHandle->adcData.I_A.value[0];
    obj->IrmsCalSum[1] += obj->halInterface->halFOCDataHandle->adcData.I_A.value[1] * obj->halInterface->halFOCDataHandle->adcData.I_A.value[1];
    obj->IrmsCalSum[2] += obj->halInterface->halFOCDataHandle->adcData.I_A.value[2] * obj->halInterface->halFOCDataHandle->adcData.I_A.value[2];

    obj->VIrmsIsrCnt++;

    if(obj->VIrmsIsrCnt > obj->VIrmsIsrSet)
    {
        obj->IrmsPrdSum[0] = obj->IrmsCalSum[0];
        obj->IrmsPrdSum[1] = obj->IrmsCalSum[1];
        obj->IrmsPrdSum[2] = obj->IrmsCalSum[2];

        obj->IrmsCalSum[0] = 0.0f;
        obj->IrmsCalSum[1] = 0.0f;
        obj->IrmsCalSum[2] = 0.0f;

        obj->VIrmsIsrCnt = 0;
        obj->flagVIrmsCal = true;
    }
}

void calculateRMSData(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;

    float32_t IrmsMax_A, IrmsMin_A, VIrmsIsrSet;

    if(obj->flagVIrmsCal == true)
    {
        obj->flagVIrmsCal = false;

        obj->Irms_A[0] =
                sqrtf(obj->IrmsPrdSum[0] * obj->IrmsCalSF);

        obj->Irms_A[1] =
                sqrtf(obj->IrmsPrdSum[1] * obj->IrmsCalSF);

        obj->Irms_A[2] =
                sqrtf(obj->IrmsPrdSum[2] * obj->IrmsCalSF);

        if(obj->Irms_A[0] > obj->Irms_A[1])
        {
            IrmsMax_A = obj->Irms_A[0];
            IrmsMin_A = obj->Irms_A[1];
        }
        else
        {
            IrmsMax_A = obj->Irms_A[0];
            IrmsMin_A = obj->Irms_A[1];
        }

        IrmsMax_A = (obj->Irms_A[2] > IrmsMax_A) ? obj->Irms_A[2] : IrmsMax_A;
        IrmsMin_A = (obj->Irms_A[2] < IrmsMin_A) ? obj->Irms_A[2] : IrmsMin_A;

        VIrmsIsrSet = obj->VIrmsIsrScale / obj->speedAbs_Hz;
        VIrmsIsrSet = (VIrmsIsrSet > obj->VIrmsIsrScale) ?
                obj->VIrmsIsrScale : VIrmsIsrSet;

        obj->VIrmsIsrSet = (uint16_t)(VIrmsIsrSet);

        obj->IrmsCalSF = 1.0f / ((float32_t)(obj->VIrmsIsrSet));

        obj->unbalanceRatio =
                (IrmsMax_A - IrmsMin_A) / (IrmsMax_A + IrmsMin_A);
        }
}

// setupCurrentControllers()
void setupCurrentControllers(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;
    USER_Params *objUser = (USER_Params *)(handle->userParamsHandle);

    float32_t RoverL_Kp_sf = objUser->RoverL_Kp_sf;
    float32_t dcBus_nominal_V = objUser->dcBus_nominal_V;
    float32_t maxCurrent_A = objUser->maxCurrent_A;
    float32_t RoverL_min_rps = objUser->RoverL_min_rps;
    float32_t currentCtrlPeriod_sec =
                (float32_t)objUser->numCtrlTicksPerCurrentTick /
                    objUser->ctrlFreq_Hz;

    float32_t outMax_V = objUser->Vd_sf * objUser->maxVsMag_V;
    float32_t Kp = RoverL_Kp_sf * dcBus_nominal_V / maxCurrent_A;
    float32_t Ki = RoverL_min_rps * currentCtrlPeriod_sec;

    // set the Id controller
    PI_setGains(&obj->pi_Id, Kp, Ki);
    PI_setUi(&obj->pi_Id, 0.0f);
    PI_setRefValue(&obj->pi_Id, 0.0f);
    PI_setFbackValue(&obj->pi_Id, 0.0f);
    PI_setFfwdValue(&obj->pi_Id, 0.0f);
    PI_setMinMax(&obj->pi_Id, -outMax_V, outMax_V);

    // set the Iq controller
    PI_setGains(&obj->pi_Iq, Kp, Ki);
    PI_setUi(&obj->pi_Iq, 0.0f);
    PI_setRefValue(&obj->pi_Iq, 0.0f);
    PI_setFbackValue(&obj->pi_Iq, 0.0f);
    PI_setFfwdValue(&obj->pi_Iq, 0.0f);
    PI_setMinMax(&obj->pi_Iq, -outMax_V, outMax_V);

    return;
} // end of setupCurrentControllers() function

void setupControllers(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;
    USER_Params *objUser = (USER_Params *)(handle->userParamsHandle);

    float32_t Ls_d_H = objUser->motor_Ls_d_H;
    float32_t Ls_q_H = objUser->motor_Ls_q_H;

    float32_t Rs_Ohm = objUser->motor_Rs_Ohm;
    float32_t RdoverLd_rps = Rs_Ohm / Ls_d_H;
    float32_t RqoverLq_rps = Rs_Ohm / Ls_q_H;

    float32_t BWc_rps = objUser->BWc_rps;
    float32_t currentCtrlPeriod_sec =
                (float32_t)objUser->numCtrlTicksPerCurrentTick /
                objUser->ctrlFreq_Hz;

    float32_t outMax_V = objUser->Vd_sf *
            objUser->maxVsMag_V;

    float32_t Kp_Id = Ls_d_H * BWc_rps;
    float32_t Ki_Id = 0.25f * RdoverLd_rps * currentCtrlPeriod_sec;

    float32_t Kp_Iq = Ls_q_H * BWc_rps;
    float32_t Ki_Iq = 0.25f * RqoverLq_rps * currentCtrlPeriod_sec;

    // set the Id controller
    PI_setGains(&obj->pi_Id, Kp_Id, Ki_Id);
    PI_setUi(&obj->pi_Id, 0.0f);
    PI_setRefValue(&obj->pi_Id, 0.0f);
    PI_setFbackValue(&obj->pi_Id, 0.0f);
    PI_setFfwdValue(&obj->pi_Id, 0.0f);
    PI_setMinMax(&obj->pi_Id, -outMax_V, outMax_V);

    // set the Iq controller
    PI_setGains(&obj->pi_Iq, Kp_Iq, Ki_Iq);

    PI_setUi(&obj->pi_Iq, 0.0f);
    PI_setRefValue(&obj->pi_Iq, 0.0f);
    PI_setFbackValue(&obj->pi_Iq, 0.0f);
    PI_setFfwdValue(&obj->pi_Iq, 0.0f);
    PI_setMinMax(&obj->pi_Iq, 0.0f, 0.0f);

    // set the speed controller
    if(objUser->Kctrl_Wb_p_kgm2 <= 0.01f)
    {
        float32_t Kp_spd1 = 2.5f * objUser->maxCurrent_A / objUser->maxFrequency_Hz;
        float32_t Ki_spd1 = 5.0f * objUser->maxCurrent_A * objUser->ctrlPeriod_sec;

        PI_setGains(&obj->pi_spd, Kp_spd1, Ki_spd1);
    }
    else
    {
        float32_t speedCtrlPeriod_sec =
            (float32_t)objUser->numCtrlTicksPerSpeedTick /
            objUser->ctrlFreq_Hz;

        float32_t BWdelta = objUser->BWdelta;

        float32_t Kctrl_Wb_p_kgm2 = objUser->Kctrl_Wb_p_kgm2;

        float32_t Kp_spd = BWc_rps / (BWdelta * Kctrl_Wb_p_kgm2);
        float32_t Ki_spd = BWc_rps * speedCtrlPeriod_sec / (BWdelta * BWdelta);// BWc_rps * BWc_rps * speedCtrlPeriod_sec / (BWdelta * BWdelta * Kctrl_Wb_p_kgm2);

        PI_setGains(&obj->pi_spd, Kp_spd, Ki_spd);
    }

    PI_setUi(&obj->pi_spd, 0.0f);
    PI_setRefValue(&obj->pi_spd, 0.0f);
    PI_setFbackValue(&obj->pi_spd, 0.0f);
    PI_setFfwdValue(&obj->pi_spd, 0.0f);
    PI_setMinMax(&obj->pi_spd,
                 -objUser->maxCurrent_A,
                 objUser->maxCurrent_A);
    // copy the Id, Iq and speed controller parameters to motorVars
    getControllers(handle);


    return;
} // end of setupControllers() function

// #TAG_FIELD_WEAKENING_START#
void updateFWCParams(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;
    MOTOR_SetVars_t *objSets = (MOTOR_SetVars_t *)(handle->motorSetsHandle);

    // Update FW control parameters
    PI_setGains(&obj->pi_fwc, objSets->Kp_fwc, objSets->Ki_fwc);
    PI_setOutMin(&obj->pi_fwc, objSets->angleFWCMax_rad);
}
// #TAG_FIELD_WEAKENING_END#

//#TAG_MTPA_START#
void updateMTPAParams(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;
    MOTOR_SetVars_t *objSets = (MOTOR_SetVars_t *)(handle->motorSetsHandle);

    if(obj->flagUpdateMTPAParams == true)
    {
        //
        // update motor parameters according to current
        //
        obj->LsOnline_d_H = MTPA_updateLs_d_withLUT(obj->mtpaHandle, obj->Is_A);

        obj->LsOnline_q_H = MTPA_updateLs_q_withLUT(obj->mtpaHandle, obj->Is_A);

        obj->fluxOnline_Wb = objSets->flux_Wb;

        //
        // update the motor constant for MTPA based on
        // the update Ls_d and Ls_q which are the function of Is
        //
        MTPA_computeParameters(obj->mtpaHandle,
                               obj->LsOnline_d_H,
                               obj->LsOnline_q_H,
                               obj->fluxOnline_Wb);
    }

    return;
}
//#TAG_MTPA_END#

//#TAG_FAST_OBS_START#
//=============================================================================
// Motor Monitoring Subfunctions
//=============================================================================

//! \brief Monitor Rs online calibration timing
static inline void monitorRsOnlineTiming(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;
    MOTOR_SetVars_t *objSets = (MOTOR_SetVars_t *)(handle->motorSetsHandle);

    if(obj->flagEnableRsOnLine == true)
    {
        if(obj->flagRsOnLineContinue == true)
        {
            obj->flagStartRsOnLine = true;
        }
        else if(obj->RsOnlineTimeCnt == 0)
        {
            if(EST_getFlag_enableRsOnLine(obj->estHandle) == true)
            {
                obj->RsOnlineTimeCnt = objSets->RsOnlineWaitTimeSet;
                obj->flagStartRsOnLine = false;
            }
            else
            {
                obj->RsOnlineTimeCnt = objSets->RsOnlineWorkTimeSet;
                obj->flagStartRsOnLine = true;
            }
        }
        else
        {
            obj->RsOnlineTimeCnt--;
        }
    }
    else
    {
        obj->flagStartRsOnLine = false;
        obj->RsOnlineTimeCnt = 0;
    }
}

void runRsOnLine(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;
    MOTOR_SetVars_t *objSets = (MOTOR_SetVars_t *)(handle->motorSetsHandle);

    monitorRsOnlineTiming(handle);

    // execute Rs OnLine code
    if(obj->flagRunIdentAndOnLine == true)
    {
        if((EST_getState(obj->estHandle) == EST_STATE_ONLINE) &&
                (obj->flagStartRsOnLine == true))
        {
            EST_setFlag_enableRsOnLine(obj->estHandle, true);

            EST_setRsOnLineId_mag_A(obj->estHandle, objSets->RsOnLineCurrent_A);

            float32_t RsError_Ohm =
                    objSets->RsOnLine_Ohm - objSets->Rs_Ohm;

            if(fabsf(RsError_Ohm) < (objSets->Rs_Ohm * 0.15f))
            {
                EST_setFlag_updateRs(obj->estHandle, true);
            }
        }
        else
        {
            EST_setRsOnLineId_mag_A(obj->estHandle, 0.0f);
            EST_setRsOnLineId_A(obj->estHandle, 0.0f);
            EST_setRsOnLine_Ohm(obj->estHandle, EST_getRs_Ohm(obj->estHandle));

            EST_setFlag_enableRsOnLine(obj->estHandle, false);
            EST_setFlag_updateRs(obj->estHandle, false);
        }
    }

    return;
} // end of runRsOnLine() function
//#TAG_FAST_OBS_END#

// update motor control variables
void updateGlobalVariables(MOTOR_Handle handle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)handle;

//    MOTOR_SetVars_t *objSets = (MOTOR_SetVars_t *)(handle->motorSetsHandle);

    //#TAG_FAST_OBS_START#
    //TODO: FAST Lib Dependency
//    USER_Params *objUser = (USER_Params *)(handle->userParamsHandle);
//
//    // get the states
//
//    obj->estState = EST_getState(obj->estHandle);
//    obj->trajState = EST_getTrajState(obj->estHandle);
//
//    // get the rotor resistance
//    objSets->Rr_Ohm = EST_getRr_Ohm(obj->estHandle);
//
//    // get the stator resistance
//    objSets->Rs_Ohm = EST_getRs_Ohm(obj->estHandle);
//
//    // get the stator inductance in the direct coordinate direction
//    objSets->Ls_d_H = EST_getLs_d_H(obj->estHandle);
//
//    // get the stator inductance in the quadrature coordinate direction
//    objSets->Ls_q_H = EST_getLs_q_H(obj->estHandle);
//
//    // get the flux, V/Hz
//    if((objUser->flag_bypassMotorId == true) || (obj->estState >= EST_STATE_RAMPUP))
//    {
//        objSets->flux_Wb   = EST_getFlux_Wb(obj->estHandle);
//        objSets->flux_VpHz = EST_getFlux_Wb(obj->estHandle) * MATH_TWO_PI;
//    }
//
//    // get the stator resistance estimate from RsOnLine
//    objSets->RsOnLine_Ohm = EST_getRsOnLine_Ohm(obj->estHandle);
//
//    // get the rated magnetizing current value
//    objSets->magneticCurrent_A = EST_getIdRated_A(obj->estHandle);
//
//    // get R/L
//    objSets->RoverL_rps = EST_getRoverL_rps(obj->estHandle);
//
//    // get the torque estimate
//    obj->torque_Nm = EST_computeTorque_Nm(obj->estHandle);

//#TAG_FAST_OBS_END#

    // Calculate the motor output power
    obj->powerReal_W = obj->torque_Nm * obj->speedAbs_Hz * obj->power_sf;
    // Calculate the RMS stator current
    obj->Is_A = sqrtf(obj->Idq_in_A.value[0] * obj->Idq_in_A.value[0] +
                      obj->Idq_in_A.value[1] * obj->Idq_in_A.value[1]);

    // Calculate the RMS stator voltage
    obj->Vs_V = sqrtf(obj->Vdq_out_V.value[0] * obj->Vdq_out_V.value[0] +
                      obj->Vdq_out_V.value[1] * obj->Vdq_out_V.value[1]);

    // Calculate the motor input power
    obj->powerInvtOut_W = obj->Vs_V * obj->Is_A * 1.50f;

    // Add a filter to calculate the motor input power
    obj->powerActive_W = obj->powerInvtOut_W * 0.125f + obj->powerActive_W * 0.875f;

    return;
} // end of updateGlobalVariables() function

//! \brief     Sets the number of current sensors
//! \param[in] handle             The Clarke (CLARKE) handle
//! \param[in] numCurrentSensors  The number of current sensors
void setupClarke_I(CLARKE_Handle handle, const uint16_t numCurrentSensors)
{
    float32_t alpha_sf, beta_sf;

    // initialize the Clarke transform module for current
    if(3 == numCurrentSensors)
    {
        alpha_sf = MATH_ONE_OVER_THREE;
        beta_sf = MATH_ONE_OVER_SQRT_THREE;
    }
    else if(2 == numCurrentSensors)
    {
        alpha_sf = 1.0f;
        beta_sf = MATH_ONE_OVER_SQRT_THREE;
    }
    else
    {
        alpha_sf = 0.0f;
        beta_sf = 0.0f;
    }

    // set the parameters
    CLARKE_setScaleFactors(handle, alpha_sf, beta_sf);
    CLARKE_setNumSensors(handle, numCurrentSensors);

    return;
} // end of setupClarke_I() function

//! \brief     Sets the number of voltage sensors
//! \param[in] handle             The Clarke (CLARKE) handle
//! \param[in] numVoltageSensors  The number of voltage sensors
void setupClarke_V(CLARKE_Handle handle,const uint16_t numVoltageSensors)
{
    float32_t alpha_sf,beta_sf;

    // initialize the Clarke transform module for voltage
    if(numVoltageSensors == 3)
    {
        alpha_sf = MATH_ONE_OVER_THREE;
        beta_sf = MATH_ONE_OVER_SQRT_THREE;
    }
    else
    {
        alpha_sf = 0.0f;
        beta_sf = 0.0f;
    }

    // set the parameters
    CLARKE_setScaleFactors(handle, alpha_sf, beta_sf);
    CLARKE_setNumSensors(handle, numVoltageSensors);

    return;
} // end of setupClarke_V() function

//! \brief Initialize motor control system (interrupts, PWM, gate driver, parameters, offset calibration)
//! \param[in] motorHandle The motor control handle
//! \param[in] isrFunc Pointer to the motor's interrupt service routine
void initMotorControlSystem(MOTOR_Handle motorHandle, void (*isrFunc)(void))
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)motorHandle;

    // Initialize the interrupt vector table with motor-specific ISR
    HAL_initIntVectorTableWithISR(&obj->halInterface->halFOCConfigHandle->intCfg, isrFunc);

    // Enable the ADC/PWM interrupts for control
    // Enable interrupts to trigger DMA
    HAL_enableCtrlInts(&obj->halInterface->halFOCConfigHandle->intCfg);

    // Configure PWM for motor control
    HAL_ConfigPWM(obj->halInterface);

    // Initialize gate driver
    HAL_initGateDriver(obj->gateDriveHandle);

    HAL_SetCompOCLimits(obj->halInterface);

    return;
} // end of initMotorControlSystem() function

//! \brief Initialize datalogging and debug outputs
//! \param[in] motorHandle The motor control handle
//! \param[in] halHandle The HAL handle
//! \param[in] datalogHandle Pointer to datalog handle
//! \param[in] datalog Pointer to datalog object
//! \param[in] datalogBuff1 Pointer to datalog buffer 1
//! \param[in] datalogBuff2 Pointer to datalog buffer 2
void initDatalogging(MOTOR_Handle motorHandle,
                    HAL_Handle halHandle,
                    DATALOG_Handle *datalogHandle,
                    DATALOG_Obj *datalog,
                    float32_t *datalogBuff1,
                    float32_t *datalogBuff2)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)motorHandle;


    // Initialize Datalog
    *datalogHandle = DATALOGIF_init(datalog, sizeof(DATALOG_Obj));
    DATALOG_Obj *datalogObj = (DATALOG_Obj *)(*datalogHandle);

    // Setup DMA buffers for datalogging
    HAL_setupDMAforDLOG(halHandle, 0, &datalogBuff1[0], &datalogBuff1[1]);
    HAL_setupDMAforDLOG(halHandle, 1, &datalogBuff2[0], &datalogBuff2[1]);

    // Configure datalog input pointers (what to log)
    datalogObj->iptr[0] = &obj->anglePLL_rad;
    datalogObj->iptr[1] = &obj->angleEST_rad;
//
//    // Configure DAC outputs for real-time debugging
//    obj->dacCtrl1.dacEn = 1;
//    obj->dacCtrl2.dacEn = 1;
//
//    // DAC Channel 1: EST angle output
//    obj->dacCtrl1.pDacOutAddr = (float32_t*)&obj->angleFOC_rad;
//    obj->dacCtrl1.dacScalingFactor = (DAC_MAX_VALUE / MATH_TWO_PI);
//
//    // DAC Channel 2: PLL angle output
//
//     obj->dacCtrl2.pDacOutAddr = (float32_t*)&obj->IdqRef_A.value[0];
//     obj->dacCtrl2.dacScalingFactor = (DAC_MAX_VALUE / obj->maxCurrent_A);

    // Alternative DAC configurations (commented for reference):
    //
    // Example 1: Output Angle PLL
    //    obj->dacCtrl2.pDacOutAddr = (float32_t*)&obj->anglePLL_rad;
    //    obj->dacCtrl2.dacScalingFactor = (DAC_MAX_VALUE / MATH_TWO_PI);
    //
    // Example 2: Output phase currents
    // obj->dacCtrl1.pDacOutAddr = (float32_t*)&obj->halInterface->halFOCDataHandle->adcData.I_A.value[0];
    // obj->dacCtrl1.dacScalingFactor = (DAC_MAX_VALUE / objUser->maxCurrent_A);
    // obj->dacCtrl2.pDacOutAddr = (float32_t*)&obj->halInterface->halFOCDataHandle->adcData.I_A.value[1];
    // obj->dacCtrl2.dacScalingFactor = (DAC_MAX_VALUE / objUser->maxCurrent_A);
    //
    // Example 3: Output ESMO voltage components
    // obj->dacCtrl1.pDacOutAddr = (float32_t*)&obj->esmoHandle->Ealpha;
    // obj->dacCtrl1.dacScalingFactor = (DAC_MAX_VALUE / objUser->dcBus_nominal_V);
    // obj->dacCtrl2.pDacOutAddr = (float32_t*)&obj->esmoHandle->Ebeta;
    // obj->dacCtrl2.dacScalingFactor = (DAC_MAX_VALUE / objUser->dcBus_nominal_V);

    return;
} // end of initDatalogging() function

/* Initialize DAC outputs for real-time debugging */
void initDACOutputs(SYSTEM_Vars_t *sysVarsPtr, MOTOR_Handle motorHandle)
{
    MOTOR_Vars_t *obj = (MOTOR_Vars_t *)motorHandle;
    USER_Params *objUser = (USER_Params *)(motorHandle->userParamsHandle);

    /* Configure DAC outputs for real-time debugging */
    sysVarsPtr->dacCtrl1.dacEn = 1U;
    /* sysVarsPtr->dacCtrl2.dacEn = 1U; */

    /* DAC Channel 1: PLL angle output */
    sysVarsPtr->dacCtrl1.pDacOutAddr = (float32_t *)&obj->angleFOC_rad;
    sysVarsPtr->dacCtrl1.dacScalingFactor = (DAC_MAX_VALUE / MATH_TWO_PI);

    /* DAC Channel 2: Id reference output */
    sysVarsPtr->dacCtrl2.pDacOutAddr = (float32_t *)&obj->IdqRef_A.value[0];
    sysVarsPtr->dacCtrl2.dacScalingFactor = (DAC_MAX_VALUE / objUser->maxCurrent_A);

    /*
     * Alternative DAC configurations (commented for reference):
     *
     * Example 1: Output Angle PLL
     *    sysVarsPtr->dacCtrl2.pDacOutAddr = (float32_t*)&obj->anglePLL_rad;
     *    sysVarsPtr->dacCtrl2.dacScalingFactor = (DAC_MAX_VALUE / MATH_TWO_PI);
     *
     * Example 2: Output phase currents
     *    sysVarsPtr->dacCtrl1.pDacOutAddr =
     *        (float32_t*)&obj->halInterface->halFOCDataHandle->adcData.I_A.value[0];
     *    sysVarsPtr->dacCtrl1.dacScalingFactor = (DAC_MAX_VALUE / objUser->maxCurrent_A);
     *    sysVarsPtr->dacCtrl2.pDacOutAddr =
     *        (float32_t*)&obj->halInterface->halFOCDataHandle->adcData.I_A.value[1];
     *    sysVarsPtr->dacCtrl2.dacScalingFactor = (DAC_MAX_VALUE / objUser->maxCurrent_A);
     *
     * Example 3: Output ESMO voltage components
     *    sysVarsPtr->dacCtrl1.pDacOutAddr = (float32_t*)&obj->esmoHandle->Ealpha;
     *    sysVarsPtr->dacCtrl1.dacScalingFactor =
     *        (DAC_MAX_VALUE / objUser->dcBus_nominal_V);
     *    sysVarsPtr->dacCtrl2.pDacOutAddr = (float32_t*)&obj->esmoHandle->Ebeta;
     *    sysVarsPtr->dacCtrl2.dacScalingFactor =
     *        (DAC_MAX_VALUE / objUser->dcBus_nominal_V);
     */

    return;
} /* end of initDACOutputs() function */
