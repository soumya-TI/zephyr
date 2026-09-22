/*
 * Copyright (c) 2021 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef HAL_CONTROLLERINTERFACE_INCLUDE_HAL_FOCINTERFACE_H_
#define HAL_CONTROLLERINTERFACE_INCLUDE_HAL_FOCINTERFACE_H_

// drivers
// TODO: Replace with M33-compatible device header
// #include "device.h"
#include "hal_obj.h"
#include "hal.h"
#include "userParams.h"
#include "dclink_ss.h"
#include "halGateDriver.h"


//! \brief      Defines the HAL_MTR handle
//! \details    The HAL_MTR handle is a pointer to a HAL_MTR object.  In all
//!             HAL_MTR functions, the HAL_MTR handle is passed so that the
//!             function knows what peripherals are to be accessed.
//!
// platforms
//! \brief      Defines the HAL object
//!
typedef struct _HAL_FOC_Data_Obj_
{

  MotorNum_e     motorNum;

  uint16_t       numCurrentSensors;  //!< the number of current sensors
  uint16_t       numVoltageSensors;  //!< the number of voltage sensors

  Board_Kit_e    boardKit;

  CURRENTSEN_Type_e currentSenseType;

  HAL_PWMData_t pwmData;

  bool flagEnableOffsetCalc;

  float32_t *pOverCurLimA;         // Pointer to over current limit in Amps
  float32_t ipdCurrentLimit;
  HAL_ADCData_t adcData;

  const HAL_GD_DATA_Obj  *gdDataHandle;

  uint32_t ecapIPDPulseTime;

} HAL_FOC_Data_Obj;

typedef struct _HAL_FOC_Data_Obj_ *HAL_FOC_Data_Handle;

typedef struct _HAL_FOC_Config_t
{
    HAL_INT_Config_t intCfg;

    const  HAL_PWM_Config_t pwmCfg;

    const  HAL_ADC_Result_Config_t adcCurrentCfg[4];

    const  HAL_ADC_Result_Config_t adcVoltageCfg[4];

    const HAL_3S_CMP_Config_t cmpCfg;

    const  DCLINK_SS_Handle dclinkHandle;  // Handle to Single object in Single Shunt Configurations.

    const HAL_ECAP_Config_t ecapIPDConfig;

}HAL_FOC_Config_t;

typedef struct _HAL_FOC_Config_t *HAL_FOC_Config_Handle;

//! \brief      Defines the hardware abstraction layer (HAL) for HAL - FOC Dependent Functions
//! \details    The HAL object contains all functions from the FOC to Hal Interface

typedef struct _HAL_FOC_vTable_t_
{
    void (*SetPhaseVoltages)        (HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle);
    void (*ReadInverterVoltages)    (HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle);
    void (*ReadPhaseCurrents)       (HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle);
    void (*SetCompOCLimits)         (HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle);
    void (*Init)                    (HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle);
    void (*RunOffsetCal)            (HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle);
    void (*ConfigPWM)               (HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle);
    void (*clearMtrFaultStatus)     (HAL_FOC_Config_t *configHandle);
    void (*ipdPulseConfig)          (VOLTAGE_VECTOR  voltageVector, HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle);
    uint8_t (*ipdPulseDone)            (HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle);
    void (*ipdInit)                 (HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle);
    void (*ipdExit)                 (HAL_FOC_Config_t *configHandle ,HAL_FOC_Data_Obj *dataHandle);

} HAL_FOC_vTable_t;

typedef struct _HAL_FOC_vTable_t_ *HAL_FOC_vTable_Handle_t;
/**
 * @brief Defines the hardware abstraction layer (HAL) interface for Field-Oriented Control (FOC)
 *
 * This structure represents the interface between the motor controller and the FOC algorithm.
 * It defines the functions and parameters that are required to control the motor using FOC.
 */
typedef struct _HAL_FOC_Interface_t_
{
    /**
     * @brief Pointer to the virtual table of functions for the FOC algorithm
     */
    const HAL_FOC_vTable_t *vFOCTable;

    /**
     * @brief Pointer to the hal configurations for the FOC algorithm
     */
    const HAL_FOC_Config_t *halFOCConfigHandle;

    /**
     * @brief pointer to the FOC Data object
     */
    HAL_FOC_Data_Obj *halFOCDataHandle;

} HAL_FOC_Interface_t;

#define DIRECT_FUNCTION_CALLS
// Direct function call optimization for time-critical ISR paths
// Define DIRECT_FUNCTION_CALLS to bypass vtable and eliminate 0.5us overhead
#ifdef DIRECT_FUNCTION_CALLS
    #ifdef MOTOR_SINGLE_SHUNT
        extern void HAL_1S_FOC_SetPhaseVoltages(HAL_FOC_Config_t *configHandle, HAL_FOC_Data_Obj *dataHandle);
        extern void HAL_1S_FOC_ReadInverterVoltages(HAL_FOC_Config_t *configHandle, HAL_FOC_Data_Obj *dataHandle);
        extern void HAL_1S_FOC_ReadPhaseCurrents(HAL_FOC_Config_t *configHandle, HAL_FOC_Data_Obj *dataHandle);

        #define HAL_SetPhaseVoltages(interfaceHandle) \
            HAL_1S_FOC_SetPhaseVoltages((HAL_FOC_Config_t *)(interfaceHandle)->halFOCConfigHandle, \
                                        (HAL_FOC_Data_Obj *)(interfaceHandle)->halFOCDataHandle)

        #define HAL_ReadInverterVoltages(interfaceHandle) \
            HAL_1S_FOC_ReadInverterVoltages((HAL_FOC_Config_t *)(interfaceHandle)->halFOCConfigHandle, \
                                            (HAL_FOC_Data_Obj *)(interfaceHandle)->halFOCDataHandle)

        #define HAL_ReadPhaseCurrents(interfaceHandle) \
            HAL_1S_FOC_ReadPhaseCurrents((HAL_FOC_Config_t *)(interfaceHandle)->halFOCConfigHandle, \
                                         (HAL_FOC_Data_Obj *)(interfaceHandle)->halFOCDataHandle)

    #elif defined(MOTOR_THREE_SHUNT)
        extern void HAL_3S_FOC_SetPhaseVoltages(HAL_FOC_Config_t *configHandle, HAL_FOC_Data_Obj *dataHandle);
        extern void HAL_3S_FOC_ReadInverterVoltages(HAL_FOC_Config_t *configHandle, HAL_FOC_Data_Obj *dataHandle);
        extern void HAL_3S_FOC_ReadPhaseCurrents(HAL_FOC_Config_t *configHandle, HAL_FOC_Data_Obj *dataHandle);

        #define HAL_SetPhaseVoltages(interfaceHandle) \
            HAL_3S_FOC_SetPhaseVoltages((HAL_FOC_Config_t *)(interfaceHandle)->halFOCConfigHandle, \
                                        (HAL_FOC_Data_Obj *)(interfaceHandle)->halFOCDataHandle)

        #define HAL_ReadInverterVoltages(interfaceHandle) \
            HAL_3S_FOC_ReadInverterVoltages((HAL_FOC_Config_t *)(interfaceHandle)->halFOCConfigHandle, \
                                            (HAL_FOC_Data_Obj *)(interfaceHandle)->halFOCDataHandle)

        #define HAL_ReadPhaseCurrents(interfaceHandle) \
            HAL_3S_FOC_ReadPhaseCurrents((HAL_FOC_Config_t *)(interfaceHandle)->halFOCConfigHandle, \
                                         (HAL_FOC_Data_Obj *)(interfaceHandle)->halFOCDataHandle)

    #else
        #error "Must define MOTOR_SINGLE_SHUNT or MOTOR_THREE_SHUNT when using DIRECT_FUNCTION_CALLS"
    #endif

#else
    // Standard vtable version (flexible but has 0.5us overhead per call)
    __attribute__((always_inline))
    static inline void HAL_SetPhaseVoltages(const HAL_FOC_Interface_t *interfaceHandle)
    {
        interfaceHandle->vFOCTable->SetPhaseVoltages((HAL_FOC_Config_t *)interfaceHandle->halFOCConfigHandle,
                                                     (HAL_FOC_Data_Obj *)interfaceHandle->halFOCDataHandle);
    }

    __attribute__((always_inline))
    static inline void HAL_ReadInverterVoltages(const HAL_FOC_Interface_t *interfaceHandle)
    {
        interfaceHandle->vFOCTable->ReadInverterVoltages((HAL_FOC_Config_t *)interfaceHandle->halFOCConfigHandle,
                                                         (HAL_FOC_Data_Obj *)interfaceHandle->halFOCDataHandle);
    }

    __attribute__((always_inline))
    static inline void HAL_ReadPhaseCurrents(const HAL_FOC_Interface_t *interfaceHandle)
    {
        interfaceHandle->vFOCTable->ReadPhaseCurrents((HAL_FOC_Config_t *)interfaceHandle->halFOCConfigHandle,
                                                      (HAL_FOC_Data_Obj *)interfaceHandle->halFOCDataHandle);
    }
#endif
static inline void HAL_SetCompOCLimits(const HAL_FOC_Interface_t *interfaceHandle)
{
    interfaceHandle->vFOCTable->SetCompOCLimits((HAL_FOC_Config_t *)interfaceHandle->halFOCConfigHandle,
                                                (HAL_FOC_Data_Obj *)interfaceHandle->halFOCDataHandle);
}
static inline void HAL_Init(const HAL_FOC_Interface_t *interfaceHandle)
{
    interfaceHandle->vFOCTable->Init((HAL_FOC_Config_t *)interfaceHandle->halFOCConfigHandle,
                                     (HAL_FOC_Data_Obj *)interfaceHandle->halFOCDataHandle);
}
static inline void HAL_RunOffsetCal(const HAL_FOC_Interface_t *interfaceHandle)
{
    interfaceHandle->vFOCTable->RunOffsetCal((HAL_FOC_Config_t *)interfaceHandle->halFOCConfigHandle,
                                             (HAL_FOC_Data_Obj *)interfaceHandle->halFOCDataHandle);
}
static inline void HAL_ConfigPWM(const HAL_FOC_Interface_t *interfaceHandle)
{
    interfaceHandle->vFOCTable->ConfigPWM((HAL_FOC_Config_t *)interfaceHandle->halFOCConfigHandle,
                                          (HAL_FOC_Data_Obj *)interfaceHandle->halFOCDataHandle);
}
static inline void HAL_clearMtrFaultStatus(const HAL_FOC_Interface_t *interfaceHandle)
{
    interfaceHandle->vFOCTable->clearMtrFaultStatus((HAL_FOC_Config_t *)interfaceHandle->halFOCConfigHandle);
}

static inline void HAL_IPDForcePWMLow(const HAL_PWM_Config_t *pwmCfg)
{
    HAL_forcePWMLow(pwmCfg);
}

static inline void HAL_ipdInit(const HAL_FOC_Interface_t *interfaceHandle)
{
    interfaceHandle->vFOCTable->ipdInit((HAL_FOC_Config_t *)interfaceHandle->halFOCConfigHandle,
                                     (HAL_FOC_Data_Obj *)interfaceHandle->halFOCDataHandle);
}
static inline void HAL_ipdPulseConfig(VOLTAGE_VECTOR voltageVector , const HAL_FOC_Interface_t *interfaceHandle)
{
    interfaceHandle->vFOCTable->ipdPulseConfig((VOLTAGE_VECTOR) voltageVector, (HAL_FOC_Config_t *)interfaceHandle->halFOCConfigHandle,
                                     (HAL_FOC_Data_Obj *)interfaceHandle->halFOCDataHandle);
}
static inline void HAL_ipdExit(const HAL_FOC_Interface_t *interfaceHandle)
{
    interfaceHandle->vFOCTable->ipdExit((HAL_FOC_Config_t *)interfaceHandle->halFOCConfigHandle,
                                     (HAL_FOC_Data_Obj *)interfaceHandle->halFOCDataHandle);
}
static inline uint8_t HAL_ipdPulseDone(const HAL_FOC_Interface_t *interfaceHandle)
{
    return(interfaceHandle->vFOCTable->ipdPulseDone((HAL_FOC_Config_t *)interfaceHandle->halFOCConfigHandle,
                                     (HAL_FOC_Data_Obj *)interfaceHandle->halFOCDataHandle));
}
#endif /* HAL_CONTROLLERINTERFACE_INCLUDE_HAL_FOCINTERFACE_H_ */
