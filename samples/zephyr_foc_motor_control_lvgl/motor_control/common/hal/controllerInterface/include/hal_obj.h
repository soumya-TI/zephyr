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

//!
//! \brief  Defines the structures for the HAL object
//!


#ifndef HAL_OBJ_H
#define HAL_OBJ_H

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

// drivers
// TODO: Replace with M33-compatible device header
// #include "device.h"

// modules
#include "hal_data.h"
#include "soc.h"

#define DAC_MAX_VALUE 4095

// TODO: Replace with M33-compatible ADC types when available
// Placeholder types for C2000 driverlib types
typedef uint16_t ADC_IntNumber;
typedef uint16_t ADC_PPBNumber;
typedef uint16_t ADC_SOCNumber;
typedef uint16_t ECAP_Events;

//------------------------------------------------------------------------------
//
//! \brief Enumeration for the kit boards
//
typedef enum
{
    BOARD_BSXL8323RS_REVA  = 0,         //!< the board is BOOSTXL_8323RS,  OK, in MCSDK
    BOARD_BSXL8323RH_REVB  = 1,         //!< the board is BOOSTXL_8323RH,  OK, in MCSDK
    BOARD_BSXL8353RS_REVA  = 2,         //!< the board is BOOSTXL_8353RS,  OK, in MCSDK
    BOARD_BSXL3PHGAN_REVA  = 3,         //!< the board is BOOSTXL_3PHGAN,  OK, in MCSDK
    BOARD_HVMTRPFC_REV1P1  = 4,         //!< the board is HVMTRPFC_REV1P1, OK, in MCSDK
    BOARD_BSXL8316RT_REVA  = 5,         //!< the board is BOOSTXL_8316RT,  OK, in MCSDK
    BOARD_DRV8329AEVM_REVA = 6,         //!< the board is BOOSTXL-DRV8329, OK

} Board_Kit_e;

typedef enum
{
    CURSEN_TYPE_SINGLE_SHUNT  = 0,   //!< the single shunt
    CURSEN_TYPE_THREE_SHUNT   = 1,   //!< the three shunt
    CURSEN_TYPE_INLINE_SHUNT  = 2,   //!< the inline shunt with ISO AMP
    CURSEN_TYPE_INLINE_HALL   = 3,   //!< the hall effect sensor
    CURSEN_TYPE_INLINE_SDFM   = 4    //!< the inline shunt with SDFM
} CURRENTSEN_Type_e;

typedef enum
{
   COMP_LOW   = 0,   //!< Low Comparator of Instance
   COMP_HIGH  = 1,   //!< High Comparator of Instance
   COMP_WINDOW  = 2   //!< Window Comparator Mode of Instance

} COMP_Type_e;

typedef struct _HAL_INT_Config_t_
{
    uint32_t intPeripheralBase;
    ADC_IntNumber intPeripheralNum;
    IRQn_Type intNVICNum;
}HAL_INT_Config_t;

typedef struct _HAL_PWM_Config_t_
{
     uint32_t pwmHandle;       //<! the PWM handles
     uint32_t pwmTrigHandle;   //<! the PWM Base for Triggering in Single Shunt handle

     uint16_t pwmPeriod;
     uint16_t numPWMTicksPerISRTick;
     uint16_t MotorStoptripInputXBARSource;
     uint32_t overCurrentTZSouce;
     uint32_t GateDriverFaultTZSource;
     uint32_t MotorStopTZSource;

     /* Triggers for Three Shunt*/
     uint16_t adcTrigPWMSource;

     /* Triggers for Single Shunt*/
     uint16_t adcTrigPWMUpCountASource;
     uint16_t adcTrigPWMUpCountBSource;
     uint16_t adcTrigPWMDownCountASource;
     uint16_t adcTrigPWMDownCountBSource;


     /* Triggers for Single Shunt support */
     volatile uint32_t * pwmDownCountTriggerSource;

     /* Triggers for Single Shunt support */
     volatile uint32_t * pwmUpCountTriggerSourceA;
     volatile uint32_t * pwmUpCountTriggerSourceB;
     volatile uint32_t * pwmDownCountTriggerSourceA;
     volatile uint32_t * pwmDownCountTriggerSourceB;

} HAL_PWM_Config_t;

typedef struct _HAL_1S_CMP_Config_t_
{
     uint32_t cmpssHandle[1];       //<! the PWM handles
     COMP_Type_e compType[1];       //<! the Comparator Type

} HAL_1S_CMP_Config_t;


typedef struct _HAL_ECAP_Config_t_
{
     uint32_t base;       //<! the Capture handle for IPD Pulse time measurement
     ECAP_Events highEvent; // Capture the High event of IPD Pulse

} HAL_ECAP_Config_t;

typedef struct _HAL_3S_CMP_Config_t_
{
     uint32_t cmpssHandle[3];       //<! the Compare handles
     COMP_Type_e compType[3];       //<! the Comparator Type

} HAL_3S_CMP_Config_t;


typedef struct _HAL_ADC_Result_Config_t_
{
    volatile uint32_t adcResultBase;
    ADC_SOCNumber socNum;

} HAL_ADC_Result_Config_t;
typedef struct _HAL_ADC_Result_Config_t_ *HAL_ADC_Result_Config_Handle;

// Defines for I2C
#define I2C_SLAVE_ADDRESS           0x50


//! \brief      Defines the hardware abstraction layer (HAL) data
//! \details    The HAL object contains all handles to peripherals.  When accessing a
//!             peripheral on a processor, use a HAL function along with the HAL handle
//!             for that processor to access its peripherals.
//!
typedef struct _HAL_Obj_
{

  uint32_t       timerHandle[1];    //<! the timer handles

  //  uint32_t       sciHandle;         //!< the SCI handle
//  uint32_t       i2cHandle;         //!< the I2C handle
//
//  uint32_t       spiHandle[2];      //!< the SPI handle
//
//  uint32_t       dmaHandle;         //!< the DMA handle
//  uint32_t       dmaChHandle[4];    //!< the DMA Channel handle


} HAL_Obj;

//! \brief      Defines the HAL handle
//! \details    The HAL handle is a pointer to a HAL object.  In all HAL functions
//!             the HAL handle is passed so that the function knows what peripherals
//!             are to be accessed.
//!
typedef struct _HAL_Obj_ *HAL_Handle;

#ifdef __cplusplus
}
#endif // extern "C"

#endif
