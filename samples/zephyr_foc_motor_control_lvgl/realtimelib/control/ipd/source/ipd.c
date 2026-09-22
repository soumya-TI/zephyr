//#############################################################################
//
// FILE:   ipd.c
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
#include "ipd.h"
#include "hal_FOCInterface.h"
#include <string.h>
#include "Platform_Types.h"

static void ipdVectorAnglesInit(IPD_Obj *);
static void ipdClearTimes(IPD_Obj *);
static void ipdAdjacentVectorsInit(IPD_Obj *);

/* Angle interpolation coefficients */
#define ANGLE_LINEAR_COEFF      (31.25f/360.0f)
#define ANGLE_CONSTANT_COEFF    (30.64375f/360.0f)

/* Vector Positions */
#define IPD_THIRTY_DEGREES          (30.0/360.0)
#define IPD_NINETY_DEGREES          (90.0/360.0)
#define IPD_ONE_FIFTY_DEGREES       (150.0/360.0)
#define IPD_TWO_TEN_DEGREES         (210.0/360.0)
#define IPD_TWO_SEVENTY_DEGREES     (270.0/360.0)
#define IPD_THREE_THIRTY_DEGREES    (330.0/360.0)

/* Timer overflows allowed before triggering timeout fault */
#define IPD_TIMER_OVERFLOW_LMT      (5)

//*****************************************************************************
//
// IPD_mem_init
//
//*****************************************************************************

IPD_Handle IPD_memInit(void *pMemory, const size_t numBytes)
{
    IPD_Handle handle;

    if((int16_t)numBytes < (int16_t)sizeof(IPD_Obj))
    {
        return((IPD_Handle)NULL);
    }

    //
    // assign the handle
    //
    handle = (IPD_Handle)pMemory;

    return(handle);
}

// end of file

void ipdInit(IPD_Obj *pIpd)
{
    pIpd->cycleCount = 0;

    pIpd->pulseToPulseCount = 0;

    pIpd->minRiseTime = 0; //1<<31 - 1;

    pIpd->vector = VOLTAGE_VECTOR_AC;

    ipdClearTimes(pIpd);
    ipdAdjacentVectorsInit(pIpd);
    ipdVectorAnglesInit(pIpd);
}

void ipdRun(IPD_Obj *pIpd)
{
    switch(pIpd->state)
    {
        case IPD_INIT:
            ipdInit(pIpd);

            HAL_ipdInit(pIpd->halInterface);
            pIpd->state = IPD_VECTOR_CONFIG;

        break;

        case IPD_VECTOR_CONFIG:
            pIpd->waitState = WAIT_GET_T1;
            pIpd->overflowCnt = 0;

            HAL_ipdPulseConfig(pIpd->vector, pIpd->halInterface);

            pIpd->state = IPD_RUN;
        break;

        case IPD_RUN:
            pIpd->pulseToPulseCount++;
            // Look for the TZ Event
            if(HAL_ipdPulseDone(pIpd->halInterface))
            {
                pIpd->pulseToPulseCountMax = pIpd->pulseToPulseCount << 1;
                pIpd->pulseToPulseCount = 0;
                pIpd->state = IPD_WAIT_CURRENT_DECAY;

                pIpd->timeRise[pIpd->vector] += pIpd->halInterface->halFOCDataHandle->ecapIPDPulseTime;

            }
            if(pIpd->pulseToPulseCount > pIpd->noMotorFaultCount)
            {
                pIpd->state = IPD_FAULT;
            }
        break;

        case IPD_WAIT_COMPLETE:
            pIpd->pulseToPulseCount++;
            pIpd->state = IPD_SET_NEXT_VECTOR;
        break;

        case IPD_SET_NEXT_VECTOR:
            pIpd->pulseToPulseCount++;
            pIpd->vector++;
            if(pIpd->vector >= VOLTAGE_VECTOR_MAX)
            {
                pIpd->vector = VOLTAGE_VECTOR_AC;
                pIpd->cycleCount++;
            }

            if(pIpd->cycleCount >= pIpd->cycleCountMax)
            {
                pIpd->state = IPD_CALC_SECTOR_RISE;
            }
            else
            {
                pIpd->pulseToPulseCount = 0;
                pIpd->state = IPD_VECTOR_CONFIG;
            }
        break;

        case IPD_WAIT_CURRENT_DECAY:
            if(pIpd->pulseToPulseCount >= pIpd->pulseToPulseCountMax)
            {
                pIpd->pulseToPulseCount = 0;
                pIpd->state = IPD_WAIT_COMPLETE;
            }
            else
            {
                pIpd->pulseToPulseCount++;
            }
        break;

        case IPD_CALC_SECTOR_RISE:
            pIpd->minRiseTime = pIpd->timeRise[0];

            pIpd->noRotationVectorCount = 0;
            static int8_t vector = 0;

            for( vector = 0; vector < 6; vector++)
            {
                if(pIpd->timeRise[vector] == 0)
                {
                    pIpd->state = IPD_FAULT;
                    break;
                }
                if( pIpd->timeRise[vector] <= pIpd->minRiseTime)
                {
                    pIpd->minRiseTime = pIpd->timeRise[vector];

                    if(pIpd->timeRise
                            [pIpd->adjacentVectorForward[vector]] <=
                        pIpd->timeRise
                            [pIpd->adjacentVectorReverse[vector]])
                    {
                        pIpd->nextMinRiseTime =
                                pIpd->timeRise[pIpd->adjacentVectorForward
                                               [vector]];
                        pIpd->nextNextMinRiseTime =
                                pIpd->timeRise[pIpd->adjacentVectorReverse
                                               [vector]];

                        pIpd->flags.b.angleAdjustinForward = TRUE;

                    }
                    else
                    {
                        pIpd->nextMinRiseTime =
                                pIpd->timeRise[pIpd->adjacentVectorReverse
                                               [vector]];
                        pIpd->nextNextMinRiseTime =
                                pIpd->timeRise[pIpd->adjacentVectorForward
                                               [vector]];

                        pIpd->flags.b.angleAdjustinForward = FALSE;
                    }

                    pIpd->noRotationVectorCount = vector;
                }
            }

            /* Only transition to next state if no fault was detected */
            if(pIpd->state != IPD_FAULT)
            {
                pIpd->state = IPD_CALC_ROTOR_POSITION;
            }
        break;

        case IPD_CALC_ROTOR_POSITION:
            if(pIpd->flags.b.highResAngleEnable)
            {
                pIpd->angleFunctionRatio =
                       ((float)(pIpd->minRiseTime - pIpd->nextMinRiseTime) /
                               (float)(pIpd->minRiseTime - pIpd->nextNextMinRiseTime));

                pIpd->angleAdjust = ANGLE_CONSTANT_COEFF -
                            (ANGLE_LINEAR_COEFF * pIpd->angleFunctionRatio);

                if(pIpd->flags.b.angleAdjustinForward)
                {
                    pIpd->rotorPosition =
                            pIpd->vectorAngle[pIpd->noRotationVectorCount] +
                            pIpd->angleAdjust;
                }
                else
                {
                    pIpd->rotorPosition =
                            pIpd->vectorAngle[pIpd->noRotationVectorCount] -
                            pIpd->angleAdjust;
                }
            }
            else
            {
                pIpd->rotorPosition =
                            pIpd->vectorAngle[pIpd->noRotationVectorCount];
            }

            pIpd->state = IPD_CALC_ANGLE;
        break;

        case IPD_CALC_ANGLE:

            HAL_ipdExit(pIpd->halInterface);

            pIpd->angle = pIpd->rotorPosition; // Add or subtract 90 degrees for the Final Applied Torque vector in Open Loop
            pIpd->state = IPD_COMPLETE;
        break;

        case IPD_COMPLETE:

        break;

        case IPD_FAULT:
            HAL_ipdExit(pIpd->halInterface);

        break;

        default:
            pIpd->state = IPD_INIT;
        break;
    }
}


static void ipdAdjacentVectorsInit(IPD_Obj *pIpd)
{
    pIpd->adjacentVectorForward[0] = VOLTAGE_VECTOR_BC;
    pIpd->adjacentVectorForward[1] = VOLTAGE_VECTOR_CB;
    pIpd->adjacentVectorForward[2] = VOLTAGE_VECTOR_BA;
    pIpd->adjacentVectorForward[3] = VOLTAGE_VECTOR_AB;
    pIpd->adjacentVectorForward[4] = VOLTAGE_VECTOR_CA;
    pIpd->adjacentVectorForward[5] = VOLTAGE_VECTOR_AC;

    pIpd->adjacentVectorReverse[0] = VOLTAGE_VECTOR_AB;
    pIpd->adjacentVectorReverse[1] = VOLTAGE_VECTOR_BA;
    pIpd->adjacentVectorReverse[2] = VOLTAGE_VECTOR_AC;
    pIpd->adjacentVectorReverse[3] = VOLTAGE_VECTOR_CA;
    pIpd->adjacentVectorReverse[4] = VOLTAGE_VECTOR_BC;
    pIpd->adjacentVectorReverse[5] = VOLTAGE_VECTOR_CB;
}

static void ipdVectorAnglesInit(IPD_Obj *pIpd)
{
    pIpd->vectorAngle[0] = IPD_THIRTY_DEGREES;
    pIpd->vectorAngle[1] = IPD_TWO_TEN_DEGREES;
    pIpd->vectorAngle[2] = IPD_NINETY_DEGREES;
    pIpd->vectorAngle[3] = IPD_TWO_SEVENTY_DEGREES;
    pIpd->vectorAngle[4] = IPD_ONE_FIFTY_DEGREES;
    pIpd->vectorAngle[5] = IPD_THREE_THIRTY_DEGREES;
}

static void ipdClearTimes(IPD_Obj *pIpd)
{
    memset((void*)pIpd->timeRise,0,VOLTAGE_VECTOR_MAX*sizeof(int32_t));
}
