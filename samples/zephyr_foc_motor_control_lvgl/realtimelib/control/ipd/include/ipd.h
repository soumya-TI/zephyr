//#############################################################################
//
// FILE:   ipd.h
//
// TITLE:  ARM M33 Initial Position Detection (IPD) library (floating point)
//
//#############################################################################
// Copyright (C) 2025 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the
//   distribution.
//
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//#############################################################################
/*!****************************************************************************
 *  @file       ipd.h
 *  @brief      ARM M33 Initial Position Detection (IPD) library (floating point)
 *
 *  @details    Implements rotor position detection for motor startup using
 *              high-frequency voltage pulse injection to determine initial
 *              rotor angle before beginning sensorless FOC operation.
 *
 *  @anchor ipd_h
 *  # Overview
 *
 *  The IPD module detects rotor position by injecting voltage vectors and
 *  measuring current rise times to determine magnetic saliency direction.
 *
 *  <hr>
 ******************************************************************************/


#ifndef IPD_H
#define IPD_H

#include <stdint.h>
#include "hal_FOCInterface.h"
#include "hal.h"
#ifdef __cplusplus
extern "C"
{
#endif

//! \brief IPD flags union for control and status
//!
typedef union
{
    struct ipdFlags
    {
        _Bool
            angleAdjustinForward: 1,        //!< Angle adjustment direction flag
            highResAngleEnable: 1,          //!< Enable high-resolution angle interpolation
            enable: 1,                      //!< IPD module enable flag
            test:  1;                       //!< Test mode flag (switches to align)
    }b;

    uint16_t w;                             //!< Word access to all flags

}IPD_FLAGS_T;

//! \brief IPD state machine enumeration
//!
//! \details Defines the execution states for the IPD algorithm
//!
typedef enum
{
    IPD_INIT = 0,                       //!< Initialize IPD module
    VOLTAGE_VECTOR_CONFIG = 1,          //!< Configure voltage vector parameters
    IPD_RUN = 2,                        //!< IPD algorithm running
    IPD_VECTOR_CONFIG = 3,              //!< Configure next voltage vector
    IPD_WAIT_CURRENT_DECAY = 5,         //!< Wait for motor current to decay
    IPD_WAIT_COMPLETE = 6,              //!< Wait for T1 and T2 measurements complete
    IPD_SET_NEXT_VECTOR = 7,            //!< Set next voltage vector to apply
    IPD_CALC_SECTOR_RISE = 8,           //!< Calculate sector from rise times
    IPD_CALC_ROTOR_POSITION = 9,        //!< Calculate rotor position angle
    IPD_CALC_ANGLE = 10,                //!< Calculate angle for motor startup
    IPD_COMPLETE = 11,                  //!< IPD successfully completed
    IPD_FAULT = 12,                     //!< IPD fault detected
}IPD_STATE_TYPES;

//! \brief IPD wait state enumeration
//!
//! \details Sub-states for waiting and capturing current rise times
//!
typedef enum
{
    WAIT_GET_T1 = 0,                       //!< Waiting to capture T1 rise time
    WAIT_GET_T2 = 1,                       //!< Waiting to capture T2 rise time
    WAIT_COMPLETE = 2,                     //!< Both measurements complete
}IPD_WAIT_TYPES;

//! \brief IPD fault type enumeration
//!
//! \details Defines possible IPD fault conditions
//!
typedef enum
{
    IPD_NO_FAULT = 0,                 //!< No fault detected
    IPD_FAULT_CLOCK_TIMEOUT = 1,      //!< Capture clock overflow (measurement timeout)
    IPD_FAULT_DECAY_TIME = 2,         //!< Insufficient decay time (wrong frequency)
}IPD_FAULT_TYPES;

//! \brief Defines the Initial Position Detection (IPD) object
//!
//! \details Contains all state variables, configuration parameters, and
//!          measured data for executing the IPD algorithm sequence.
//!
typedef struct _IPD_Obj_
{
    IPD_STATE_TYPES
        state;                  //!< Current IPD state machine state

    IPD_WAIT_TYPES
        waitState;              //!< Current wait sub-state for measurements

    VOLTAGE_VECTOR
        vector;                 //!< Active voltage vector being applied

    uint16_t
        cycleCount,             //!< Current electrical cycle counter
        cycleCountMax;          //!< Maximum electrical cycles allowed

    uint16_t
        vectorCount,            //!< Current voltage vector sequence counter
        noRotationVectorCount,  //!< Counter for preventing reverse rotation
        adjacentVectorForward
                [VOLTAGE_VECTOR_MAX],  //!< Forward rotation adjacent vector table
        adjacentVectorReverse
                [VOLTAGE_VECTOR_MAX];  //!< Reverse rotation adjacent vector table

    IPD_FAULT_TYPES
        faultStatus;            //!< Current IPD fault status

    int32_t
        timeRise[VOLTAGE_VECTOR_MAX],  //!< Current rise time for each vector, ticks
        minRiseTime,            //!< Minimum rise time measured, ticks
        nextMinRiseTime,        //!< Adjacent vector minimum rise time, ticks
        nextNextMinRiseTime,    //!< Next adjacent vector minimum rise time, ticks
        pulseToPulseCount,      //!< Count between current pulses, ticks
        noMotorFaultCount,      //!< Timeout threshold for no motor fault, ticks
        pulseToPulseCountMax;   //!< Maximum allowed pulse-to-pulse count, ticks

    float
        rotorPosition,          //!< Detected rotor position angle, rad
        angle,                  //!< Startup angle for open-loop transition, rad
        advanceAngle,           //!< Angle advance for IPD vector, rad
        vectorAngle
                [VOLTAGE_VECTOR_MAX],  //!< Voltage vector angles for initialization, rad

        angleFunctionRatio,     //!< Ratio for angle calculation function
        angleAdjust;            //!< Angle adjustment value, rad

    uint8_t overflowCnt;        //!< Timer overflow counter for fault detection

    IPD_FLAGS_T
        flags;                  //!< IPD control and status flags

    const HAL_FOC_Interface_t *halInterface;  //!< Hardware abstraction layer interface handle

}IPD_Obj;
//*****************************************************************************
//
//! \brief Defines the IPD handle
//
//*****************************************************************************
typedef struct _IPD_Obj_ *IPD_Handle;

void ipdInit(IPD_Obj *pIpd);

void ipdRun(IPD_Obj *pIpd);

IPD_Handle IPD_memInit(void *pMemory, const size_t numBytes);

inline _Bool getIpdComplete(IPD_Obj *pIpd)
{
    return (pIpd->state == IPD_COMPLETE);
}

inline _Bool getIpdTestMode(IPD_Obj *pIpd)
{
    return (pIpd->flags.b.test == true);
}

inline _Bool getIpdFaultStatus(IPD_Obj *pIpd)
{
    return (pIpd->state == IPD_FAULT);
}

void IPDCurrThreshAction(IPD_Obj *pIpd);

void IPDOverFlowAction(IPD_Obj *pIpd);

#ifdef __cplusplus
}
#endif
#endif /* IPD_H */
