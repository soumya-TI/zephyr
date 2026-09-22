//##############################################################################
//##############################################################################

//------------------------------------------------------------------------------
//!
//! \file   libraries/utilities/cpu_time/source/cpu_usage.c
//! \brief  Portable C fixed point code.  These functions define the
//!         CPU usage time (CPU_TIME) module routines
//!
//------------------------------------------------------------------------------


// **************************************************************************
// the includes

#include "cpu_time.h"


// **************************************************************************
// the globals


// **************************************************************************
// the functions

// Initializes the CPU TIME (CPU_TIME) object
CPU_TIME_Handle CPU_TIME_init(void *pMemory,const size_t numBytes)
{
    CPU_TIME_Handle handle;

    if(numBytes < sizeof(CPU_TIME_Obj))
    {
        return((CPU_TIME_Handle)NULL);
    }

    // assign the handle
    handle = (CPU_TIME_Handle)pMemory;

    return(handle);
} // end of CPU_TIME_init() function

// Resets timer count
void CPU_TIME_reset(CPU_TIME_Handle handle)
{
    CPU_TIME_Obj *obj = (CPU_TIME_Obj *)handle;
    uint16_t cnt;

    for(cnt = 0; cnt < MOD_INDEX_MAX; cnt++)
    {
        obj->deltaNow[cnt] = 0;
        obj->deltaMin[cnt] = 0x7FFF;
        obj->deltaMax[cnt] = 0;
    }

    obj->pwmCntADC = 0;
    obj->pwmCntISR = 0;

    obj->pwmCntADCMin = 0x3FFF;
    obj->pwmCntISRMin = 0x3FFF;

    obj->pwmCntADCMax = 0;
    obj->pwmCntISRMax = 0;

    obj->flag_resetStatus = true;

    return;
} // end of CPU_TIME_setParams() function

// end of file
