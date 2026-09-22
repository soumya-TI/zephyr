//##############################################################################
//##############################################################################

//------------------------------------------------------------------------------
//!
//! MotorControl SDK
//!
//! \file   libraries/utilities/angle_gen/source/angle_gen.c
//! \brief  Contains the public interface to the angle compensation
//! \brief  generator (ANGLE_GEN)
//!         module routines
//!
//------------------------------------------------------------------------------


//
// the includes
//
#include "angle_gen.h"


//*****************************************************************************
//
// ANGLE_GEN_init
//
//*****************************************************************************
ANGLE_GEN_Handle ANGLE_GEN_init(void *pMemory, const size_t numBytes)
{
    ANGLE_GEN_Handle handle;

    if(numBytes < sizeof(ANGLE_GEN_Obj))
      return((ANGLE_GEN_Handle)NULL);

    //
    // assign the handle
    //
    handle = (ANGLE_GEN_Handle)pMemory;

    return(handle);
} // end of ANGLE_GEN_init() function

// Sets up parameters for angle generation
void ANGLE_GEN_setParams(ANGLE_GEN_Handle handle, float32_t ctrlPeriod_sec)
{
    ANGLE_GEN_Obj *obj = (ANGLE_GEN_Obj *)handle;

    obj->angleDeltaFactor = ctrlPeriod_sec * MATH_TWO_PI;
    obj->angleDelta_rad = 0.0;
    obj->angle_rad = 0.0;

    return;
} // end of ANGLE_COMP_setParams() function

// end of the file
