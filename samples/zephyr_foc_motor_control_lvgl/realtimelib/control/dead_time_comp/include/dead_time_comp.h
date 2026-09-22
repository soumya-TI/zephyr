//#############################################################################
//
//! @file   dead_time_comp.h
//! @brief  ARM M33 Dead Time Compensation (DTC) library (floating point)
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

#ifndef DEAD_TIME_COMP_H
#define DEAD_TIME_COMP_H

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

//*****************************************************************************
//
//! \defgroup DEAD_TIME_COMP
//! @{
//
//*****************************************************************************

#include "types.h"
#include "ti_math.h"
#include "pr.h"

#define FREQ_PRE_WARP

//*****************************************************************************
//
//! \brief Defines the Dead Time Compensation object
//!
//! \details Implements dead-time compensation using PR controllers to
//!          counteract voltage distortion caused by inverter dead time,
//!          improving current control accuracy.
//
//*****************************************************************************
typedef struct _DTC_Obj_
{
    bool DTC_Enable;                 //!< Enable flag for dead-time compensation

    float32_t speedEnableLimit;      //!< Speed threshold to enable DTC, Hz
    float32_t speedDisableLimit;     //!< Speed threshold to disable DTC, Hz
    float32_t invDenominator;        //!< Inverse denominator for PR controller
    float32_t prevOutCoeff;          //!< Previous output coefficient for PR controller
    float32_t prevPrevOutCoeff;      //!< Previous-previous output coefficient for PR controller
    float32_t max_PR_out_factor;     //!< Maximum PR output scaling factor

    PR_Obj prIdObj;                  //!< PR controller object for d-axis compensation
    PR_Obj prIqObj;                  //!< PR controller object for q-axis compensation

   volatile PR_Handle prId;          //!< PR controller handle for d-axis
   volatile PR_Handle prIq;          //!< PR controller handle for q-axis

    MATH_Vec2 Vdq_DTC_V;             //!< Compensation voltage vector [Vd, Vq], V
}DTC_Obj;

//*****************************************************************************
//
//! \brief Defines the DTC handle
//
//*****************************************************************************
typedef struct _DTC_Obj_ *DTC_Handle;

//*****************************************************************************
//
// Prototypes for the APIs
//
//*****************************************************************************

//! \brief     Initializes the Dead Time Compensation module
//! \param[in] pMemory   A pointer to the memory for the DTC object
//! \param[in] numBytes  The number of bytes allocated for the DTC object
//! \return The DTC object handle
extern DTC_Handle DTC_init(void *pMemory, const size_t numBytes);

//*****************************************************************************
//
//! \brief                  Runs the dead time compensation module
//!
//! \param[in] handle       DTC handle
//!
//! \param[in] pInVec       The pointer to the input vector
//!
//! \return    None
//
//*****************************************************************************
static inline void deadTimeCompRun(DTC_Handle handle, const MATH_Vec2 *pInIdq)
{
    DTC_Obj *obj = (DTC_Obj *)handle;

    obj->prId->Feedback = pInIdq->value[0];
    obj->prIq->Feedback = pInIdq->value[1];

    // run the PR Id controller
    PR_run(obj->prId, obj->invDenominator, obj->prevOutCoeff,
                  obj->prevPrevOutCoeff);

    // run the PR Iq controller
    PR_run(obj->prIq, obj->invDenominator, obj->prevOutCoeff,
                  obj->prevPrevOutCoeff);

    obj->Vdq_DTC_V.value[0] = obj->prId->out;
    obj->Vdq_DTC_V.value[1] = obj->prIq->out;

    return;
}

//*****************************************************************************
//
//! \brief                  Runs the deadTimeCompSetCoff
//!
//! \param[in] handle       DTC handle
//!
//! \param[in] pSpeed       The pointer to the speed
//!
//! \param[in] pDeltaT      The pointer to the sampling time
//!
//! \param[in] pVdc         The pointer to the DC bus voltage
//!
//! \return    None
//
//*****************************************************************************
static inline void deadTimeCompSetCoff(DTC_Handle handle, const float32_t *pSpeed,
                                              const float32_t *pDeltaT, const float32_t *pVdc)
{
    DTC_Obj *obj = (DTC_Obj *)handle;

    float32_t omegaTsByTwo;
    float32_t speedOrig;
    float32_t omegaTs_2_orig;
    #ifdef FREQ_PRE_WARP
    float32_t tanOmegaTs_2_orig;
    #endif
    float32_t omegaTsByTwoSqr;
    float32_t maxSaturation;

    speedOrig = 6.0f * (*pSpeed);

#ifdef FREQ_PRE_WARP

    omegaTs_2_orig = (speedOrig * (*pDeltaT) * MATH_PI);

    tanOmegaTs_2_orig = (__sin(omegaTs_2_orig) / __cos(omegaTs_2_orig));

    omegaTsByTwo = tanOmegaTs_2_orig;

#else
    omegaTs_2_orig = (speedOrig * (*pDeltaT));

    omegaTsByTwo = omegaTs_2_orig * MATH_PI;
#endif

    omegaTsByTwoSqr = omegaTsByTwo * omegaTsByTwo;

    obj->prevPrevOutCoeff = -(1.0f + omegaTsByTwoSqr);

    obj->invDenominator = (1.0f / (-obj->prevPrevOutCoeff));

    obj->prevOutCoeff = (1.0f - omegaTsByTwoSqr) * 2.0f;

    maxSaturation = (*pVdc) * obj->max_PR_out_factor;

    obj->prId->outMax = maxSaturation;
    obj->prId->outMin = -maxSaturation;

    obj->prIq->outMax = maxSaturation;
    obj->prIq->outMin = -maxSaturation;

    return;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // end of DEAD_TIME_COMP_H defines
