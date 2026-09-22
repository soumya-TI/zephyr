/*
 * halGateDriver.h
 *
 *  Created on: Jul 8, 2025
 *      Author: a0132555
 */

#ifndef HAL_GATEDRIVERINTERFACE_INCLUDE_HALGATEDRIVER_H_
#define HAL_GATEDRIVERINTERFACE_INCLUDE_HALGATEDRIVER_H_

#include <stdint.h>

/* Syscfg includes */
#include "ti_sdk_dl_config.h"

#include "hal.h"

// ti_math.h internally includes <math.h> and fastrts_tmu.h for TMU acceleration
#include "ti_math.h"

//! \brief Defines a float32_t type for ARM M33 compatibility
#ifndef float32_t
typedef float float32_t;
#endif

//! \brief      Defines the hardware abstraction layer (HAL) for HAL - Gate Driver Dependent Functions
//! \details    The HAL object contains all functions from the FOC to Hal Interface

typedef struct _HAL_GD_vTable_t_
{
    void (*initGateDriver)     (void const *handle);
    void (*enableGateDriver)   (void const *handle);

} HAL_GD_vTable_t;


typedef struct _HAL_GD_DATA_Obj_
{
   float32_t    gdMaxCurrentA;
   float32_t    gdCurrentSF;
   float32_t    gdCurrSensDelay;
   float32_t    gdCurrSampDelay;
   float32_t    gdCurrOffset;
   float32_t    gdCurrSign;
   float32_t    gdOverVolVal;
   float32_t    gdUnderVolVal;
   float32_t    gdVolSF;
   float32_t    gdPhVoltOffsetSF;
   float32_t    gdVolFilterPoleFreqRPS;

}HAL_GD_DATA_Obj;

typedef struct _HAL_GD_DATA_Obj_ *HAL_GD_DATA_Handle;


/**
 * @brief Defines the hardware abstraction layer (HAL) interface for Field-Oriented Control (FOC)
 *
 * This structure represents the interface between the motor controller and the FOC algorithm.
 * It defines the functions and parameters that are required to control the motor using FOC.
 */
typedef struct _HAL_GD_Interface_t_
{
    /**
     * @brief Pointer to the virtual table of functions for the FOC algorithm
     */
    const HAL_GD_vTable_t *vGDTable;

    /**
     * @brief Handle to the FOC algorithm instance
     */
    const void *halGDHandle;

    const HAL_GD_DATA_Obj *gdDataHandle;

}HAL_GD_Interface_t;

static inline void HAL_initGateDriver(const HAL_GD_Interface_t *interfaceHandle)
{
    interfaceHandle->vGDTable->initGateDriver(interfaceHandle->halGDHandle);
}
static inline void HAL_enableGateDriver(const HAL_GD_Interface_t *interfaceHandle)
{
    interfaceHandle->vGDTable->enableGateDriver(interfaceHandle->halGDHandle);
}

#endif /* HAL_GATEDRIVERINTERFACE_INCLUDE_HALGATEDRIVER_H_ */
