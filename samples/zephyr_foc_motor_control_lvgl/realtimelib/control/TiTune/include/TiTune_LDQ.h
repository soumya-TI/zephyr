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

/*
 *  If building with a C++ compiler, make all of the definitions in this header
 *  have a C binding.
 */
#ifdef __cplusplus
extern "C"
{
#endif

//! @file   TiTune_LDQ.h
//! @brief  TI Motor Control Library - Ld/Lq Inductance Tuning Module
//! @details Implements automatic D-axis and Q-axis inductance identification through
//!          high-frequency voltage injection and DFT analysis. Scans rotor position
//!          to find minimum/maximum inductance and initial rotor angle.

/**
 *  \defgroup TITUNE_API_MODULE APIs for TiTune
 *  \ingroup  CONTROL_MODULE
 *
 *  Here is the list of TiTune function APIs
 *  @{
 *
 *  \file           TiTune.h
 *  \brief          Contains TiTune functions implementation for self-commissioning
 */

// the includes

#include <stdlib.h>
#include "ti_math.h"
#include "types.h"

//*****************************************************************************
//
//! @brief Defines the Ld/Lq Tuning object
//! @details Structure containing all parameters and state variables for automatic
//!          D-axis and Q-axis inductance identification
//
//*****************************************************************************
typedef struct _TiTune_LDQ_Obj_
{
    bool VariableInit;                  //!< Variable initialization enable flag
    float32_t VoltageFound;             //!< Suitable injection voltage found flag
    float32_t VoltageNotFound;          //!< Voltage search failure error flag
    float32_t VolMinAssigned;           //!< Min search voltage assigned flag
    float32_t VolMaxAssigned;           //!< Max search voltage assigned flag
    float32_t ExecuteEnable;            //!< Calculation execution enable flag
    float32_t InjVolHalfPrdSampleNo;    //!< Current injection half-period sample count
    float32_t InjSampleCounter;         //!< Injection sample counter
    float32_t InjPrdCounter;            //!< Completed injection period counter
    float32_t FirstLdqEstCheck;         //!< Inductance calculation execution counter
    float32_t FirstVolMaxLimHitCheck;   //!< Max voltage limit hit counter
    float32_t FirstFreqMinLimHitCheck;  //!< Min frequency limit hit counter
    float32_t InjVolAngleStep;          //!< Injection voltage angle step (Per-Unit)
    float32_t InjVolMagn;               //!< Injected voltage magnitude (Per-Unit)
    float32_t InjVolFreq;               //!< Injected voltage frequency (Per-Unit)
    float32_t InjVolAngle;              //!< Injection voltage angle (Per-Unit)
    float32_t InjVolAngle_ZOHComp;      //!< ZOH-compensated injection angle (Per-Unit)
    float32_t ElecThetaUnsat;           //!< Unsaturated electrical angle (Per-Unit)
    float32_t MinSearchVol;             //!< Minimum voltage search value (Per-Unit)
    float32_t MaxSearchVol;             //!< Maximum voltage search value (Per-Unit)
    float32_t InputGainDFT;             //!< DFT input scaling coefficient
    float32_t CosDFT;                   //!< Cosine value for DFT computation
    float32_t SinDFT;                   //!< Sine value for DFT computation
    float32_t VdRealDFT;                //!< Real part of D-axis voltage DFT (Per-Unit)
    float32_t VdImagDFT;                //!< Imaginary part of D-axis voltage DFT (Per-Unit)
    float32_t IdRealDFT;                //!< Real part of D-axis current DFT (Per-Unit)
    float32_t IdImagDFT;                //!< Imaginary part of D-axis current DFT (Per-Unit)
    float32_t IqRealDFT;                //!< Real part of Q-axis current DFT (Per-Unit)
    float32_t IqImagDFT;                //!< Imaginary part of Q-axis current DFT (Per-Unit)
    float32_t VdMagnDFT;                //!< D-axis voltage peak magnitude (Per-Unit)
    float32_t IdMagnDFT;                //!< D-axis current peak magnitude (Per-Unit)
    float32_t IqMagnDFT;                //!< Q-axis current peak magnitude (Per-Unit)
    float32_t IsMagn;                   //!< Stator current vector magnitude (Per-Unit)
    float32_t Lpu;                      //!< Calculated inductance at current angle (Per-Unit)
    float32_t VdPhaseDFT;               //!< D-axis voltage phase angle (Per-Unit)
    float32_t IdPhaseDFT;               //!< D-axis current phase angle (Per-Unit)

    float32_t InjVolHalfPrdSampleNoMax; //!< Max injection half-period samples (min frequency)
    float32_t InjVolHalfPrdSampleNoInit;//!< Initial injection half-period samples
    float32_t InjSettlePrd;             //!< Injection periods for settling
    float32_t AngIntgGain;              //!< Angle integration gain: Fbase*Tsample
    float32_t ElecThetaStep;            //!< Electrical angle increment (Per-Unit)
    float32_t InjVolMagnInit;           //!< Initial injection voltage magnitude (Per-Unit)
    float32_t IsMagnMinLim;             //!< Minimum current magnitude limit (Per-Unit)
    float32_t IsMagnMaxLim;             //!< Maximum current magnitude limit (Per-Unit)
    float32_t InjVolMagnMaxLim;         //!< Maximum injection voltage limit (Per-Unit)
    float32_t Vdc_Base;                 //!< DC-link voltage base value
    float32_t Vph_Base;                 //!< Phase voltage base value
    float32_t I_Base;                   //!< Current base value
    float32_t F_Base;                   //!< Frequency base value

    float32_t Udc;                      //!< DC-link voltage feedback input (Per-Unit)
    float32_t IdFbk;                    //!< D-axis current feedback input (Per-Unit)
    float32_t IqFbk;                    //!< Q-axis current feedback input (Per-Unit)

    float32_t InjVol;                   //!< Injected voltage reference output (Per-Unit)
    float32_t ElecTheta;                //!< Electrical angle output (Per-Unit)
    float32_t LminApprox;               //!< Approximate minimum inductance (Per-Unit)
    float32_t LmaxApprox;               //!< Approximate maximum inductance (Per-Unit)
    float32_t InitialRotorAngle;        //!< Approximate initial rotor angle (Per-Unit)
    float32_t Finish;                   //!< Procedure completion flag

    // Below arrays removed to reduce memory usage size. Meant for debugging routine.
//    float32_t Ind_Total[160];
//    float32_t Ang_Total[160];
//    uint16_t i12;

    float32_t X_base;                   //!< Reactance base value
    float32_t flux_base;                //!< Flux linkage base value
    float32_t Lam_PM;                   //!< PM flux linkage
    float32_t inductance_base;          //!< Inductance base value

    float32_t ld_Final;                 //!< Final Ld in physical units
    float32_t lq_Final;                 //!< Final Lq in physical units


} TiTune_LDQ_Obj;

//*****************************************************************************
//
//! brief Defines the TiTune handle
//
//*****************************************************************************
typedef struct _TiTune_LDQ_Obj_ *TiTune_LDQ_Handle;

//*****************************************************************************

//*****************************************************************************
// the function prototypes

//*****************************************************************************
//
//! brief     Initializes the TiTune
//!
//! param[in] pMemory   A pointer to the memory for the TiTune object
//!
//! param[in] numBytes  The number of bytes allocated for the TiTune
//!
//! return    The TiTune object handle
//
//*****************************************************************************
extern TiTune_LDQ_Handle TiTune_LDQ_init(void *pMemory, const size_t numBytes);

//*****************************************************************************
//!
//! \brief     Run the TiTune function
//!
//! \param[in] handle       The TiTune handle
//!
//! \param[in] vdc_ref      The DC link voltage reference value
//!
//! \param[in] id_fb        The feedback id value
//!
//! \param[in] iq_fb        The feedback iq value
//!
//*****************************************************************************
static inline void TiTune_LDQ_run(TiTune_LDQ_Handle handle,
                              const float32_t vdc_ref,
                              const float32_t id_fb,
                              const float32_t iq_fb)
{
    TiTune_LDQ_Obj *obj = (TiTune_LDQ_Obj *)handle;

    if (obj->VariableInit == true)
       {
       obj->VariableInit = false;
       obj->VoltageFound = 0.0f;
       obj->VoltageNotFound = 0.0f;
       obj->VolMinAssigned = 0.0f;
       obj->VolMaxAssigned = 0.0f;
       obj->ExecuteEnable = 0.0f;

       obj->InjPrdCounter = 0.0f;
       obj->InjSampleCounter = 0.0f;
       obj->FirstLdqEstCheck = 0.0f;
       obj->FirstVolMaxLimHitCheck = 0.0f;
       obj->FirstFreqMinLimHitCheck = 0.0f;

       obj->InjVolHalfPrdSampleNo = obj->InjVolHalfPrdSampleNoInit;
       obj->InjVolAngleStep = (1.0f / (2.0f * obj->InjVolHalfPrdSampleNo));

       obj->InjVolMagn = obj->InjVolMagnInit;
       obj->InjVolFreq = obj->InjVolAngleStep / obj->AngIntgGain;
       obj->InjVolAngle = 0.0f;
       obj->InjVolAngle_ZOHComp = 0.0f;
       obj->InputGainDFT = 2.0f * obj->InjVolAngleStep;

       obj->VdRealDFT = 0.0f;
       obj->VdImagDFT = 0.0f;
       obj->IdRealDFT = 0.0f;
       obj->IdImagDFT = 0.0f;
       obj->IqRealDFT = 0.0f;
       obj->IqImagDFT = 0.0f;
       obj->ElecThetaUnsat = 0.0f;

       obj->ElecTheta = 0.0f;
       obj->Finish = 0.0f;

//       obj->i12 = 0;
       }

    obj->Udc = vdc_ref / obj->Vdc_Base;

    obj->IdFbk = id_fb / obj->I_Base;
    obj->IqFbk = iq_fb / obj->I_Base;

/* DISCRETE FOURIER TRANSFORM: Find the amplitude and phase of the voltage and current at the injection frequency. */
/* Multiplication by 'InputGainDFT' is to get DFT amplitude and equivalent to dividing by N/2. 'InputGainDFT' is updated when frequency changes.*/
    if (obj->InjPrdCounter == obj->InjSettlePrd)
       {
        obj->CosDFT = cosf(obj->InjVolAngle * MATH_TWO_PI);
        obj->SinDFT = sinf(obj->InjVolAngle * MATH_TWO_PI);

        obj->VdRealDFT += obj->InjVolMagn * cosf(obj->InjVolAngle * MATH_TWO_PI) * obj->Udc * obj->CosDFT * obj->InputGainDFT;
        obj->VdImagDFT -= obj->InjVolMagn * cosf(obj->InjVolAngle * MATH_TWO_PI) * obj->Udc * obj->SinDFT * obj->InputGainDFT;

        obj->IdRealDFT += obj->IdFbk * obj->CosDFT * obj->InputGainDFT;
        obj->IdImagDFT -= obj->IdFbk * obj->SinDFT * obj->InputGainDFT;

        obj->IqRealDFT += obj->IqFbk * obj->CosDFT * obj->InputGainDFT;
        obj->IqImagDFT -= obj->IqFbk * obj->SinDFT * obj->InputGainDFT;
       }

/* INJECTION PERIOD HANDLER: Enables signal detection by DFT and inductance calculation. */
/* In this ctrl cycle, the last sample of the injected voltage period  is being applied. */
/* At the end of this ctrl cycle, there will be zero-crossing of the current due to almost 90 degree phase shift at high frequency. Thus, if V or f changes now, no transient is seen. */
    if (obj->InjSampleCounter == (2.0f * obj->InjVolHalfPrdSampleNo))
       {
        obj->InjSampleCounter = 0.0f;
        obj->InjPrdCounter++;

        /* Just after the injection period for DFT finishes, enable execution. */
        if (obj->InjPrdCounter == (obj->InjSettlePrd + 1.0f))
        {
            obj->InjPrdCounter = 0.0f;
            obj->ExecuteEnable = 1.0f;
        }
       }
//
//
    /* MAKE CALCULATIONS */
    if (obj->ExecuteEnable == 1.0f)
       {  obj->ExecuteEnable = 0.0f;

       obj->VdMagnDFT = sqrtf((obj->VdRealDFT * obj->VdRealDFT) + (obj->VdImagDFT * obj->VdImagDFT));
       obj->VdPhaseDFT = atan2f(obj->VdImagDFT,obj->VdRealDFT);

            /* D-axis current amplitude and phase. */
       obj->IdMagnDFT = sqrtf((obj->IdRealDFT * obj->IdRealDFT) + (obj->IdImagDFT * obj->IdImagDFT));
       obj->IdPhaseDFT = atan2f(obj->IdImagDFT, obj->IdRealDFT);

            /* Q-axis current amplitude. */
       obj->IqMagnDFT = sqrtf((obj->IqRealDFT * obj->IqRealDFT) + (obj->IqImagDFT * obj->IqImagDFT));

          /* Reset DFT variables. */
       obj->VdRealDFT = 0.0f;
       obj->VdImagDFT = 0.0f;
       obj->IdRealDFT = 0.0f;
       obj->IdImagDFT = 0.0f;
       obj->IqRealDFT = 0.0f;
       obj->IqImagDFT = 0.0f;

          /* Stator current vector amplitude. */
       obj->IsMagn = ((obj->IdMagnDFT* obj->IdMagnDFT) + (obj->IqMagnDFT* obj->IqMagnDFT));

          /* If 'IsMagn' leaves the allowed range at any electrical angle, find a new injection signal. */
          if ((obj->VoltageFound == 1.0f) && ((obj->IsMagn < obj->IsMagnMinLim) || (obj->IsMagn > obj->IsMagnMaxLim)))
             {

             obj->VoltageFound = 0.0f;
             obj->VolMinAssigned = 0.0f;
             obj->VolMaxAssigned = 0.0f;
             obj->MinSearchVol = 0.0f;
             obj->MaxSearchVol = 0.0f;}


          /* 2- INJECTION SIGNAL SELECTION */
          /* Determine the injection voltage magnitude and frequency.  */
          if (obj->VoltageFound == 0.0f)
             {
                if (obj->IsMagn < obj->IsMagnMinLim)
                   {  obj->MinSearchVol = obj->InjVolMagn;
                   obj->VolMinAssigned = 1.0f;

                      if (obj->VolMaxAssigned == 0.0f)
                         {
                          obj->InjVolMagn = 2.0f*(obj->InjVolMagn);

                            /* Check limits. */
                            if (obj->InjVolMagn > obj->InjVolMagnMaxLim)
                               {  obj->InjVolMagn = obj->InjVolMagnMaxLim;
                               obj->FirstVolMaxLimHitCheck++;

                                  /* If max voltage is not enough, then decrease the frequency. */
                                  if (obj->FirstVolMaxLimHitCheck > 1.0f)
                                     {  /* To prevent uncontrolled rise and overflow, limit 'FirstVolMaxLimHitCheck' to 2 which is higher than 1. So, it still satisfies above IF. */
                                      obj->FirstVolMaxLimHitCheck = 2.0f;
                                        /* Reset VolMin/Max and start-over again if frequency change occurs. */
                                      obj->VolMinAssigned = 0.0f;
                                      obj->VolMaxAssigned = 0.0f;
                                        /* InjVolMagn = InjVolMagnInit; */ /* Check this line, either use or not. */

                                      obj->InjVolHalfPrdSampleNo = 2.0f*obj->InjVolHalfPrdSampleNo;

                                      obj->InjVolAngleStep = (obj->InjVolAngleStep/2.0f);
                                      obj->InjVolFreq = (obj->InjVolFreq/2.0f);
                                      obj->InputGainDFT = (obj->InputGainDFT/2.0f);

                                        if (obj->InjVolHalfPrdSampleNo > obj->InjVolHalfPrdSampleNoMax)
                                           {
                                            obj->InjVolHalfPrdSampleNo = obj->InjVolHalfPrdSampleNoMax;
                                           obj->InjVolAngleStep = ((1.0f)/ (2.0f*obj->InjVolHalfPrdSampleNo));
                                           obj->InjVolFreq = (obj->InjVolAngleStep/ obj->AngIntgGain);
                                           obj->InputGainDFT = (2.0f*obj->InjVolAngleStep);

                                           obj->FirstFreqMinLimHitCheck++;
                                              if (obj->FirstFreqMinLimHitCheck > 1.0f)
                                                 {
                                                  /* To prevent uncontrolled rise and overflow, limit 'FirstFreqMinLimHitCheck' to 2 which is higher than 1. So, it still satisfies above IF. */
                                                  obj->FirstFreqMinLimHitCheck = 2.0f;
                                                  obj->VoltageFound = 1.0f;
                                                  obj->VoltageNotFound = 1.0f;} /* DONT FORGET TO USE THIS ERROR MESSAGE. */
                                           }
                                      }
                                }
                          }
                       else
                          {
                           obj->InjVolMagn = (obj->MinSearchVol + obj->MaxSearchVol) / 2.0f;}
                   }

                else if (obj->IsMagn > obj->IsMagnMaxLim)
                   {  obj->MaxSearchVol = obj->InjVolMagn;
                   obj->VolMaxAssigned = 1.0f;

                      if (obj->VolMinAssigned == 0.0f)
                         {
                          obj->InjVolMagn = (obj->InjVolMagn) / 2.0f;}
                      else
                         {
                          obj->InjVolMagn = (obj->MinSearchVol + obj->MaxSearchVol) / 2.0f;}
                   }
                else
                   {
                    obj->VoltageFound = 1.0f;
                   }
             }

          /* 3- LD, LQ, THETA CALCULATION */
          /* Search for Ld, Lq, and initial rotor position. Scan 180 electrical degrees in rotor with small angle steps. */
          /* If Part-2 decides VoltageFound, then code directly enter here. No need to repeat the test. */
          if (obj->VoltageFound == 1.0f)
             {
              /* Calculate inductance. */
              obj->Lpu = (((obj->VdMagnDFT)/(obj->InjVolFreq* obj->IdMagnDFT))* sinf(obj->VdPhaseDFT - obj->IdPhaseDFT));

              // Below arrays removed to reduce memory usage size. Meant for debugging routine.
//              obj->Ind_Total[obj->i12]=obj->Lpu;
//              obj->Ang_Total[obj->i12]=obj->ElecTheta;
//              obj->i12++;

              obj->FirstLdqEstCheck++;
                if (obj->FirstLdqEstCheck == 1.0f)
                   {  obj->LminApprox = obj->Lpu;
                   obj->LmaxApprox = obj->Lpu;
                   obj->InitialRotorAngle = obj->ElecTheta;}

                /* To prevent uncontrolled rise and overflow, limit 'FirstLdqEstCheck' to 2 which is higher than 1. So, it still satisfies above IF-statement. */
                if (obj->FirstLdqEstCheck > 1.0f)
                   {  obj->FirstLdqEstCheck = 2.0f;}

                // Can average Lpu for SPSM where Ld/Lq are the same
                if (obj->Lpu > obj->LmaxApprox)
                   {  obj->LmaxApprox = obj->Lpu;}
                if (obj->Lpu < obj->LminApprox)
                   {  obj->LminApprox = obj->Lpu;
                   obj->InitialRotorAngle = obj->ElecTheta;}

                obj->ElecThetaUnsat += obj->ElecThetaStep;
                obj->ElecTheta = MATH_sat(obj->ElecThetaUnsat, 0.5f, 0.0f);
             }
       }

    /* Compute the angle for injection sinusoidal voltage. */
    /* In order to synthesize real cosine voltage appearing at the motor, the injection angle starts from 'InjVolAngleStep'. */
    obj->InjVolAngle = (obj->InjSampleCounter * obj->InjVolAngleStep);
    /* Saturate the angle within (0, 1) */
    while (obj->InjVolAngle > 1.0f)
        {  obj->InjVolAngle -= 1.0f;}
    while (obj->InjVolAngle < -1.0f)
        {  obj->InjVolAngle += 1.0f;}
    obj->InjSampleCounter++;

    /* The addition of InjVolAngleStep is to compensate the phase delay due to the ZOH effect of PWM. So, motor experiences 1.0f cosine voltage. */
    /* Notice that both CMPA update from PWM shadow registers and SOCA/interrupt execution are initiated at TBCTR = PRD. This is important for phase difference between voltage and current. */
    obj->InjVolAngle_ZOHComp = obj->InjVolAngle + (obj->InjVolAngleStep / 2.0f);
    while (obj->InjVolAngle_ZOHComp > 1.0f)
        {
        obj->InjVolAngle_ZOHComp -= 1.0f;
        }
    while (obj->InjVolAngle_ZOHComp < -1.0f)
        {
        obj->InjVolAngle_ZOHComp += 1.0f;
        }

    /* Calculate injection voltage. Put this part after Goertzel IIR part because this voltage will be applied during next ctrl cycle. */
    obj->InjVol = (obj->InjVolMagn* cosf(obj->InjVolAngle_ZOHComp * MATH_TWO_PI)) * vdc_ref / sqrtf(3.0f);


    /* Stop */
    if (obj->ElecThetaUnsat > obj->ElecTheta)
       {
        obj->Finish = 1.0f;
          obj->VariableInit = true;
       }

    return;
} // end of TiTune_LDQ_run() function

/**
 *  Mark the end of the C bindings section for C++ compilers.
 */

#ifdef __cplusplus
}
#endif
