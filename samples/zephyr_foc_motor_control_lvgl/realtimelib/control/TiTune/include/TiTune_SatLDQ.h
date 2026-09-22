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

//! @file   TiTune_SatLDQ.h
//! @brief  TI Motor Control Library - Saturated Ld/Lq Tuning Module
//! @details Implements automatic saturated inductance identification across multiple
//!          operating points. Characterizes differential inductances Ld(Id) and Lq(Iq)
//!          through small-signal AC injection superimposed on DC bias currents.

#ifndef TITUNE_SATLDQ_H
#define TITUNE_SATLDQ_H

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

#define LDQ_TEST_NUMBER (4)         // Parameter: Number. Inductance LUT size.

#define LdqEstStep_START                          (1)
#define LdqEstStep_NEGATIVE_CURRENT_ESTIMATION    (2)
#define LdqEstStep_POSITIVE_CURRENT_ESTIMATION    (3)
#define LdqEstStep_FINISH                         (4)
#define LdqEstStep_OPERATING_POINT_CHANGE         (5)

#define sgn_POSITIVE (+1)
#define sgn_NEGATIVE (-1)

#define D_AXIS (1)
#define Q_AXIS (2)

#define MtrType_INDUCTION               (1)
#define MtrType_IPMSM                   (2)
#define MtrType_SYNRM                   (3)

/*************************************************************************************************************************************************************************************/
//! @brief Defines the Saturated Ld/Lq Tuning object
//! @details Structure containing all parameters and state variables for automatic
//!          saturated inductance characterization across multiple operating points
/*************************************************************************************************************************************************************************************/
typedef struct _TiTune_SatLDQ_Obj_
{
    bool VariableInit;                 //!< Variable initialization enable flag
    float32_t ExecuteEnable;           //!< Parameter calculation enable flag
    float32_t InjSineEnable;           //!< Sinusoidal current injection enable flag
    int16_t CurrentSign;               //!< Current sign: +1 (positive) or -1 (negative)
    uint16_t Axis;                     //!< Active axis: D_AXIS=1, Q_AXIS=2
    uint16_t OpPtNo;                   //!< Current magnetic operating point number
    uint16_t EstStep;                  //!< Estimation routine step number
    uint32_t CtrlCycleCounter;         //!< Control cycle execution counter
    uint32_t InjSampleCounter;         //!< Injection sample counter
    uint32_t InjSinePrdCounter;        //!< Completed injection period counter
    uint32_t InjCurHalfPrdSampleNo;    //!< AC injection half-period sample count
    uint32_t dcCurHalfPrdSampleNo;     //!< DC bias half-period sample count

    float32_t InjCurAngle;             //!< Injected AC current angle (Per-Unit)
    float32_t InjCurAngleStep;         //!< AC current angle step (Per-Unit)
    float32_t InputGainDFT;            //!< DFT input scaling coefficient
    float32_t InjCurFreq;              //!< Injected current frequency (Per-Unit)
    float32_t VmagnDFT;                //!< Voltage peak magnitude from DFT (Per-Unit)
    float32_t VphaseDFT;               //!< Voltage phase angle from DFT (Per-Unit)
    float32_t ImagnDFT;                //!< Current peak magnitude from DFT (Per-Unit)
    float32_t IphaseDFT;               //!< Current phase angle from DFT (Per-Unit)
    float32_t CosDFT;                  //!< Cosine value for DFT computation
    float32_t SinDFT;                  //!< Sine value for DFT computation
    float32_t VrealDFT;                //!< Real part of voltage DFT (Per-Unit)
    float32_t VimagDFT;                //!< Imaginary part of voltage DFT (Per-Unit)
    float32_t IrealDFT;                //!< Real part of current DFT (Per-Unit)
    float32_t IimagDFT;                //!< Imaginary part of current DFT (Per-Unit)
    float32_t Lest;                    //!< Calculated differential inductance (Per-Unit)

    uint16_t MotorType;                //!< Motor type: IM=1, IPMSM=2, SynRM=3
    uint32_t Nbase;                    //!< Base sample size: Fsampling/Fbase
    uint32_t InjSettleWait_PrdNo;      //!< AC injection periods for settling
    uint32_t dcSettleWait_SampleNo;    //!< DC bias samples for settling
    float32_t InjCurFreqInit;          //!< Initial AC injection frequency (Per-Unit)
    float32_t InjCurMagn;              //!< AC injection current amplitude (Per-Unit)
    float32_t IrefDCStep;              //!< DC bias current step size (Per-Unit)

    float32_t Ifbk;                    //!< Axis current feedback input (Per-Unit)
    float32_t Vref;                    //!< Axis voltage reference input (Per-Unit)

    float32_t Finish;                  //!< Identification completion flag
    float32_t Iref;                    //!< Axis current reference output (Per-Unit)
    float32_t Lsigma [LDQ_TEST_NUMBER+1];   //!< Leakage inductance vs current (IM) (Per-Unit)
    float32_t Ld [2 * LDQ_TEST_NUMBER+1];   //!< D-axis inductance vs Id (Per-Unit)
    float32_t Lq [2 * LDQ_TEST_NUMBER+1];   //!< Q-axis inductance vs Iq (Per-Unit)

    float32_t Ld_Final [2 * LDQ_TEST_NUMBER+1];  //!< Final Ld(Id) in physical units
    float32_t Lq_Final [2 * LDQ_TEST_NUMBER+1];  //!< Final Lq(Iq) in physical units

    float32_t Vdc_Base;                //!< DC-link voltage base value
    float32_t Vph_Base;                //!< Phase voltage base value
    float32_t I_Base;                  //!< Current base value
    float32_t F_Base;                  //!< Frequency base value

    float32_t X_base;                  //!< Reactance base value
    float32_t flux_base;               //!< Flux linkage base value
    float32_t Lam_PM;                  //!< PM flux linkage
    float32_t inductance_base;         //!< Inductance base value

} TiTune_SatLDQ_Obj;

/*****************************************************************************/
//  Defines the TiTune_SatLDQ handle
/*****************************************************************************/

typedef struct _TiTune_SatLDQ_Obj_ *TiTune_SatLDQ_Handle;

/*****************************************************************************/

extern TiTune_SatLDQ_Handle TiTune_SatLDQ_init(void *pMemory, const size_t numBytes);

/*****************************************************************************/
static inline void TiTune_SatLDQ_run(TiTune_SatLDQ_Handle handle, const float32_t Vd_fb, const float32_t Vq_fb, const float32_t id_fb, const float32_t iq_fb)
{
    TiTune_SatLDQ_Obj *obj = (TiTune_SatLDQ_Obj *)handle;

    /* Initialize some variables. */
        /* This enables safe start and usage of this routine again without resetting CPU. */

    if (obj->VariableInit == true)
           {  obj->VariableInit = false;
              obj->ExecuteEnable = 0.0f;
              obj->InjSineEnable = 0.0f;
              obj->OpPtNo = 0;
              obj->Axis = D_AXIS; /* Initialize. */
              obj->EstStep = LdqEstStep_POSITIVE_CURRENT_ESTIMATION; /* To shorthen the test. As Idc is zero at the beginning (OpPtNo is initialized to zero), no need for neg current.*/

              obj->CtrlCycleCounter = 0;
              obj->InjSampleCounter = 0;
              obj->InjSinePrdCounter = 0;


              obj->InjCurHalfPrdSampleNo = ((obj->Nbase) * (1.0f / obj->InjCurFreqInit)) / (2.0f); /* Here, '_IQdiv' won't overflow because InjCurFreq > 1 p.u. mostly. */

              if ( (((obj->Nbase * (1.0f / obj->InjCurFreqInit)) / 2.0f)  - obj->InjCurHalfPrdSampleNo) >= 0.5f ) /* Round the float variable into integer. Useful only in FLOAT_MATH */
              {  obj->InjCurHalfPrdSampleNo++;}


              /* Calculate the count number for half period of square wave current: DC settling count + (AC settling prd + DFT period)*AC Period count */
              obj->dcCurHalfPrdSampleNo = obj->dcSettleWait_SampleNo + (obj->InjSettleWait_PrdNo + 1) * (2 * obj->InjCurHalfPrdSampleNo);

              obj->InjCurAngleStep = 1.0f / (2.0f * obj->InjCurHalfPrdSampleNo);

              obj->InjCurFreq = obj->InjCurAngleStep / (1.0f / obj->Nbase);

              obj->InjCurAngle = 0.0f;
              obj->VrealDFT = 0.0f;
              obj->VimagDFT = 0.0f;
              obj->IrealDFT = 0.0f;
              obj->IimagDFT = 0.0f;

              /* 'InputGainDFT' is to prevent overflow in DFT memories. Normally we need to divide by (N/2). (1UL<<(30-GLOBAL_Q)) is to use all 32-bit capacity. */
              obj->InputGainDFT = 2.0f * obj->InjCurAngleStep;

              obj->Finish = 0.0f;
           }

           if ( (obj->MotorType == MtrType_INDUCTION) || ( ((obj->MotorType == MtrType_IPMSM) || (obj->MotorType == MtrType_SYNRM)) && (obj->Axis == D_AXIS) ) )
           {
               obj->Ifbk = id_fb / obj->I_Base;
               obj->Vref = Vd_fb / obj->Vph_Base;
            }

            if ( ((obj->MotorType == MtrType_IPMSM) || (obj->MotorType == MtrType_SYNRM)) && (obj->Axis == Q_AXIS) )
            {
                obj->Ifbk = iq_fb / obj->I_Base; ;
                obj->Vref = Vq_fb / obj->Vph_Base ;
             }

    /* DIRECT DISCRETE FOURIER TRANSFORM. */
       /* 'v.InputGainDFT' is to prevent overflow in the memories. */
       if (obj->InjSinePrdCounter == obj->InjSettleWait_PrdNo)
          {
           obj->SinDFT = sinf(obj->InjCurAngle * MATH_TWO_PI);
           obj->CosDFT = cosf(obj->InjCurAngle * MATH_TWO_PI);

           obj->VrealDFT += (obj->Vref * obj->CosDFT * obj->InputGainDFT);
           obj->VimagDFT -= (obj->Vref * obj->SinDFT * obj->InputGainDFT);
           obj->IrealDFT += (obj->Ifbk * obj->CosDFT * obj->InputGainDFT);
           obj->IimagDFT -= (obj->Ifbk * obj->SinDFT * obj->InputGainDFT);
          }

       /* INJECTION PERIOD HANDLER: Enables signal detection by DFT and inductance calculation. */
       /* In this ctrl cycle, the last sample of the injected current period  is being applied. */
       if ( obj->InjSampleCounter == (2 * obj->InjCurHalfPrdSampleNo) )
          {
           obj->InjSampleCounter = 0;
           obj->InjSinePrdCounter++;
             /* Just after the injection period for DFT finishes, enable execution. */
             if (obj->InjSinePrdCounter == (obj->InjSettleWait_PrdNo + 1))
                {  obj->InjSinePrdCounter = 0;
                   obj->ExecuteEnable = 1.0f;}
          }


       /* PARAMETER CALCULATIONS */
       if (obj->ExecuteEnable == 1.0f)
          {
          obj->ExecuteEnable = 0.0f;

          obj->VphaseDFT = atan2f(obj->VimagDFT, obj->VrealDFT) - (obj->InjCurAngleStep / 2.0f); /* Half-sample ZOH delay is taken into account. */
          obj->IphaseDFT = atan2f(obj->IimagDFT, obj->IrealDFT);
          obj->VmagnDFT = sqrtf( (obj->VrealDFT * obj->VrealDFT) + (obj->VimagDFT * obj->VimagDFT));
          obj->ImagnDFT = sqrtf( (obj->IrealDFT * obj->IrealDFT) + (obj->IimagDFT * obj->IimagDFT));

             /* Reset DFT variables for re-use in the next operating point. */
          obj->VrealDFT = 0.0f;
          obj->VimagDFT = 0.0f;
          obj->IrealDFT = 0.0f;
          obj->IimagDFT = 0.0f;

          obj->Lest = ((obj->VmagnDFT / (obj->ImagnDFT * obj->InjCurFreq)) * sinf(obj->VphaseDFT - obj->IphaseDFT) );
             if (obj->MotorType == MtrType_INDUCTION)
                {  obj->Lsigma[obj->OpPtNo] = obj->Lest;}
             else if ( (obj->MotorType == MtrType_IPMSM) || (obj->MotorType == MtrType_SYNRM) )
                { if (obj->Axis == D_AXIS)
                     {  obj->Ld[LDQ_TEST_NUMBER + obj->CurrentSign*(int16_t)obj->OpPtNo] = obj->Lest;
                        obj->Ld_Final[LDQ_TEST_NUMBER + obj->CurrentSign*(int16_t)obj->OpPtNo] = obj->Lest * obj->inductance_base;}
                /* Be careful with multiplication. Use type-casting because CurrentSign is 'int16' and OpPtNo 'Uint16'. */
                  else
                     {
                       obj->Lq[LDQ_TEST_NUMBER + obj->CurrentSign*(int16_t)obj->OpPtNo] = obj->Lest;
                       obj->Lq_Final[LDQ_TEST_NUMBER + obj->CurrentSign*(int16_t)obj->OpPtNo] = obj->Lest * obj->inductance_base;} /* Be careful with multiplication. Use type-casting because CurrentSign is 'int16' and OpPtNo 'Uint16'. */
                }
          }

       /* ESTIMATION STEPS */
       obj->CtrlCycleCounter++;
       if (obj->EstStep == LdqEstStep_START)
          {  obj->CurrentSign = sgn_POSITIVE;
             if (obj->CtrlCycleCounter == (obj->dcCurHalfPrdSampleNo>>1) )
                {  obj->CtrlCycleCounter = 0;
                   obj->EstStep = LdqEstStep_NEGATIVE_CURRENT_ESTIMATION;}
          }

       else if (obj->EstStep == LdqEstStep_NEGATIVE_CURRENT_ESTIMATION)
          {  obj->CurrentSign = sgn_NEGATIVE;
             if (obj->CtrlCycleCounter >= obj->dcSettleWait_SampleNo)
                {  obj->InjSineEnable = 1.0f;}
             if (obj->CtrlCycleCounter == obj->dcCurHalfPrdSampleNo)
                {  obj->CtrlCycleCounter = 0;
                   obj->InjSineEnable = 0.0f;
                   obj->EstStep = LdqEstStep_POSITIVE_CURRENT_ESTIMATION;}
          }

       else if (obj->EstStep == LdqEstStep_POSITIVE_CURRENT_ESTIMATION)
          {  obj->CurrentSign = sgn_POSITIVE;
             if (obj->CtrlCycleCounter >= obj->dcSettleWait_SampleNo)
                {  obj->InjSineEnable = 1.0f;}
             if (obj->CtrlCycleCounter == obj->dcCurHalfPrdSampleNo)
                {  obj->CtrlCycleCounter = 0;
                   obj->InjSineEnable = 0.0f;

                   if (obj->MotorType == MtrType_INDUCTION)
                      {  obj->EstStep = LdqEstStep_OPERATING_POINT_CHANGE;}
                   else if ( (obj->MotorType == MtrType_IPMSM) || (obj->MotorType == MtrType_SYNRM) )
                      {  obj->EstStep = LdqEstStep_FINISH;}
                }
          }

       else if (obj->EstStep == LdqEstStep_FINISH)
          {  obj->CurrentSign = sgn_NEGATIVE;
             if (obj->CtrlCycleCounter >= obj->dcCurHalfPrdSampleNo)
                {  obj->CurrentSign = sgn_POSITIVE;}
             if (obj->CtrlCycleCounter == (obj->dcCurHalfPrdSampleNo + (obj->dcCurHalfPrdSampleNo>>1)) )
                {  obj->CtrlCycleCounter = 0;
                   obj->EstStep = LdqEstStep_OPERATING_POINT_CHANGE;}
          }

       /* Operating point change. */
       if (obj->EstStep == LdqEstStep_OPERATING_POINT_CHANGE)
          {
             if (obj->MotorType == MtrType_INDUCTION)
                {  obj->EstStep = LdqEstStep_POSITIVE_CURRENT_ESTIMATION;}
             else if ( (obj->MotorType == MtrType_IPMSM) || (obj->MotorType == MtrType_SYNRM) )
                {  obj->EstStep = LdqEstStep_START;}

             /* STOP */
             obj->OpPtNo++;
             if (obj->OpPtNo > LDQ_TEST_NUMBER)
                {  if (obj->MotorType == MtrType_INDUCTION)
                      {  obj->Finish = 1.0f;
                         obj->VariableInit = true;}
                   else if ( (obj->MotorType == MtrType_IPMSM) || (obj->MotorType == MtrType_SYNRM) )
                      {  if (obj->Axis == D_AXIS)
                            {  obj->Axis = Q_AXIS;
                               obj->EstStep = LdqEstStep_POSITIVE_CURRENT_ESTIMATION;
                               obj->OpPtNo = 0;}
                         else
                            {  obj->Finish = 1.0f;
                               obj->VariableInit = true;}
                       }
                }
          }


       /* ASSIGN REFERENCE CURRENT FOR SMALL SIGNAL ANALYSIS AROUND AN OPERATING POINT. */
       /* Compute the angle. Saturate the angle rate within (0, 1).  */
       if (obj->InjSineEnable == 1.0f)
          {  obj->InjSampleCounter++;}
       else
          {  obj->InjSampleCounter = 0;}

       obj->InjCurAngle = obj->InjSampleCounter * obj->InjCurAngleStep;
       while (obj->InjCurAngle > 1.0f)
             {  obj->InjCurAngle -= 1.0f;}
       while (obj->InjCurAngle < 0.0f)
             {  obj->InjCurAngle += 1.0f;}

       /* Be careful with multiplication between 'CurrentSign' and 'OpPtNo'. Use type-casting (int16) for OpPtNo because CurrentSign is 'int16' and OpPtNo is 'Uint16'. */
       obj->Iref = ( (obj->CurrentSign*(int16_t)obj->OpPtNo) * obj->IrefDCStep) + ( obj->InjCurMagn * sinf(obj->InjCurAngle * MATH_TWO_PI));
       /* Increase ac amplitude the zero dc case */
       if (obj->OpPtNo == 0) {  obj->Iref = ( (LDQ_TEST_NUMBER * (obj->IrefDCStep / 2.0f)) * sinf(obj->InjCurAngle * MATH_TWO_PI));}


    return;
} // end of TiTune_SatLDQ_run() function

//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // end of TITUNE_SATLDQ_H defines
