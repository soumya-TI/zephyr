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

#ifndef HAL_H
#define HAL_H


//! \file   hal.h
//! \brief  Contains public interface to various functions related
//!         to the HAL object
//!

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

/* Syscfg includes */
#include "ti_sdk_dl_config.h"

// platforms
#include "hal_obj.h"

#include "dclink_ss.h"

// the globals
extern HAL_Handle    halHandle;
extern HAL_Obj       hal;

//! \brief Enumeration for the CPU Timer
//!
typedef enum
{
    HAL_CPU_TIMER0 = 0,  //!< Select CPU Timer0
    HAL_CPU_TIMER1 = 1,  //!< Select CPU Timer1
    HAL_CPU_TIMER2 = 2   //!< Select CPU Timer2
} HAL_CPUTimerNum_e;

/*! @brief VOLTAGE_VECTOR */
typedef enum
{
    /*! IPD vector AC */
    VOLTAGE_VECTOR_AC = 0,
    /*! IPD vector CA */
    VOLTAGE_VECTOR_CA,
    /*! IPD vector BC */
    VOLTAGE_VECTOR_BC,
    /*! IPD vector CB */
    VOLTAGE_VECTOR_CB,
    /*! IPD vector BA */
    VOLTAGE_VECTOR_BA,
    /*! IPD vector AB */
    VOLTAGE_VECTOR_AB,
    /*! Max number of IPD vectors */
    VOLTAGE_VECTOR_MAX
}VOLTAGE_VECTOR;

// **************************************************************************
// the function prototypes

//! \brief      Enables the ADC interrupts
//! \details    Enables the ADC interrupt in the PIE, and CPU.  Enables the
//!             interrupt to be sent from the ADC peripheral.
//! \param[in]  handle  The hardware abstraction layer (HAL) handle
extern void HAL_enableCtrlInts(const HAL_INT_Config_t *handle);

//! \brief     Disables global interrupts
extern void HAL_disableGlobalInts();

//! \brief     Enables global interrupts
extern void HAL_enableGlobalInts();

//! \brief     Gets the pwm enable status
//! \param[in]  handle  The hardware abstraction layer (HAL) handle
//! \return    The pwm enable
static inline bool HAL_getPwmEnableStatus(HAL_PWMData_t *handle)
{
     HAL_PWMData_t *obj = (HAL_PWMData_t *)handle;
     return(obj->flagEnablePwm);
} // end of HAL_getPwmStatus() function



//! \brief      Initializes the interrupt vector table
//! \details    Points the ISR to the function mainISR.
//! \param[in]  handle  The hardware abstraction layer (HAL) handle
static inline void HAL_initIntVectorTable(const HAL_INT_Config_t *handle)
{
    // TODO: Replace with M33-compatible peripheral code
    // // Motor_1->ADCA_INT1
    // Interrupt_register(handle->intPIENum, &motor1CtrlISR);
    (void)handle;
    return;
} // end of HAL_initIntVectorTable() function


//! \brief      Initializes the interrupt vector table with custom ISR
//! \details    Points the ISR to a specified function, enabling multi-motor support
//! \param[in]  handle  The hardware abstraction layer (HAL) interrupt config handle
//! \param[in]  isrFunc Pointer to the interrupt service routine function
static inline void HAL_initIntVectorTableWithISR(const HAL_INT_Config_t *handle,
                                                  void (*isrFunc)(void))
{
    // TODO: Replace with M33-compatible peripheral code
    // Interrupt_register(handle->intPIENum, isrFunc);
    (void)handle;
    (void)isrFunc;
    return;
} // end of HAL_initIntVectorTableWithISR() function


//! \brief      Initializes the interrupt vector table
//! \details    Points the ISR to the function mainISR.
//! \param[in]  intNum : Interrupt Number, fnPtr : Pointer to Interrupt function Call
static inline void HAL_registerInterrupt(uint32_t intNumber, void (*fnPtr)(void))
{
    // TODO: Replace with M33-compatible peripheral code
    // Interrupt_register(intNumber, fnPtr);
    (void)intNumber;
    (void)fnPtr;
    return;
}

////! \brief     Acknowledges an interrupt from the ADC so that another ADC
////!            interrupt can happen again.
////! \param[in] handle     The hardware abstraction layer (HAL) handle

static inline void HAL_ackMtrADCInt(const HAL_INT_Config_t *handle)
{
     // clear the ADC interrupt flag
    DL_ADC_clearInterruptStatus((ADC_Regs *)handle->intPeripheralBase,(DL_ADC_INT_NUMBER)handle->intPeripheralNum);

} // end of HAL_ackADCInt() function


static inline
float32_t HAL_ControllerReadMtrCurrentAdcData(HAL_ADC_Result_Config_Handle cfg)
{
    return((float32_t)DL_ADC_readResult((ADC_RESULT_Regs *)cfg->adcResultBase,(DL_ADC_SOC_NUMBER) cfg->socNum));
}

static inline
float32_t HAL_ControllerReadMtrVoltageAdcData(HAL_ADC_Result_Config_Handle cfg)
{
     return((float32_t)DL_ADC_readResult((ADC_RESULT_Regs *)cfg->adcResultBase,(DL_ADC_SOC_NUMBER) cfg->socNum));
}

//! \brief     Configures the High Comparator to measure the -Ve current Limit
//! \param[in] cmpBase    Base Address of the Comparator
//! \param[in] compValue  Compare DAC Value
static inline void HAL_Set_PhHighCompVal(uint32_t cmpBase, uint16_t compValue)
{
    DL_CMPSSLITE_setDACValueHigh((CMPSS_LITE_Regs *)cmpBase, compValue);
}
//! \brief     Configures the Low Comparator to measure the -Ve current Limit
//! \param[in] cmpBase    Base Address of the Comparator
//! \param[in] compValue  Compare DAC Value
static inline void HAL_Set_PhLowCompVal(uint32_t cmpBase, uint16_t compValue)
{
    DL_CMPSSLITE_setDACValueLow((CMPSS_LITE_Regs *)cmpBase, compValue);
}

//! \brief     Toggles the GPIO pin
//! \param[in] handle      The hardware abstraction layer (HAL) handle
//! \param[in] gpioNumber  The GPIO number
static inline void HAL_toggleGPIO(uint32_t handle,const uint32_t gpioNumber)
{
     // set GPIO high
    DL_GPIO_togglePins((GPIO_Regs*)handle, gpioNumber);
} // end of HAL_toggleGPIO() function

//! \brief     Sets the GPIO pin high
//! \param[in] handle      The hardware abstraction layer (HAL) handle
//! \param[in] gpioNumber  The GPIO number
static inline void HAL_setGPIOHigh(uint32_t handle,const uint32_t gpioNumber)
{
     // set GPIO high
    DL_GPIO_setPins((GPIO_Regs*)handle, gpioNumber);

} // end of HAL_setGPIOHigh() function

//! \brief     Sets the GPIO pin low
//! \param[in] handle      The hardware abstraction layer (HAL) handle
//! \param[in] gpioNumber  The GPIO number
static inline void HAL_setGPIOLow(uint32_t handle,const uint32_t gpioNumber)
{
     // set GPIO low
    DL_GPIO_clearPins((GPIO_Regs*)handle, gpioNumber);

} // end of HAL_setGPIOLow() function

//! \brief      clear Comparator High Fault Status
//! \details
//! \param[in]  handle  The hardware abstraction layer (HAL) handle
static inline void HAL_clear_CMPHigh_FaultStatus(uint32_t base)
{
    DL_CMPSSLITE_clearFilterLatchHigh((CMPSS_LITE_Regs *)base);
} // end of HAL_clear_CMPHigh_FaultStatus() function

//! \brief      clear Comparator Low Fault Status
//! \details
//! \param[in]  handle  The hardware abstraction layer (HAL) handle
static inline void HAL_clear_CMPLow_FaultStatus(uint32_t base)
{
    DL_CMPSSLITE_clearFilterLatchLow((CMPSS_LITE_Regs *)base);

} // end of HAL_clear_CMPHigh_FaultStatus() function

static inline void HAL_StopIPDPulseCount(const HAL_ECAP_Config_t *ecapIPDHandle)
{

    DL_ECAP_stopCounter((ECAP_Regs *)ecapIPDHandle->base);

}
static inline void HAL_StartIPDPulseCount(const HAL_ECAP_Config_t *ecapIPDHandle)
{
    DL_ECAP_enableTimeStampCapture((ECAP_Regs *)ecapIPDHandle->base);
    DL_ECAP_startCounter((ECAP_Regs *)ecapIPDHandle->base);

}
static inline void HAL_ResetIPDPulseCount(const HAL_ECAP_Config_t *ecapIPDHandle)
{

    DL_ECAP_resetCounters((ECAP_Regs *)ecapIPDHandle->base);

}
static inline uint32_t HAL_getIPDPulseTime(const HAL_ECAP_Config_t *ecapIPDHandle)
{

    return(DL_ECAP_getEventTimeStamp((ECAP_Regs *)ecapIPDHandle->base,ecapIPDHandle->highEvent));

}
/******************************************************************************/

//! \brief     Sets up the PWMs (Pulse Width Modulators)
//! \param[in] handle          The hardware abstraction layer (HAL) handle
static inline uint32_t HAL_getMtrTripFaults(const HAL_PWM_Config_t *handle)
{
    uint32_t tripFault = 0;

    tripFault = (DL_MCPWM_getTripZoneFlagStatus((MCPWM_Regs *)handle->pwmHandle) &
            handle->overCurrentTZSouce);

    return(tripFault);
}

//! \brief     Sets up the PWMs (Pulse Width Modulators)
//! \param[in] handle          The hardware abstraction layer (HAL) handle
static inline uint32_t HAL_getGDTripFaults(const HAL_PWM_Config_t *handle)
{
    uint32_t tripFault = 0;

     tripFault = (DL_MCPWM_getTripZoneFlagStatus((MCPWM_Regs *)handle->pwmHandle) &
             handle->GateDriverFaultTZSource);

     return(tripFault);
}

//! \brief      clear PWM fault status of motor control
//! \details
//! \param[in]  handle  The hardware abstraction layer (HAL) handle
static inline void HAL_clearPWMFaultStatus(const HAL_PWM_Config_t *handle)
{
    // Clear the Trip Zone Event Caused by Gate Driver / Over Current
    DL_MCPWM_clearTripZoneFlagStatus((MCPWM_Regs *)handle->pwmHandle,
                 (handle->overCurrentTZSouce | handle->GateDriverFaultTZSource));

    // Clear the Master Trip Event from Interrupt
    DL_MCPWM_clearInterrupt((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_INT_TZ_OST);

} // end of HAL_clearMtrFaultStatus() function

// Two methods for single shunt
//! \brief     PWM phase shift and ADC SOC timing for dc_link current sensing
//! \details   PWM phase shift compensation based on each phase duty and min duration.
//!            dc_link current sampling point is calcuated with shifted PWM compare value.
//! \param[in] handle       The hardware abstraction layer (HAL) handle
//! \param[in] dclinkHandle The dclink handle
//! \param[in] pVab_out     The pointer to the Vab voltage
//! \param[in] pPWMData     The pointer to the PWM data
static inline void
HAL_runSingleShuntCompensation(const HAL_PWM_Config_t *handle,
                               DCLINK_SS_Handle dclinkHandle,
                               const MATH_vec2 *pVab_out,
                               HAL_PWMData_t *pPWMData, const float32_t Vdc_V)
{
     MATH_ui_vec2 upSoc, downSoc;
     MATH_ui_vec3 pwmCMPA, pwmCMPB;

     // get EPWMx CMPA/CMPB values
     pwmCMPA.value[0] = pwmCMPB.value[0] = pPWMData->cmpValue[0];
     pwmCMPA.value[1] = pwmCMPB.value[1] = pPWMData->cmpValue[1];
     pwmCMPA.value[2] = pwmCMPB.value[2] = pPWMData->cmpValue[2];
     // run PWM compensation for single-shunt

     DCLINK_SS_runPWMCompensation(dclinkHandle, pVab_out, Vdc_V,
                               &pwmCMPA, &pwmCMPB, &upSoc, &downSoc);

//     pPWMData->cmpAValue[0] = pwmCMPA.value[0];
//     pPWMData->cmpBValue[0] = pwmCMPB.value[0];
//     pPWMData->cmpAValue[1] = pwmCMPA.value[1];
//     pPWMData->cmpBValue[1] = pwmCMPB.value[1];
//     pPWMData->cmpAValue[2] = pwmCMPA.value[2];
//     pPWMData->cmpBValue[2] = pwmCMPB.value[2];
//     pPWMData->upSoc[0] = upSoc.value[0];
//     pPWMData->upSoc[1] = upSoc.value[1];
//     pPWMData->downSoc[0] = downSoc.value[0];
//     pPWMData->downSoc[1] = downSoc.value[1];

     DL_MCPWM_setCounterCompareShadowValue((MCPWM_Regs *)handle->pwmHandle,
                                           DL_MCPWM_COUNTER_COMPARE_1A,
                                           pwmCMPA.value[0]);
     DL_MCPWM_setCounterCompareShadowValue((MCPWM_Regs *)handle->pwmHandle,
                                           DL_MCPWM_COUNTER_COMPARE_1B,
                                               pwmCMPB.value[0]);
     DL_MCPWM_setCounterCompareShadowValue((MCPWM_Regs *)handle->pwmHandle,
                                           DL_MCPWM_COUNTER_COMPARE_2A,
                                               pwmCMPA.value[1]);
     DL_MCPWM_setCounterCompareShadowValue((MCPWM_Regs *)handle->pwmHandle,
                                           DL_MCPWM_COUNTER_COMPARE_2B,
                                               pwmCMPB.value[1]);
     DL_MCPWM_setCounterCompareShadowValue((MCPWM_Regs *)handle->pwmHandle,
                                           DL_MCPWM_COUNTER_COMPARE_3A,
                                               pwmCMPA.value[2]);
     DL_MCPWM_setCounterCompareShadowValue((MCPWM_Regs *)handle->pwmHandle,
                                           DL_MCPWM_COUNTER_COMPARE_3B,
                                               pwmCMPB.value[2]);

    *handle->pwmUpCountTriggerSourceA     = ((uint32_t) upSoc.value[0] & MCPWM_CMPCS_CMPCS_MASK);
    *handle->pwmUpCountTriggerSourceB     = ((uint32_t) upSoc.value[1] & MCPWM_CMPCS_CMPCS_MASK);
    *handle->pwmDownCountTriggerSourceA   = ((uint32_t) downSoc.value[0] & MCPWM_CMPCS_CMPCS_MASK);
    *handle->pwmDownCountTriggerSourceB   = ((uint32_t) downSoc.value[1] & MCPWM_CMPCS_CMPCS_MASK);

    return;
} // end of HAL_singleShuntCompensation() function


//! \brief     Writes PWM data to the PWM comparators for motor control
//! \param[in] handle    The hardware abstraction layer (HAL) handle
//! \param[in] pPWMData  The pointer to the PWM data
static inline void
HAL_writePWMData(const HAL_PWM_Config_t *handle, HAL_PWMData_t *pPWMData)
{
     uint16_t pwmCnt;
     for(pwmCnt=0U; pwmCnt<3U; pwmCnt++)
     {
       // compute the value
         float32_t V_pu = -pPWMData->Vabc_pu.value[pwmCnt];
         float32_t V_sat_pu = __fsat(V_pu, 0.5f, -0.5f);
         float32_t V_sat_dc_pu = V_sat_pu + 0.5f;
         pPWMData->cmpValue[pwmCnt]  = (int16_t)(V_sat_dc_pu * handle->pwmPeriod);

         if(pPWMData->cmpValue[pwmCnt] < pPWMData->minCMPValue)
         {
             pPWMData->cmpValue[pwmCnt] = pPWMData->minCMPValue;
         }
     }
     DL_MCPWM_setCounterCompareShadowValue((MCPWM_Regs *)handle->pwmHandle,
                                           DL_MCPWM_COUNTER_COMPARE_1A,
                                           pPWMData->cmpValue[0]);
     DL_MCPWM_setCounterCompareShadowValue((MCPWM_Regs *)handle->pwmHandle,
                                           DL_MCPWM_COUNTER_COMPARE_1B,
                                           pPWMData->cmpValue[0]);
     DL_MCPWM_setCounterCompareShadowValue((MCPWM_Regs *)handle->pwmHandle,
                                           DL_MCPWM_COUNTER_COMPARE_2A,
                                           pPWMData->cmpValue[1]);
     DL_MCPWM_setCounterCompareShadowValue((MCPWM_Regs *)handle->pwmHandle,
                                           DL_MCPWM_COUNTER_COMPARE_2B,
                                           pPWMData->cmpValue[1]);
     DL_MCPWM_setCounterCompareShadowValue((MCPWM_Regs *)handle->pwmHandle,
                                           DL_MCPWM_COUNTER_COMPARE_3A,
                                           pPWMData->cmpValue[2]);
     DL_MCPWM_setCounterCompareShadowValue((MCPWM_Regs *)handle->pwmHandle,
                                           DL_MCPWM_COUNTER_COMPARE_3B,
                                           pPWMData->cmpValue[2]);

} // end of HAL_writePWMData() function

//! \brief      Enforces the Tri state Switches for PWM Base
//! \details    Turns on the outputs of the EPWM peripheral which will allow
//!             the power switches to be controlled.
//! \param[in]  handle  The hardware abstraction layer (HAL) handle
static inline void HAL_forcePWMLow(const HAL_PWM_Config_t *handle)
{

    DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_AQ_OUTPUT_1A,
                                        DL_MCPWM_AQ_SW_CONTINUOUS_LOW);

    DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_AQ_OUTPUT_1B,
                                        DL_MCPWM_AQ_SW_CONTINUOUS_LOW);

    DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_AQ_OUTPUT_2A,
                                        DL_MCPWM_AQ_SW_CONTINUOUS_LOW);

    DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_AQ_OUTPUT_2B,
                                        DL_MCPWM_AQ_SW_CONTINUOUS_LOW);

    DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_AQ_OUTPUT_3A,
                                        DL_MCPWM_AQ_SW_CONTINUOUS_LOW);

    DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_AQ_OUTPUT_3B,
                                        DL_MCPWM_AQ_SW_CONTINUOUS_LOW);

    // setup the Dead-Band Generator Control Register (DBCTL)
    DL_MCPWM_setDeadBandDelayMode((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_DB_RED, false);
    DL_MCPWM_setDeadBandDelayMode((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_DB_FED, false);

    return;
} // end of HAL_forcePWMDisable() function

//! \brief      Enables the PWM for braking
//! \details    Turns on the Low Side outputs of the MCPWM peripheral
//! \param[in]  handle  The hardware abstraction layer (HAL) handle
static inline void HAL_enableBrakePWM(const HAL_PWM_Config_t *handle, HAL_PWMData_t *dataHandle)
{
    DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_AQ_OUTPUT_1A,
                                    DL_MCPWM_AQ_SW_CONTINUOUS_LOW);

    DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_AQ_OUTPUT_1B,
                                    DL_MCPWM_AQ_SW_CONTINUOUS_HIGH);

    DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_AQ_OUTPUT_2A,
                                    DL_MCPWM_AQ_SW_CONTINUOUS_LOW);

    DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_AQ_OUTPUT_2B,
                                    DL_MCPWM_AQ_SW_CONTINUOUS_HIGH);

    DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_AQ_OUTPUT_3A,
                                    DL_MCPWM_AQ_SW_CONTINUOUS_LOW);

    DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_AQ_OUTPUT_3B,
                                    DL_MCPWM_AQ_SW_CONTINUOUS_HIGH);

    // setup the Dead-Band Generator Control Register (DBCTL)
    DL_MCPWM_setDeadBandDelayMode((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_DB_RED, false);
    DL_MCPWM_setDeadBandDelayMode((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_DB_FED, false);

} // end of HAL_enableBrakePWM() function

//! \brief      Exits the PWM braking routine
//! \details    Turns on the outputs of the MCPWM peripheral which will allow
//!             the power switches to be controlled.
//! \param[in]  handle  The hardware abstraction layer (HAL) handle
static inline void HAL_ResetPWM(const HAL_PWM_Config_t *handle, HAL_PWMData_t *dataHandle)
{

    // setup the Dead-Band Generator Control Register (DBCTL)
    DL_MCPWM_setDeadBandDelayMode((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_DB_RED, true);
    DL_MCPWM_setDeadBandDelayMode((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_DB_FED, true);

    DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_AQ_OUTPUT_1A,
                                        DL_MCPWM_AQ_SW_FORCE_DISABLED);

    DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_AQ_OUTPUT_1B,
                                        DL_MCPWM_AQ_SW_FORCE_DISABLED);

    DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_AQ_OUTPUT_2A,
                                        DL_MCPWM_AQ_SW_FORCE_DISABLED);

    DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_AQ_OUTPUT_2B,
                                        DL_MCPWM_AQ_SW_FORCE_DISABLED);

    DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_AQ_OUTPUT_3A,
                                        DL_MCPWM_AQ_SW_FORCE_DISABLED);

    DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_AQ_OUTPUT_3B,
                                        DL_MCPWM_AQ_SW_FORCE_DISABLED);


     dataHandle->flagEnablePwm = false;

    return;
} // end of HAL_enableBrakePWM() function

//! \brief      Sets the appropriate MCPWM state for a given Voltage Vector
//! \details    Turns on the outputs of the MCPWM peripheral which will allow
//!             the power switches to be controlled.
//! \param[in]  handle  The hardware abstraction layer (HAL) handle
static inline void HAL_forceVoltageSector(VOLTAGE_VECTOR vector,
                                         const HAL_PWM_Config_t *handle)
{
    switch(vector)
    {
        case VOLTAGE_VECTOR_AC:

            /* Force Phase A High Side Switch */
           DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle,DL_MCPWM_AQ_OUTPUT_1A,DL_MCPWM_AQ_SW_CONTINUOUS_HIGH);
           /* Force Phase C Low Side Switch */
           DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle,DL_MCPWM_AQ_OUTPUT_3B,DL_MCPWM_AQ_SW_CONTINUOUS_HIGH);

            break;
        case VOLTAGE_VECTOR_CA:

            /* Force Phase C High Side Switch */
           DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle,DL_MCPWM_AQ_OUTPUT_3A,DL_MCPWM_AQ_SW_CONTINUOUS_HIGH);
           /* Force Phase A Low Side Switch */
           DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle,DL_MCPWM_AQ_OUTPUT_1B,DL_MCPWM_AQ_SW_CONTINUOUS_HIGH);

            break;
        case VOLTAGE_VECTOR_BC:

            /* Force Phase B High Side Switch */
           DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle,DL_MCPWM_AQ_OUTPUT_2A,DL_MCPWM_AQ_SW_CONTINUOUS_HIGH);
           /* Force Phase C Low Side Switch */
           DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle,DL_MCPWM_AQ_OUTPUT_3B,DL_MCPWM_AQ_SW_CONTINUOUS_HIGH);

            break;
        case VOLTAGE_VECTOR_CB:

            /* Force Phase C High Side Switch */
           DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle,DL_MCPWM_AQ_OUTPUT_3A,DL_MCPWM_AQ_SW_CONTINUOUS_HIGH);
           /* Force Phase B Low Side Switch */
           DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle,DL_MCPWM_AQ_OUTPUT_2B,DL_MCPWM_AQ_SW_CONTINUOUS_HIGH);

            break;
        case VOLTAGE_VECTOR_BA:

            /* Force Phase B High Side Switch */
           DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle,DL_MCPWM_AQ_OUTPUT_2A,DL_MCPWM_AQ_SW_CONTINUOUS_HIGH);
           /* Force Phase A Low Side Switch */
           DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle,DL_MCPWM_AQ_OUTPUT_1B,DL_MCPWM_AQ_SW_CONTINUOUS_HIGH);

            break;
        case VOLTAGE_VECTOR_AB:

            /* Force Phase A High Side Switch */
           DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle,DL_MCPWM_AQ_OUTPUT_1A,DL_MCPWM_AQ_SW_CONTINUOUS_HIGH);
           /* Force Phase B Low Side Switch */
           DL_MCPWM_setActionQualifierSWAction((MCPWM_Regs *)handle->pwmHandle,DL_MCPWM_AQ_OUTPUT_2B,DL_MCPWM_AQ_SW_CONTINUOUS_HIGH);

            break;
        default:
            /* This is expected to be empty */
            break;
    }
}

//! \brief      Enables the PWM devices for motor control
//! \details    Turns on the outputs of the EPWM peripheral which will allow
//!             the power switches to be controlled.
//! \param[in]  handle  The hardware abstraction layer (HAL) handle
static inline void HAL_enablePWM(const HAL_PWM_Config_t *handle, HAL_PWMData_t *dataHandle)
{
    /* Release the Forced PWM Low State */
    HAL_ResetPWM(handle,dataHandle);

    // Set the Input XBAR that trips the PWM's through TZ.
    DL_XBAR_setInputXBAR((DL_XBAR_InputNum)handle->MotorStoptripInputXBARSource, DL_XBAR_DRIVE_HIGH);

    // Clear the Trip Zone Event Caused by Motor Stop
    DL_MCPWM_clearTripZoneFlagStatus((MCPWM_Regs *)handle->pwmHandle, handle->MotorStopTZSource);

    // Clear the Master Trip Event from Interrupt
    DL_MCPWM_clearInterrupt((MCPWM_Regs *)handle->pwmHandle, DL_MCPWM_INT_TZ_OST);

    //TODO: Look to eliminate the dependency of HAL
    dataHandle->flagEnablePwm = true;

    return;
} // end of HAL_enablePWM() function

//! \brief      Disables the PWM device for motor control
//! \details    Turns off the outputs of the EPWM peripherals which will put
//!             the power switches into a high impedance state.
//! \param[in]  handle  The hardware abstraction layer (HAL) handle
static inline void HAL_disablePWM(const HAL_PWM_Config_t *handle, HAL_PWMData_t *dataHandle)
{
    // Set the Input XBAR that trips the PWM's through TZ.
    DL_XBAR_setInputXBAR((DL_XBAR_InputNum)handle->MotorStoptripInputXBARSource, DL_XBAR_DRIVE_LOW);

    /* Set all the PWM's Action to Forced Low State*/
    HAL_forcePWMLow(handle);

    dataHandle->flagEnablePwm = false;

} // end of HAL_disablePWM() function

//! \brief     Set trigger point near period for dc-link current offset
//! \param[in] handle       The hardware abstraction layer (HAL) handle
static inline void HAL_setOffsetTrigger(const HAL_PWM_Config_t *handle)
{
     uint16_t offsetUpSoc = handle->pwmPeriod * (3.0f/4.0f);
     uint16_t offsetDownSoc = handle->pwmPeriod * (1.0f/4.0f);
     // set the SOC trigger point for UP count

     *handle->pwmUpCountTriggerSourceA     = ((uint32_t)offsetUpSoc & MCPWM_CMPCS_CMPCS_MASK);
     *handle->pwmUpCountTriggerSourceB     = ((uint32_t)offsetUpSoc & MCPWM_CMPCS_CMPCS_MASK);
     *handle->pwmDownCountTriggerSourceA   = ((uint32_t)offsetDownSoc & MCPWM_CMPCS_CMPCS_MASK);
     *handle->pwmDownCountTriggerSourceB   = ((uint32_t)offsetDownSoc & MCPWM_CMPCS_CMPCS_MASK);

} // end of HAL_setOffsetTrigger() function

//! \brief     Set trigger point in the middle of the low side pulse
//! \param[in] handle    The hardware abstraction layer (HAL) handle
//! \param[in] deadband     The setting deadband for mosfet gate driver
//! \param[in] noisewindow  The noise window
//! \param[in] noisewindow  The adc Sampling window
void HAL_setTriggerParams(HAL_PWMData_t *pPWMData, const float32_t systemFreq_MHz,
                   const float32_t deadband_us, const float32_t noiseWindow_us,
                   const float32_t adcSample_us);

static inline void SCI_writeBytesNonBlocking(DL_UNICOMM_Inst_Regs *base, const uint8_t *data, uint16_t *idx, uint16_t len)
{
     while (*idx < len)
     {
         if (!DL_UART_isTXFIFOFull(base))
         {
             // Only lower 8 bits are sent
             DL_UART_transmitData(base, data[*idx]);
             (*idx)++;
         }
         else
         {
             break; // no room; finish next ISR tick
         }
     }
}

//! \brief     Reads the timer count
//! \param[in] handle       The hardware abstraction layer (HAL) handle
//! \param[in] timerNumber  The timer number, 0,1 or 2
//! \return    The timer count
static inline uint32_t
HAL_readTimerCnt(HAL_Handle handle,const uint16_t timerNumber)
{
    // TODO: Replace with M33-compatible peripheral code
    // HAL_Obj *obj = (HAL_Obj *)handle;
    // uint32_t timerCnt = CPUTimer_getTimerCount(obj->timerHandle[timerNumber]);
    // return(timerCnt);
    (void)handle;
    (void)timerNumber;
    return 0;
} // end of HAL_readTimerCnt() function
//! Sets the CPU timer period count.
//! \param base is the base address of the timer module.
//! \param timerCount is the CPU timer timer count.
static inline void HAL_setCPUTimerCount(HAL_Handle halHandle, uint32_t timerCount)
{
    // TODO: Replace with M33-compatible peripheral code
    // HAL_Obj   *obj = (HAL_Obj *)halHandle;
    //
    // // Load the MSB timer Count
    // //
    // HWREG(obj->timerHandle[1] + CPUTIMER_O_TIM) = timerCount;
    (void)halHandle;
    (void)timerCount;
    return;
}

//! \brief     Sets up the timers
//! \param[in] handle          The hardware abstraction layer (HAL) handle
//! \param[in] cpuTimerNumber  The CPU timer number
static inline void
HAL_clearCPUTimerFlag(HAL_Handle halHandle, const uint16_t cpuTimerNumber)
{
    // TODO: Replace with M33-compatible peripheral code
    // HAL_Obj   *obj = (HAL_Obj *)halHandle;
    // CPUTimer_clearOverflowFlag(obj->timerHandle[cpuTimerNumber]);
    (void)halHandle;
    (void)cpuTimerNumber;
    return;
}   // end of HAL_clearTimerFlag() function

//! \brief     Gets CPU Timer status
//! \param[in] handle          The hardware abstraction layer (HAL) handle
//! \param[in] cpuTimerNumber  The CPU timer number
static inline bool
HAL_getCPUTimerStatus(HAL_Handle halHandle, const uint16_t cpuTimerNumber)
{
    // TODO: Replace with M33-compatible peripheral code
    // HAL_Obj   *obj = (HAL_Obj *)halHandle;
    // return (CPUTimer_getTimerOverflowStatus(obj->timerHandle[cpuTimerNumber]));
    (void)halHandle;
    (void)cpuTimerNumber;
    return false;
}

//! \brief     Sets up the DMA for datalog
//! \param[in] handle          The hardware abstraction layer (HAL) handle
//! \param[in] dmaChNumber     The DMC Channel Number
//! \param[in] destAddr    The Datalog buffer dest address
//! \param[in] srcAddr     The Datalog buffer src address
void HAL_setupDMAforDLOG(HAL_Handle handle, const uint16_t dmaChNum,
                     const void *destAddr, const void *srcAddr);

//! \brief     Sets up the DMA
//! \param[in] N/A
void HAL_setupDMA(void);

//! \brief     Force trig the DMA channel for datalog
//! \param[in] handle          The hardware abstraction layer (HAL) handle
//! \param[in] dmaChNumber     The DMC Channel Number
static inline void
HAL_trigDMAforDLOG(HAL_Handle handle, const uint16_t DMAChNum)
{
    // TODO: Replace with M33-compatible peripheral code
    // HAL_Obj *obj = (HAL_Obj *)handle;
    // DMA_startChannel(obj->dmaChHandle[DMAChNum]);
    // DMA_forceTrigger(obj->dmaChHandle[DMAChNum]);
    (void)handle;
    (void)DMAChNum;
    return;
} // end of HAL_trigDlogWithDMA() function

//! \brief     Sets up the PIE (Peripheral Interrupt Expansion)
//! \param[in] handle  The hardware abstraction layer (HAL) handle
extern void HAL_setupPIE(HAL_Handle handle);

//! Configure the Three Shunt PWM Based Trigger Configurations
void HAL_SetupThreeShuntPWMConfig(const HAL_PWM_Config_t *handle);

//! Configure the Single Shunt PWM Based Trigger Configurations
void HAL_SetupSingleShuntPWMConfig(const HAL_PWM_Config_t *handle);

//! Setup the EPWM Run-Time Parameters
extern void HAL_setupPWMs(const HAL_PWM_Config_t *handle);

//! \brief
//! \param[in]
//! \param[in]
void HAL_clearDataRAM(void *pMemory, uint32_t lengthMemory);

//! \brief      Sets the hardware abstraction layer parameters
//! \details    Sets up the microcontroller peripherals.  Creates all of the scale
//!             factors for the ADC voltage and current conversions.  Sets the initial
//!             offset values for voltage and current measurements.
//! \param[in]  handle       The hardware abstraction layer (HAL) handle
extern void HAL_setParams(HAL_Handle handle);

//! \brief     Sets up the timers for CPU usage diagnostics
//! \param[in] handle          The hardware abstraction layer (HAL) handle
extern void HAL_setupCPUUsageTimer(HAL_Handle handle);

extern HAL_Handle HAL_init(void *pMemory,const size_t numBytes);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
