//#############################################################################
//
// FILE:   volt_calc.c
//
// TITLE:  Calculate the phase voltage
//
//#############################################################################

#include "volt_calc.h"

//*****************************************************************************
//
// PI_init
//
//*****************************************************************************
PHVOLC_Handle
HVOLC_init(void *pMemory, const size_t numBytes)
{
    PHVOLC_Handle handle;

    if((int16_t)numBytes < (int16_t)sizeof(PHVOLC_Obj))
    {
        return((PHVOLC_Handle)NULL);
    }

    // Assign the handle
    handle = (PHVOLC_Handle)pMemory;

    return(handle);
} // end of HVOLC_init() function

// end of file
