/*
 * Copyright (c) 2021 Texas Instruments Incorporated - http://www.ti.com
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

#ifndef USER_MOTOR_PARAMS_H
#define USER_MOTOR_PARAMS_H

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

// includes
// ti_math.h internally includes <math.h> and fastrts_tmu.h for TMU acceleration
#include "libraries/math/include/ti_math.h"
#include "user_mtr1.h"

//! \brief Structure to hold motor parameters that were previously defined as macros
typedef struct _USER_MotorParams_t_
{
    // Motor configuration parameters
    uint16_t numCurrentSensors;
    uint16_t numVoltageSensors;

    // Controller parameters
    uint16_t numPwmTicksPerIsrTick;
    uint16_t numIsrTicksPerCurrentTick;
    uint16_t numIsrTicksPerSpeedTick;

    // PWM parameters
    float32_t pwmFreq_kHz;
    uint16_t pwmTbprdNum;
    float32_t pwmPeriod_usec;
    float32_t isrFreq_Hz;
    float32_t isrPeriod_usec;
    float32_t ctrlPeriod_usec;
    float32_t ctrlPeriod_sec;

    // Voltage parameters
    float32_t vdSf;
    float32_t maxVsMag_pu;
    float32_t vsRefMag_pu;

    // Field Weakening Control parameters
    float32_t fwcKp;
    float32_t fwcKi;
    float32_t fwcMaxAngle_rad;
    float32_t fwcMinAngle_rad;

    // Fault parameters
    float32_t lostPhaseCurrent_A;
    float32_t unbalanceRatio;
    float32_t overLoadPower_W;
    float32_t torqueFailedSet_Nm;
    float32_t failSpeedMax_Hz;
    float32_t failSpeedMin_Hz;
    float32_t stallCurrent_A;
    float32_t faultCheckCurrent_A;

    // Fault timing parameters
    uint16_t overCurrentTimesSet;
    uint16_t voltageFaultTimeSet;
    uint16_t motorStallTimeSet;
    uint16_t startupFailTimeSet;
    uint16_t overSpeedTimeSet;
    uint16_t overLoadTimeSet;
    uint16_t unbalanceTimeSet;
    uint16_t lostPhaseTimeSet;
    uint16_t stopWaitTimeSet;
    uint16_t restartWaitTimeSet;
    uint16_t startTimesSet;

    // Estimator parameters
    float32_t estFluxHfSf;
    float32_t estFreqHfSf;
    float32_t estBemfHfSf;

    // IPD parameters
    float32_t ipdFaultTimeSec;

} USER_MotorParams_t;

//! \brief Defines the USER_MotorParams_t handle
typedef struct _USER_MotorParams_t_ *USER_MotorParams_Handle;

// External declarations for motor parameter structures
extern USER_MotorParams_t userMotorParams_M1;

// Function prototypes
extern void USER_initMotorParams(void);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif // extern "C"

#endif // USER_MOTOR_PARAMS_H
