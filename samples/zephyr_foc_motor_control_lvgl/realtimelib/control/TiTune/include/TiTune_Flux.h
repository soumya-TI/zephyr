/*
 *  Copyright (C) 2025 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//! @file   TiTune_Flux.h
//! @brief  TI Motor Control Library - Flux Linkage Tuning Module
//! @details Implements automatic flux linkage parameter identification through
//!          high-speed operation and back-EMF measurement. The algorithm accelerates
//!          the motor to a test speed, waits for stabilization, then measures the
//!          back-EMF to calculate the permanent magnet flux linkage.

#ifndef TITUNE_FLUX_H
#define TITUNE_FLUX_H

/*************************************************************************************************************************************************************************************/
// If building with a C++ compiler, make all of the definitions in this header have a C binding.
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

/*************************************************************************************************************************************************************************************/
#include <stdlib.h>
#include <stdint.h>

#include "ti_math.h"
#include "types.h"

#define MtrType_INDUCTION               (1)
#define MtrType_IPMSM                   (2)
#define MtrType_SYNRM                   (3)

/*************************************************************************************************************************************************************************************/
//! @brief Defines the Flux Tuning controller object
//! @details Structure containing all parameters and state variables for automatic
//!          flux linkage identification through high-speed back-EMF measurement
/*************************************************************************************************************************************************************************************/
typedef struct _TiTune_Flux_Obj_
{
    bool VariableInit;                  //!< Variable initialization enable flag
    float32_t TimeOutError;             //!< Timeout error flag - speed failed to stabilize
    float32_t SpeedRampArrives;         //!< Speed ramp arrival flag
    float32_t MagnFluxEstEn;            //!< Magnet flux estimation enable flag
    float32_t SlowDownEn;               //!< Motor slow-down enable flag after estimation
    float32_t CtrlCycleCounter;         //!< Control cycle execution counter
    float32_t AveragerInputGain;        //!< Averager input scaling coefficient
    float32_t SpeedErrFiltered;         //!< Filtered speed error for stabilization detection (Per-Unit)
    float32_t IqFbkDerivative;          //!< Low-pass filtered derivative of Iq for inductive compensation (Per-Unit)
    float32_t IqFbkPast;                //!< One-sample delayed Iq feedback (Per-Unit)
    float32_t SpeedFbk_Average;         //!< Averaged speed feedback (Per-Unit)
    float32_t BackEMF_Average;          //!< Averaged back-EMF voltage (Per-Unit)

    uint32_t AveragingSize;             //!< Number of samples to average for estimation
    float32_t WaitingTime2Settle_Speed; //!< Samples to wait for speed loop settling
    float32_t MaxTime4Settle;           //!< Maximum allowed settling time before timeout
    float32_t TestSpeed;                //!< Target test speed for flux measurement (Per-Unit)
    float32_t SpeedErrLim4Settle;       //!< Speed error threshold for stabilization (Per-Unit)
    float32_t Rs;                       //!< Stator resistance (Per-Unit)
    float32_t Lq;                       //!< Q-axis inductance (Per-Unit)
    float32_t TimePU;                   //!< Sampling time in per-unit
    float32_t LPF_IqDeriv_Gain;         //!< LPF gain for Iq derivative calculation
    float32_t LPF_SpeedErr_Gain;        //!< LPF gain for speed error filtering

    float32_t SpeedFbk;                 //!< Speed feedback input (Per-Unit)
    float32_t IqFbk;                    //!< Q-axis current feedback input (Per-Unit)
    float32_t VqRef;                    //!< Q-axis voltage reference input (Per-Unit)

    float32_t SpeedRef;                 //!< Speed reference output (Per-Unit)
    float32_t MagnFlux;                 //!< Measured PM flux linkage output (Per-Unit)
    float32_t Finish;                   //!< Identification completion flag

    float32_t Vph_Base;                 //!< Phase voltage base value
    float32_t I_Base;                   //!< Current base value
    float32_t F_Base;                   //!< Frequency base value

    float32_t BASE_FREQ;                //!< Base frequency in Hz

    float32_t X_base;                   //!< Reactance base value
    float32_t flux_base;                //!< Flux linkage base value
    float32_t Flux_Final;               //!< Final flux linkage in physical units

} TiTune_Flux_Obj;

/*****************************************************************************/
//  Defines the TiTune_Flux handle
/*****************************************************************************/

typedef struct _TiTune_Flux_Obj_ *TiTune_Flux_Handle;

/*****************************************************************************/

extern TiTune_Flux_Handle TiTune_Flux_init(void *pMemory, const size_t numBytes);

/*****************************************************************************/
static inline void TiTune_Flux_run(TiTune_Flux_Handle handle, const float32_t MechSpeed, const float32_t iq_fb, const float32_t Vq_fb)
{

    TiTune_Flux_Obj *obj = (TiTune_Flux_Obj *)handle;

        /* Initialize some variables. */
        /* This enables safe start and usage of this routine again without resetting CPU. */

    if (obj->VariableInit == true)
       {  obj->VariableInit = false;

          obj->TimeOutError = 0.0f;
          obj->SpeedRampArrives = 0.0f;
          obj->MagnFluxEstEn = 0.0f;
          obj->SlowDownEn = 0.0f;
          obj->CtrlCycleCounter = 0.0f;

          /* Find '1/N' or '1/AveragingSize'. They will be used as a multiplier for input signal in DFT so that DFT will not overflow if input is always less than 1 */

          obj->AveragingSize = MATH_sat(obj->AveragingSize, (1UL<<29), 1UL); /* Safety check for over/underflow-free correct working of below code. */

          /* Use full capacity of 32-bit. Enlarge the input gain of the DFT or average not to loose information for trancation. But then at the end, divide the result. */
          obj->AveragerInputGain = ( (float)(1UL<<(30-24)) / (float)(obj->AveragingSize) );

          /* Assign the reference test speed */
          obj->SpeedRef = obj->TestSpeed;
          obj->SpeedErrFiltered = obj->TestSpeed; /* Precaution for pre-mature pass from STEP-1 to STEP-2 due to the if-comparison in STEP-1. */

          obj->IqFbkDerivative = 0.0f;
          obj->IqFbkPast = 0.0f;

          obj->SpeedFbk_Average = 0.0f;
          obj->BackEMF_Average = 0.0f;

          obj->Finish = 0.0f;
       }
    obj->SpeedFbk =  MechSpeed / obj->F_Base;
    obj->IqFbk = iq_fb / obj->I_Base;
    obj->VqRef = Vq_fb / obj->Vph_Base;

    /* Speed control error is filtered by a 1st order Low-Pass Filter (Backward Euler discretized) because the error will be used in comparison, noise may cause wrong decision. */
    obj->SpeedErrFiltered += (obj->LPF_SpeedErr_Gain * ((obj->SpeedRef - obj->SpeedFbk) - obj->SpeedErrFiltered));

    /* STEP-1 : Wait until motor speed stabilizes to the test speed */
    if ((obj->SpeedRampArrives == 1.0f) && (obj->MagnFluxEstEn == 0.0f) && (obj->SlowDownEn == 0.0f))
       {  obj->CtrlCycleCounter++;
          if (obj->CtrlCycleCounter >= obj->WaitingTime2Settle_Speed)
             {
//              /* Option-1 : Only time-based decision making. */
//                obj->MagnFluxEstEn = 1.0f;
//                obj->CtrlCycleCounter = 0.0f;

                /* Option-2 : Checking Speed error vanishes as a second parameter on top of the 'WaitingTime2Settle_Speed' timer for decision making. */
                if (MATH_abs(obj->SpeedErrFiltered) < obj->SpeedErrLim4Settle)
                  {
                    obj->MagnFluxEstEn = 1.0f;
                    obj->CtrlCycleCounter = 0.0f;
                  }
                /* Part of Option-2. If Id cannot settle in a maximum allowed settling time, then 'TimeOutError' is set TRUE. */
                if (obj->CtrlCycleCounter >= obj->MaxTime4Settle)
                   {
                    obj->TimeOutError = 1.0f;
                    obj->MagnFluxEstEn = 1.0f;
                    obj->CtrlCycleCounter = 0.0f;
                   }
             }
       }

    /* STEP-2 : Estimation */
    if ((obj->MagnFluxEstEn == 1.0f) && (obj->SlowDownEn == 0.0f))
       {  obj->CtrlCycleCounter++;
          /* Limited bandwidth differentiation. Backward Euler differentiation with followed by a 1st order Low-Pass Filter which is also discretized with Backward Euler. */
         obj->IqFbkDerivative += ( obj->LPF_IqDeriv_Gain * (((obj->IqFbk - obj->IqFbkPast) / obj->TimePU) - obj->IqFbkDerivative) );

          /* Pass variables to the DFT or average which selects X(k=0) harmonic component. */

         obj->SpeedFbk_Average += (obj->SpeedFbk * obj->AveragerInputGain);
         obj->BackEMF_Average += ( (obj->VqRef - (obj->Rs * obj->IqFbk) - (obj->Lq * obj->IqFbkDerivative)) * obj->AveragerInputGain);

          /* STOP estimation when cycle number hits the predefined limit value of 'AveragingSize' */
          if (obj->CtrlCycleCounter >= (float)(obj->AveragingSize))
             {  obj->CtrlCycleCounter = 0.0f;
                obj->SpeedRampArrives = 0.0f;
                obj->MagnFluxEstEn = 0.0f;
                obj->SlowDownEn = 1.0f;

                obj->SpeedRef = 0.0f;
                obj->SpeedErrFiltered = (0.0f - obj->TestSpeed); /* Precaution for pre-mature finish before truely slow-down due to the if-comparison in STEP-3. */

                /* Save the estimated permanent-magnet flux linkage */
                obj->MagnFlux  = (obj->BackEMF_Average / obj->SpeedFbk_Average);
                obj->Flux_Final = obj->MagnFlux * obj->flux_base;
             }
       }

    /* Put present current feedback into memory for one sample delay. Used in the 'IqFbkDerivative'. */
    obj->IqFbkPast = obj->IqFbk;

    /* STEP-3 : Finish permanent-magnet flux linkage identification when motor speed stabilizes to zero. */
    if ((obj->SpeedRampArrives == 1.0f) && (obj->SlowDownEn == 1.0f))
       {  obj->CtrlCycleCounter++;

          if (obj->CtrlCycleCounter >= obj->WaitingTime2Settle_Speed)
             {
              /* Option-1 : Only time-based decision making. */
//              obj->Finish = true;
//              obj->VariableInit = true;

                /* Option-2 : Checking Speed error vanishes as a second parameter on top of the 'WaitingTime2Settle_Speed' timer for decision making. */
                /* This time, we do not set 'TimeOutError' even if settling takes more time than 'MaxTime4Settle' because estimation is already done and only stopping is left. */
                if ((MATH_abs(obj->SpeedErrFiltered) < obj->SpeedErrLim4Settle) || (obj->CtrlCycleCounter >= obj->MaxTime4Settle))
                   {
                      obj->Finish = true;
                      obj->VariableInit = true;
                   }
             }
       }

    return;
} // end of TiTune_Flux_run() function


//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // end of TITUNE_FLUX_H define
