//##############################################################################
//##############################################################################

//------------------------------------------------------------------------------
//!
//! \file   libraries/utilities/cpu_time/include/cpu_time.h
//! \brief  Contains the public interface to the
//!         CPU_time (CPU_time) module routines
//!
//------------------------------------------------------------------------------


#ifndef CPU_TIME_H
#define CPU_TIME_H


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
//! \defgroup CPU_TIME CPU_TIME
//! @{
//
//*****************************************************************************

// the includes
#include "ti_math.h"

// system
#include <stdbool.h>  // needed for bool type, true/false
#include <string.h>   // needed for size_t typedef
#include <stdint.h>   // needed for C99 data types


// **************************************************************************
// the defines
#define MOD_INDEX_MAX      16

#define TIME_MOD_CHK00      0
#define TIME_MOD_CHK01      1
#define TIME_MOD_CHK02      2
#define TIME_MOD_CHK03      3
#define TIME_MOD_CHK04      4
#define TIME_MOD_CHK05      5
#define TIME_MOD_CHK06      6
#define TIME_MOD_CHK07      7
#define TIME_MOD_CHK08      8
#define TIME_MOD_CHK09      9
#define TIME_MOD_CHK0A     10
#define TIME_MOD_CHK0B     11
#define TIME_MOD_CHK0C     12
#define TIME_MOD_CHK0D     13
#define TIME_MOD_CHK0E     14
#define TIME_MOD_CHK0F     15

#define TIME_ISR_CHK       TIME_MOD_CHK00
#define TIME_TEST_CHK      TIME_MOD_CHK01
#define TIME_GUI_CHK       TIME_MOD_CHK02
#define TIME_FOC_CHK       TIME_MOD_CHK03
#define TIME_CLARK_CHK     TIME_MOD_CHK04
#define TIME_FAST_CHK      TIME_MOD_CHK05
#define TIME_ESMO_CHK      TIME_MOD_CHK06
#define TIME_PARK_CHK      TIME_MOD_CHK07
#define TIME_TRAJ_CHK      TIME_MOD_CHK08
#define TIME_PI_SPD_CHK    TIME_MOD_CHK09
#define TIME_PI_IDQ_CHK    TIME_MOD_CHK0A
#define TIME_IPARK_CHK     TIME_MOD_CHK0B
#define TIME_SVGEN_CHK     TIME_MOD_CHK0C
#define TIME_PWMDRV_CHK    TIME_MOD_CHK0D
#define TIME_ANGEN_CHK     TIME_MOD_CHK0E
#define TIME_RMS_CHK       TIME_MOD_CHK0F

// **************************************************************************
// the typedefs

//! \brief Defines the CPU usage (CPU_TIME) object
//!
typedef struct _CPU_TIME_Obj_
{
  float32_t cpuWidthRatio;
  uint32_t  timerCntNow;
  uint32_t  timerCntPrev[MOD_INDEX_MAX];

  uint16_t  ctrlPeriodCnt;
  uint16_t  deltaNow[MOD_INDEX_MAX];    // the latest delta count value, cnts
  uint16_t  deltaMin[MOD_INDEX_MAX];    // the minimum delta counts, cnts
  uint16_t  deltaMax[MOD_INDEX_MAX];    // the maximum delta counts, cnts

  uint16_t  pwmCntADC;
  uint16_t  pwmCntISR;

  uint16_t  pwmCntADCMin;
  uint16_t  pwmCntISRMin;

  uint16_t  pwmCntADCMax;
  uint16_t  pwmCntISRMax;

  bool      flag_resetStatus;           // a flag to reset all measured data
} CPU_TIME_Obj;


//! \brief Defines the CPU_TIME handle
//!
typedef struct _CPU_TIME_Obj_ *CPU_TIME_Handle;


// **************************************************************************
// the globals


// **************************************************************************
// the function prototypes

//! \brief     Initializes the CPU TIME (CPU_TIME) object
//! \param[in] pMemory    A pointer to the base address of the object
//! \param[in] numBytes   The object size, bytes
//! \return    The handle to the object
extern CPU_TIME_Handle CPU_TIME_init(void *pMemory,const size_t numBytes);


//! \brief     Resets timer
//! \param[in] handle  The CPU TIME (CPU_TIME) handle
extern void CPU_TIME_reset(CPU_TIME_Handle handle);

//! \brief     Sets the state of the reset stats flag
//! \param[in] handle  The CPU TIME (CPU_TIME) handle
static inline void CPU_TIME_setResetFlag(CPU_TIME_Handle handle)
{
  CPU_TIME_Obj *obj = (CPU_TIME_Obj *)handle;

  obj->flag_resetStatus = true;

  return;
} // end of CPU_TIME_setResetFlag() function

//! \brief     Runs the CPU TIME module
//! \param[in] handle  The CPU TIME (CPU_TIME) handle
//! \param[in] pwmCntADC  The control interrupt period
static inline void
CPU_TIME_setCntADC(CPU_TIME_Handle handle, uint16_t pwmCntADC)
{
  CPU_TIME_Obj *obj = (CPU_TIME_Obj *)handle;

  obj->pwmCntADC = pwmCntADC;

  if(obj->pwmCntADC < obj->pwmCntADCMin)
      obj->pwmCntADCMin = obj->pwmCntADC;

  if(obj->pwmCntADC > obj->pwmCntADCMax)
      obj->pwmCntADCMax = obj->pwmCntADC;

  return;
} // end of CPU_TIME_setCtrlPeriod() function


//! \brief     Runs the CPU TIME module
//! \param[in] handle  The CPU TIME (CPU_TIME) handle
//! \param[in] pwmCntADC  The control interrupt period
static inline void
CPU_TIME_setCntISR(CPU_TIME_Handle handle, uint16_t pwmCntISR)
{
  CPU_TIME_Obj *obj = (CPU_TIME_Obj *)handle;

  obj->pwmCntISR = pwmCntISR;

  if(obj->pwmCntISR < obj->pwmCntISRMin)
      obj->pwmCntISRMin = obj->pwmCntISR;

  if(obj->pwmCntISR > obj->pwmCntISRMax)
      obj->pwmCntISRMax = obj->pwmCntISR;

  return;
} // end of CPU_TIME_setCtrlPeriod() function

//! \brief     Runs the CPU TIME module
//! \param[in] handle  The CPU TIME (CPU_TIME) handle
//! \param[in] ctrlPeriod  The control interrupt period
static inline void
CPU_TIME_setCtrlPeriod(CPU_TIME_Handle handle, uint16_t ctrlPeriod)
{
  CPU_TIME_Obj *obj = (CPU_TIME_Obj *)handle;

  obj->ctrlPeriodCnt = ctrlPeriod * 2;

  return;
} // end of CPU_TIME_setCtrlPeriod() function


//! \brief     Updates the current and previous count values
//! \param[in] handle    The CPU TIME (CPU_TIME) handle
static inline void
CPU_TIME_calcCPUWidthRatio(CPU_TIME_Handle handle)
{
  CPU_TIME_Obj *obj = (CPU_TIME_Obj *)handle;

  float32_t ctrlPeriod = (float32_t)obj->ctrlPeriodCnt;

  obj->cpuWidthRatio = ((float32_t)obj->deltaMax[0]) / ctrlPeriod;

  return;
} // end of CPU_TIME_calcCPUWidthRatio() function


//! \brief     Updates the current and previous count values
//! \param[in] handle    The CPU TIME (CPU_TIME) handle
//! \param[in] timerCnt  The current count value
//! \param[in] modIndex  The current index
static inline void
CPU_TIME_updateCnts(CPU_TIME_Handle handle, const uint32_t cnt, uint16_t modIndex)
{
  CPU_TIME_Obj *obj = (CPU_TIME_Obj *)handle;

  obj->timerCntPrev[modIndex] = cnt;

  if(obj->flag_resetStatus == true)
  {
	  obj->flag_resetStatus = false;

      uint16_t cnt;

      for(cnt = 0; cnt < MOD_INDEX_MAX; cnt++)
      {
          obj->deltaMin[cnt] = 0xFFFF;
          obj->deltaMax[cnt] = 0;
      }

      obj->pwmCntADCMin = 0x3FFF;
      obj->pwmCntISRMin = 0x3FFF;

      obj->pwmCntADCMax = 0;
      obj->pwmCntISRMax = 0;
  }

  return;
} // end of CPU_TIME_updateCnts() function


//! \brief     Runs the CPU TIME module
//! \param[in] handle  The CPU TIME (CPU_TIME) handle
//! \param[in] modIndex  The current index
static inline void
CPU_TIME_run(CPU_TIME_Handle handle, uint32_t count, uint16_t modIndex)
{
  CPU_TIME_Obj *obj = (CPU_TIME_Obj *)handle;

  obj->timerCntNow = count;

  if(modIndex >= MOD_INDEX_MAX)
  {
      return;
  }

  obj->deltaNow[modIndex] =
                    (uint16_t)(obj->timerCntPrev[modIndex] - obj->timerCntNow - 6);

  if(obj->deltaNow[modIndex] > obj->deltaMax[modIndex])
      obj->deltaMax[modIndex] = obj->deltaNow[modIndex];

  if(obj->deltaNow[modIndex] < obj->deltaMin[modIndex])
      obj->deltaMin[modIndex] = obj->deltaNow[modIndex];

  return;
} // end of CPU_TIME_run() function

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

#endif // end of CPU_TIME_H definition
