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

//! @file   TiTune_J.h
//! @brief  TI Motor Control Library - Inertia Tuning Module
//! @details Implements automatic mechanical inertia identification using speed
//!          oscillation and DFT analysis. The algorithm applies controlled torque
//!          variations and measures the resulting speed response to calculate inertia.

#ifndef TITUNE_J_H
#define TITUNE_J_H

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


#include <stdlib.h>

#include "ti_math.h"

#define sgn_POSITIVE        (+1)
#define sgn_NEGATIVE        (-1)


//*****************************************************************************
//
//! @brief Defines the Inertia Tuning controller object
//! @details Structure containing all parameters and state variables for automatic
//!          mechanical inertia identification through speed oscillation analysis
//
//*****************************************************************************
typedef struct _TiTune_J_Obj_
{
    bool VariableInit;                    //!< Variable initialization enable flag
    uint16_t ResetRiseFallCounters;       //!< Rise/fall time counter reset flag
    uint16_t TorqueFound;                 //!< Suitable torque level found flag
    uint16_t TorqueNotFoundError;         //!< Torque search failure error flag
    uint16_t IqMinAssigned;               //!< Min search Iq value assigned flag
    uint16_t IqMaxAssigned;               //!< Max search Iq value assigned flag
    uint16_t EnableCheckingHighTorque;    //!< High torque checking enable flag
    uint16_t Prepare4Est;                 //!< DFT parameter preparation flag
    uint16_t EstimationEn;                //!< Estimation and DFT enable flag
    int16_t  TorqueSign;                  //!< Torque sign: +1 (positive) or -1 (negative)
    uint16_t MaxIqHitCounter;             //!< Counter for IqRef hitting IqMaxLim
    uint16_t MaxSpdHitCounter;            //!< Counter for speed hitting SpdMaxLim
    uint16_t MaxSpdHitCounter_Resettable; //!< Resettable speed limit hit counter
    uint32_t SampleCounterDFT;            //!< DFT sample counter
    uint32_t SampleSizeDFT;               //!< Total DFT sample size
    uint16_t FinishedTestPeriodCounter;   //!< Completed speed oscillation period counter
    uint16_t DownSampleCounter;           //!< Down-sampling counter
    uint16_t DownSampleRatio;             //!< Down-sampling ratio for large DFT sizes
    uint32_t SpdRiseCounter;              //!< Speed rise time sample counter
    uint32_t SpdFallCounter;              //!< Speed fall time sample counter
    uint32_t RiseTime4Est;                //!< Rise time samples for estimation
    uint32_t FallTime4Est;                //!< Fall time samples for estimation

    float32_t MinSearchIq;                //!< Minimum Iq search value (Per-Unit)
    float32_t MaxSearchIq;                //!< Maximum Iq search value (Per-Unit)
    float32_t InjSpeedFreq;               //!< Speed oscillation frequency (Per-Unit)
    float32_t AngleDFT;                   //!< Current DFT angle value (Per-Unit)
    float32_t AngleStepDFT;               //!< DFT angle increment per sample (Per-Unit)
    float32_t InputGainDFT;               //!< DFT input scaling coefficient
    float32_t SpdMagnDFT;                 //!< Speed peak magnitude from DFT (Per-Unit)
    float32_t SpdPhaseDFT;                //!< Speed phase angle from DFT (Per-Unit)
    float32_t IqMagnDFT;                  //!< Iq peak magnitude from DFT (Per-Unit)
    float32_t IqPhaseDFT;                 //!< Iq phase angle from DFT (Per-Unit)
    float32_t CosDFT;                     //!< Cosine value for DFT computation
    float32_t SinDFT;                     //!< Sine value for DFT computation
    float32_t SpdRealDFT;                 //!< Real part of speed DFT (Per-Unit)
    float32_t SpdImagDFT;                 //!< Imaginary part of speed DFT (Per-Unit)
    float32_t IqRealDFT;                  //!< Real part of Iq DFT (Per-Unit)
    float32_t IqImagDFT;                  //!< Imaginary part of Iq DFT (Per-Unit)

    uint32_t RiseTimeMin;                 //!< Minimum allowed speed rise time samples
    uint32_t RiseTimeMax;                 //!< Maximum allowed speed rise time samples
    uint32_t SettlePeriod;                //!< Speed periods for settling before estimation
    uint32_t MaxSampleSizeDFT;            //!< Maximum DFT sample size for numerical accuracy
    float32_t Nbase;                      //!< Base sample size: Fsample/Fbase
    float32_t SpdMinLim;                  //!< Minimum speed limit (Per-Unit)
    float32_t SpdMaxLim;                  //!< Maximum speed limit (Per-Unit)
    float32_t IqMaxLim;                   //!< Maximum Iq limit (Per-Unit)
    float32_t IqInit;                     //!< Initial Iq value (Per-Unit)

    float32_t SpdFbk;                     //!< Speed feedback input (Per-Unit)
    float32_t IqFbk;                      //!< Iq current feedback input (Per-Unit)

    float32_t IqRef;                      //!< Iq current reference output (Per-Unit)
    float32_t InertiaOverFlux;            //!< Estimated J/(1.5*pp*Lambda) (Per-Unit)
    uint16_t Finish;                      //!< Procedure completion flag

    float32_t Vdc_Base;                   //!< DC-link voltage base value
    float32_t Vph_Base;                   //!< Phase voltage base value
    float32_t I_Base;                     //!< Current base value
    float32_t F_Base;                     //!< Frequency base value

    float32_t X_base;                     //!< Reactance base value
    float32_t flux_base;                  //!< Flux linkage base value
    float32_t J_eq_base;                  //!< Equivalent inertia base value
    float32_t J_base;                     //!< Inertia base value
    float32_t Lam_PM;                     //!< PM flux linkage
    float32_t P_Base;                     //!< Power base value
    float32_t Inertia_Final;              //!< Final inertia in physical units
    float32_t Tau_Base;                   //!< Torque constant base value
    float32_t friction_base;              //!< Friction base value
    float32_t inductance_base;            //!< Inductance base value
    float32_t inertia_base;               //!< Inertia base value
    float32_t power_base;                 //!< Power base value
    float32_t torque_base;                //!< Torque base value


} TiTune_J_Obj;

//*****************************************************************************
//
//! brief Defines the PI handle
//
//*****************************************************************************
typedef struct _TiTune_J_Obj_ *TiTune_J_Handle;

//*****************************************************************************

//*****************************************************************************
extern TiTune_J_Handle TiTune_J_init(void *pMemory, const size_t numBytes);

//*****************************************************************************
static inline void TiTune_J_run(TiTune_J_Handle handle,
                                const float32_t Speed_fb,
                                const float32_t iq_fb)
{

    TiTune_J_Obj *obj = (TiTune_J_Obj *)handle;

    /* Initialize some variables. */
    /* This enables safe start and usage of this routine again without resetting CPU */
     if (obj->VariableInit == true)
        {   obj->VariableInit = false;
            obj->ResetRiseFallCounters = true;
            obj->TorqueFound = false;
            obj->TorqueNotFoundError = false;
            obj->IqMinAssigned = false;
            obj->IqMaxAssigned = false;
            obj->EnableCheckingHighTorque = false;
            obj->Prepare4Est = false;
            obj->EstimationEn = false;
            obj->SampleCounterDFT = 0;
            obj->TorqueSign = sgn_POSITIVE;
            obj->MaxIqHitCounter = 0;
            obj->MaxSpdHitCounter = 0;
            obj->MaxSpdHitCounter_Resettable = 0;
            obj->FinishedTestPeriodCounter = 0;
            obj->DownSampleCounter = 0;
            obj->SpdRiseCounter = 0;
            obj->SpdFallCounter = 0;

            obj->SpdRealDFT = 0.0f;
            obj->SpdImagDFT = 0.0f;
            obj->IqRealDFT = 0.0f;
            obj->IqImagDFT = 0.0f;

            obj->IqRef = obj->IqInit;
            obj->Finish = false;
        }

     obj->SpdFbk= Speed_fb/obj->F_Base;
     obj->IqFbk=iq_fb/obj->I_Base;


     /* PART-1 : INCREASE THE COUNTERS */
     if (obj->TorqueSign > 0) {  obj->SpdRiseCounter++;}
     else {  obj->SpdFallCounter++;}

     /* PART-2 : HYSTERESIS CONTROL OF SPEED */
     /* Speed hits the MAXIMUM limit */
     if ( (obj->TorqueSign > 0) && ( ((obj->SpdFbk > obj->SpdMaxLim) && (obj->EstimationEn == false)) || ((obj->SpdRiseCounter > obj->RiseTime4Est) && (obj->EstimationEn == true)) )  )
        {  /* Mode change */
           obj->TorqueSign = sgn_NEGATIVE;

           /* Reset 'MinSearchIq' and 'MaxSearchIq' at the real 1st max speed hit in this procedure. */
           /* If the test Iq should be less than static friction, 'MinSearchIq' needs to be resetted.  */
           obj->MaxSpdHitCounter++;
           if (obj->MaxSpdHitCounter == 1)
              {  obj->IqMinAssigned = false;
                 obj->IqMaxAssigned = false;}
           if (obj->MaxSpdHitCounter > 1)
              {  obj->MaxSpdHitCounter = 2;}  /* Prevent uncontrolled rise. */

           /* Wait 1 max-spd hit before the evaluation. This is a precaution for torque increase in half-way while rising */
           obj->MaxSpdHitCounter_Resettable++;
           if (obj->MaxSpdHitCounter_Resettable > 1)
              {  obj->MaxSpdHitCounter_Resettable = 2; /* Prevent uncontrolled rise. */
                 obj->EnableCheckingHighTorque = true;}
        }

     /* Speed hits the MINIMUM limit */
     if ( (obj->TorqueSign < 0) && ( ((obj->SpdFbk < obj->SpdMinLim) && (obj->EstimationEn == false)) || ((obj->SpdFallCounter > obj->FallTime4Est) && (obj->EstimationEn == true)) )  )
        {  /* Mode change & Reset counter */
           obj->TorqueSign = sgn_POSITIVE;
           obj->ResetRiseFallCounters = true;

           /* Count the finished test periods. */
           if (obj->TorqueFound == true)
              {  obj->FinishedTestPeriodCounter++;
                 if (obj->FinishedTestPeriodCounter == obj->SettlePeriod) {obj->Prepare4Est = true;}
                 if (obj->FinishedTestPeriodCounter == (obj->SettlePeriod+1)) {obj->Finish = true; obj->VariableInit = true;}
              }
        }

     /* PART-3 : ADJUST THE CURRENT/TORQUE LEVEL */
     if (obj->TorqueFound == false)
        {
           /* Static friction precaution. Increase Iq if rise rate is too slow. */
           if (obj->SpdRiseCounter > obj->RiseTimeMax)
              {  obj->ResetRiseFallCounters = true;
                 /* Reset 'obj->MaxSpdHitCounter_Resettable' so that any torque increase, while motor speed is between 'SpdMinLim' and 'SpdMaxLim', does not cause erronous and small Rise Time. */
                 obj->MaxSpdHitCounter_Resettable = 0;
                 obj->EnableCheckingHighTorque = false;

                 obj->MinSearchIq = obj->IqRef;
                 obj->IqMinAssigned = true;

                 if (obj->IqMaxAssigned == false)
                    {
                     obj->IqRef =  2.0f*(obj->IqRef);

                       /* Check limits. */
                       if (obj->IqRef > obj->IqMaxLim)
                          {  obj->IqRef = obj->IqMaxLim;

                             obj->MaxIqHitCounter++;
                             if (obj->MaxIqHitCounter > 1)
                                {  /* Limit to 2. Prevent overflow. */
                                   obj->MaxIqHitCounter = 2;
                                   obj->TorqueFound = true;
                                   obj->TorqueNotFoundError = true;}
                          }
                    }
                 else
                    {
                     obj->IqRef = (obj->MinSearchIq + obj->MaxSearchIq)/2.0f;
                    }
              }

           /* Decrease Iq if rise rate is too fast. */
           if (obj->EnableCheckingHighTorque == true)
              {  obj->EnableCheckingHighTorque = false;
                 if (obj->SpdRiseCounter < obj->RiseTimeMin)
                    {  obj->MaxSearchIq = obj->IqRef;
                       obj->IqMaxAssigned = true;

                       if (obj->IqMinAssigned == false)
                          {
                           obj->IqRef = (obj->IqRef)/2.0f;
                          }
                       else
                          {
                           obj->IqRef = (obj->MinSearchIq + obj->MaxSearchIq)/2.0f;
                          }
                    }

                 else if (obj->SpdRiseCounter < obj->RiseTimeMax)
                    {  obj->TorqueFound = true;}
              }
        }

     /* PART-4 : DIRECT DISCRETE FOURIER TRANSFORM. */
     if (obj->EstimationEn == true)
        {  obj->DownSampleCounter++;
           if (obj->DownSampleCounter == obj->DownSampleRatio)
              {  obj->DownSampleCounter = 0;

                 /* Calculate the angle used in DFT. */
                 obj->SampleCounterDFT++;
                 while (obj->SampleCounterDFT > obj->SampleSizeDFT) /* Although just one period is used in DFT, still it is better to have this 'while' as a precaution. */
                     {  obj->SampleCounterDFT -= obj->SampleSizeDFT;}

                 obj->AngleDFT = obj->SampleCounterDFT*obj->AngleStepDFT;
                 while (obj->AngleDFT > 1.0f)
                     {  obj->AngleDFT -= 1.0f;}
                 while (obj->AngleDFT < 0.0f)
                     {  obj->AngleDFT += 1.0f;}


                 obj->SinDFT = sinf(obj->AngleDFT*MATH_TWO_PI);
                 obj->CosDFT = cosf(obj->AngleDFT*MATH_TWO_PI);

                 /* 'obj->InputGainDFT' is to get DFT amplitude and avoid overflow. */
                 obj->SpdRealDFT += ((obj->SpdFbk* obj->CosDFT)* obj->InputGainDFT);
                 obj->SpdImagDFT -= ((obj->SpdFbk* obj->SinDFT)* obj->InputGainDFT);
                 obj->IqRealDFT += ((obj->IqFbk* obj->CosDFT)* obj->InputGainDFT);
                 obj->IqImagDFT -= ((obj->IqFbk* obj->SinDFT)* obj->InputGainDFT);
              }
         }


     /* PART-5 : PREPARATION FOR ESTIMATION. SET DFT PARAMETERS. */
     if (obj->Prepare4Est == true)
        {  obj->Prepare4Est = false;
           obj->EstimationEn = true;

           /* Just after 'FinishedTestPeriodCounter' is incremented to 'SettlePeriod' when speed hits minimum, save 'SpdFallCounter' and 'SpdRiseCounter' before resetted in PART-7. */
           obj->RiseTime4Est = obj->SpdRiseCounter;
           obj->FallTime4Est = obj->SpdFallCounter;

           /* Down sample if 'SampleSizeDFT' is bigger than 'MaxSampleSizeDFT'. REASON: Prevent numerical precision loss in DFT calculation. */
           obj->SampleSizeDFT = MATH_sat((obj->RiseTime4Est + obj->FallTime4Est), (1UL<<29), 1UL); /*Safety check for over/underflow-free correct working of below code. */

           obj->DownSampleRatio = 1;
           while (obj->SampleSizeDFT > obj->MaxSampleSizeDFT)
              {  obj->SampleSizeDFT = obj->SampleSizeDFT>>1;
                 obj->DownSampleRatio = obj->DownSampleRatio<<1;}

           /* 'InputGainDFT' is to prevent overflow in DFT memories. Divide by (N/2) to limit all variables to 1 */
           obj->AngleStepDFT = (1.0f) / (float)obj->SampleSizeDFT;
           obj->InputGainDFT = 2.0f*(obj->AngleStepDFT);

           /* The Injected Frequency is Per Unitized.. which is to be compensated later in Kp & Ki Computations) */
           obj->InjSpeedFreq = (obj->Nbase/(obj->SampleSizeDFT * obj->DownSampleRatio));
        }


     /* PART-6 : FINISH the ESTIMATION and CALCULATE INERTIA. */
     if (obj->Finish == true)
        {
           /* SIGNAL AMPLITUDE and PHASE CALCULATION */
           /* DFT works only one injection period and selects first harmonic component. */
           obj->IqPhaseDFT = atan2f(obj->IqImagDFT, obj->IqRealDFT)  / MATH_TWO_PI;
           obj->SpdPhaseDFT = atan2f(obj->SpdImagDFT, obj->SpdRealDFT)  / MATH_TWO_PI; /* Maybe we can include speed measurement/estimation delay here. */
           obj->IqMagnDFT =  sqrtf((obj->IqRealDFT * obj->IqRealDFT) + (obj->IqImagDFT * obj->IqImagDFT));
           obj->SpdMagnDFT = sqrtf((obj->SpdRealDFT * obj->SpdRealDFT) + (obj->SpdImagDFT * obj->SpdImagDFT));

          /* Calculate the estimated parameter. No need for DFT register resetting because only one estimation is done here. */
          obj->InertiaOverFlux = fabs((obj->IqMagnDFT/(obj->InjSpeedFreq * obj->SpdMagnDFT))* (sinf(obj->IqPhaseDFT - obj->SpdPhaseDFT)));
        }

     /* PART-7 : RESET COUNTERS. */
     if (obj->ResetRiseFallCounters == true)
        {
         obj->ResetRiseFallCounters = false;
         obj->SpdRiseCounter = 0;
         obj->SpdFallCounter = 0;
        }

     obj->Inertia_Final = obj->InertiaOverFlux * obj->J_eq_base * 3.0f / 4.0f * obj->P_Base * obj->Lam_PM;

     return;
} // end of TiTune_J_run() function

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // end of TITUNE_J_H defines
