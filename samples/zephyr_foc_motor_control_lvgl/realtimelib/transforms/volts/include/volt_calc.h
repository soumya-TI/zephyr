//#############################################################################
//
// FILE:   volt_calc.h
//
// TITLE:  ARM M33 Proportional-Integral (PI) controller library
//         (floating point)
//
//#############################################################################

#ifndef VOLT_CAL_H
#define VOLT_CAL_H

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
//! \defgroup VOLT_CALC VOLT_CALC
//! @{
//
//*****************************************************************************

#include "types.h"
#include "ti_math.h"

//*****************************************************************************
//
//! \brief Defines the PHVOLC controller object
//
//*****************************************************************************
typedef struct _PHVOLC_Obj_
{
    float32_t  VphaseA;			// Output: Phase voltage phase A (pu)
    float32_t  VphaseB;			// Output: Phase voltage phase B (pu)
    float32_t  VphaseC;			// Output: Phase voltage phase C (pu)
    float32_t  Valpha;			// Output: Stationary d-axis phase voltage (pu)
    float32_t  Vbeta;  			// Output: Stationary q-axis phase voltage (pu)
    uint16_t   flagOutOfPhase;  // Out of Phase adjustment (0 or 1)
} PHVOLC_Obj;

//*****************************************************************************
//
//! \brief Defines the PHVOLC handle
//
//*****************************************************************************
typedef struct _PHVOLC_Obj_ *PHVOLC_Handle;

//-----------------------------------------------------------------------------
// OutOfPhase = 0 for the out of phase correction if
// MfuncV1 is out of phase with PWM1,
// MfuncV2 is out of phase with PWM3,
// MfuncV3 is out of phase with PWM5,
// otherwise, set 0 if their phases are correct.

#define VC_ONE_THIRD   0.33333333333333f
#define VC_TWO_THIRD   0.66666666666667f
#define VC_INV_SQRT3   0.57735026918963f


// Phase Voltage Calculation
static inline void
PHVOLC_run(PHVOLC_Handle handle, float32_t Vdcbus,
           MATH_Vec3 *pVabc_pu, MATH_Vec2 *pVab_V)
{
    PHVOLC_Obj *obj = (PHVOLC_Obj *)handle;
    float32_t Vtemp;

    // Scale the incomming Modulation functions with the DC bus voltage value
    // and calculate the 3 Phase voltages
    Vtemp = Vdcbus * VC_ONE_THIRD;

    obj->VphaseA = Vtemp * (pVabc_pu->value[0] * 2.0f -
                            pVabc_pu->value[1] - pVabc_pu->value[2]);

    obj->VphaseB = Vtemp * (pVabc_pu->value[1] * 2.0f -
                            pVabc_pu->value[0] - pVabc_pu->value[2]);

    obj->VphaseC = Vtemp * (pVabc_pu->value[2] * 2.0f -
                            pVabc_pu->value[1] - pVabc_pu->value[0]);

    if(obj->flagOutOfPhase == 1)
    {
        obj->VphaseA = -obj->VphaseA;
        obj->VphaseB = -obj->VphaseB;
        obj->VphaseC = -obj->VphaseC;
    }

    // Voltage transformation (a,b,c)  ->  (Alpha,Beta)
    obj->Valpha = obj->VphaseA;
    obj->Vbeta  = (obj->VphaseA + obj->VphaseB * 2.0f) * VC_INV_SQRT3;

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

#endif // end of VOLT_CAL_H defines
