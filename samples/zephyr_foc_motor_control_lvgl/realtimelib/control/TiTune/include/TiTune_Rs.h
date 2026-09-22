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


//! @file   TiTune_Rs.h
//! @brief  TI Motor Control Library - Stator Resistance Tuning Module
//! @details Implements automatic stator resistance and dead-time distortion voltage
//!          identification. Applies two distinct DC currents at D-axis (zero torque)
//!          and measures voltage drop to calculate Rs and dead-time compensation.

/*
 *      Estimation of stator resistance and distortion voltage due to dead-time.
 *      The method applies two distinct and high-amplitude DC currents at d-axis, resulting zero torque.
 *      Results include motor resistance as well as cable and IGBT resistances. So, it is a good representation of real system.
 *
 */


//***************************************************************************************************************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//***************************************************************************************************************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//***************************************************************************************************************************************************************************
#include <stdlib.h>
#include <stdint.h>
#include "ti_math.h"

//#include "user.h"

//***************************************************************************************************************************************************************************
//! @brief Defines the Stator Resistance Tuning object
//! @details Structure containing all parameters and state variables for automatic
//!          stator resistance and dead-time distortion voltage identification
//***************************************************************************************************************************************************************************

typedef struct _TiTune_Rs_Obj_
{
    bool VariableInit;              //!< Variable initialization enable flag
    float32_t AveragerEnable;       //!< Averaging enable flag after current stabilization
    float32_t TimeOutError;         //!< Timeout error flag - current failed to stabilize
    float32_t Mode;                 //!< Test mode: 1=TestCurrent1, 2=TestCurrent2
    float32_t CtrlCycleCounter;     //!< Control cycle execution counter
    float32_t IdFbk_Average;        //!< Accumulated Id feedback sum (Per-Unit)
    float32_t VdRef_Average;        //!< Accumulated Vd reference sum (Per-Unit)
    float32_t IdFbkAve_Mode1;       //!< Averaged Id from Mode 1 (Per-Unit)
    float32_t VdRefAve_Mode1;       //!< Averaged Vd from Mode 1 (Per-Unit)
    float32_t IdErrFiltered;        //!< Filtered Id error for stabilization detection (Per-Unit)

    uint32_t  AveragingSize;        //!< Number of samples to average
    float32_t Wait4SettleCurrent;   //!< Samples to wait for current loop settling
    float32_t MaxTime4Settle;       //!< Maximum allowed settling time before timeout
    float32_t TestCurrent1;         //!< First test current amplitude (Per-Unit)
    float32_t TestCurrent2;         //!< Second test current amplitude (Per-Unit)
    float32_t IdErrLim4Settle;      //!< Id error threshold for stabilization (Per-Unit)
    float32_t LPF_IdErr_Gain;       //!< LPF gain for Id error filtering
    float32_t Vdc_Base;             //!< DC-link voltage base value
    float32_t Vph_Base;             //!< Phase voltage base value
    float32_t I_Base;               //!< Current base value
    float32_t F_Base;               //!< Frequency base value
    int32_t i12;                    //!< Internal counter

    float32_t IdFbk;                //!< D-axis current feedback input (Per-Unit)
    float32_t VdRef;                //!< D-axis voltage reference input (Per-Unit)
    float32_t VdistGain;            //!< Angle-dependent distortion gain input
    float32_t VdistGain1;           //!< Secondary angle-dependent distortion gain

    float32_t IdRef;                //!< D-axis current reference output (Per-Unit)
    float32_t Rs;                   //!< Measured stator resistance output (Per-Unit)
    float32_t Vdist;                //!< Measured distortion voltage output (Per-Unit)
    float32_t Finish;               //!< Identification completion flag

    float32_t X_base;               //!< Reactance base value
    float32_t Rs_Final;             //!< Final Rs in physical units (Ohms)
    float32_t impedance_base;       //!< Impedance base value
} TiTune_Rs_Obj;

//***************************************************************************************************************************************************************************

//***************************************************************************************************************************************************************************
//      Defines the TiTune_Rs handle
//***************************************************************************************************************************************************************************

typedef struct _TiTune_Rs_Obj_ *TiTune_Rs_Handle;

//***************************************************************************************************************************************************************************

//***************************************************************************************************************************************************************************
//      extern TiTune_Rs_Handle
//***************************************************************************************************************************************************************************
extern TiTune_Rs_Handle TiTune_Rs_init(void *pMemory, const size_t numBytes);

//***************************************************************************************************************************************************************************
static inline void TiTune_Rs_run(TiTune_Rs_Handle handle, const float32_t v_dist_gain, const float32_t id_fb, const float32_t vd_fb)
{
    TiTune_Rs_Obj *obj = (TiTune_Rs_Obj *)handle;

    /* Initialize some variables. */
    /* This enables safe start and usage of this routine again without resetting CPU */

    if (obj->VariableInit == true){
        obj->VariableInit = false;
        obj->AveragerEnable = 0.0f;
        obj->TimeOutError = 0.0f;
        obj->Mode = 1.0f;
        obj->CtrlCycleCounter = 0.0f;

        /* Safety check to prevent excessive averaging size */
        obj->AveragingSize = MATH_sat(obj->AveragingSize, 1000000UL, 1UL);

        obj->IdFbk_Average = 0.0f;
        obj->VdRef_Average = 0.0f;

        obj->IdRef = obj->TestCurrent1;
        obj->IdErrFiltered = obj->TestCurrent1; /* Precaution for pre-mature pass from STEP-1 to STEP-2. */
        obj->Finish = 0.0f;
           }

    obj->VdistGain = v_dist_gain;
    obj->IdFbk = id_fb/obj->I_Base;
    obj->VdRef = vd_fb/obj->Vph_Base;

    /* Id control error is filtered by a 1st order Low-Pass Filter (Backward Euler discretized) because the error will be used in comparison, noise may cause wrong decision. */
    obj->IdErrFiltered += (obj->LPF_IdErr_Gain * ((obj->IdRef - obj->IdFbk) - obj->IdErrFiltered));
    /* Inject periodic test current and count the completed periods */
    obj->CtrlCycleCounter++;


    /* STEP-1 : Wait until Id current stabilizes to the test current */
    if ((obj->AveragerEnable == 0.0f) && (obj->CtrlCycleCounter >= obj->Wait4SettleCurrent))
       {  /* Option-1 : Only time-based decision making. */
          /* v.AveragerEnable = TRUE; */
          /* v.CtrlCycleCounter = 0; */

          /* Option-2 : Checking Id error vanishes as a second parameter on top of the timer 'Wait4SettleCurrent' for decision making. */
          if (MATH_abs(obj->IdErrFiltered) < obj->IdErrLim4Settle)
             {  obj->AveragerEnable = 1.0f;
                obj->CtrlCycleCounter = 0.0f;}

          /* Part of Option-2. If Id cannot settle in a maximum allowed settling time, then 'TimeOutError' is set TRUE. */
          if (obj->CtrlCycleCounter >= obj->MaxTime4Settle)
             {  obj->TimeOutError = 1.0f;
                obj->AveragerEnable = 1.0f;
                obj->CtrlCycleCounter = 0.0f;}
             }

    /* STEP-2 : Estimation. Simple accumulation for float32 (no scaling needed). */
    if (obj->AveragerEnable == 1.0f)
       {  obj->IdFbk_Average += obj->IdFbk;
          obj->VdRef_Average += obj->VdRef;
       }


    /* STEP-3: Calculate average current and voltage values. */
    if ( (obj->AveragerEnable == 1.0f) && (obj->CtrlCycleCounter == (obj->AveragingSize - 1.0f)))
       {  obj->AveragerEnable = 0.0f;
          obj->CtrlCycleCounter = 0.0f;

          /* Calculate average by dividing accumulated sum by number of samples */
          float32_t inv_N = 1.0f / (float32_t)obj->AveragingSize;
          obj->IdFbk_Average *= inv_N;
          obj->VdRef_Average *= inv_N;

          /* Store voltage and current averages of Mode=1. Then calculate Rs and Vdist using voltage and current averages of Mode=1 and Mode=2, just after Mode=2 finishes. */
          if (obj->Mode == 1.0f)
             {  obj->Mode = 2.0f;
                obj->IdRef = obj->TestCurrent2;
                obj->IdErrFiltered = (obj->TestCurrent2 - obj->TestCurrent1);    /* Precaution for pre-mature pass from STEP-1 to STEP-2. */

                obj->IdFbkAve_Mode1 = obj->IdFbk_Average;
                obj->VdRefAve_Mode1 = obj->VdRef_Average;
                obj->IdFbk_Average = 0.0f;
                obj->VdRef_Average = 0.0f;
             }
          else if (obj->Mode == 2.0f)
             {  obj->VariableInit = true;
                obj->Finish = 1.0f;
                obj->IdRef = 0.0f;

                obj->Rs = ((obj->VdRef_Average - obj->VdRefAve_Mode1) / (obj->IdFbk_Average - obj->IdFbkAve_Mode1));
                obj->Vdist = ((obj->VdRef_Average - (obj->Rs * obj->IdFbk_Average)) / obj->VdistGain);
             }
       }

    return;
}

//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif
