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

#ifndef DRV8323RH_INTERFACE_H
#define DRV8323RH_INTERFACE_H

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

#ifdef BSXL8323RH_REVB

#include "stdint.h"

// *****************************************************************************
// the defines

//! \brief Defines the nominal DC bus voltage, V
//!
#define DRV8323_NOMINAL_DC_BUS_VOLTAGE_V         (48)

//! \brief Defines the maximum voltage at the AD converter
#define DRV8323_ADC_FULL_SCALE_VOLTAGE_V         (57.5285f)

//! \brief Defines the analog voltage filter pole location, Hz
#define DRV8323_VOLTAGE_FILTER_POLE_Hz           (680.4839f)

//! \brief Defines the maximum current at the AD converter
#define DRV8323_ADC_FULL_SCALE_CURRENT_A         (47.1429f)


//! \brief Defines the voltage scale factor for the system
//!
#define DRV8323_VOLTAGE_SF          (DRV8323_ADC_FULL_SCALE_VOLTAGE_V / 4096.0f)

//! \brief Defines the current scale factor for the system
//!
#define DRV8323_CURRENT_SF          (DRV8323_ADC_FULL_SCALE_CURRENT_A / 4096.0f)


//! \brief Defines the current scale invert factor for the system
//!
#define DRV8323_CURRENT_INV_SF      (4096.0f / DRV8323_ADC_FULL_SCALE_CURRENT_A)


//! \brief Defines the analog voltage filter pole location, rad/s
//!
#define DRV8323_VOLTAGE_FILTER_POLE_rps  (MATH_TWO_PI * DRV8323_VOLTAGE_FILTER_POLE_Hz)

#define DRV8323_SIGN_CURRENT_SF         (1.0f)

//! \brief ADC current offsets for A, B, and C phases
//#define DRV8323_IA_OFFSET_AD    (2045.4001f)
//#define DRV8323_IB_OFFSET_AD    (2012.8669f)
//#define DRV8323_IC_OFFSET_AD    (2031.5974f)

#define DRV8323_IDC_OFFSET_AD    (2048.0f)

//! \brief ADC current offset for CMPSS
#define DRV8323_IS_OFFSET_CMPSS   DRV8323_IDC_OFFSET_AD

//! \brief ADC voltage offsets for A, B, and C phases
#define DRV8323_VA_OFFSET_SF    (0.507f)
#define DRV8323_VB_OFFSET_SF    (0.5054f)
#define DRV8323_VC_OFFSET_SF    (0.5077f)

//! \brief DC bus over voltage threshold
#define DRV8323_OVER_VOLTAGE_FAULT_V        (54.5f)

//! \brief DC bus over voltage threshold
#define DRV8323_OVER_VOLTAGE_NORM_V         (52.5f)

//! \brief DC bus under voltage threshold
#define DRV8323_UNDER_VOLTAGE_FAULT_V       (8)

//! \brief DC bus under voltage threshold
#define DRV8323_UNDER_VOLTAGE_NORM_V        (10)

//------------------------------------------------------------------------------
//! \brief ADC current offsets checking value for A, B, and C phases
#define DRV8323_IA_OFFSET_AD_MAX    (2195.4001)
#define DRV8323_IB_OFFSET_AD_MAX    (2162.8669)
#define DRV8323_IC_OFFSET_AD_MAX    (2181.5974)

#define DRV8323_IA_OFFSET_AD_MIN    (1895.4001)
#define DRV8323_IB_OFFSET_AD_MIN    (1862.8669)
#define DRV8323_IC_OFFSET_AD_MIN    (1881.5974)

//! \brief ADC voltage offsets for A, B, and C phases
#define DRV8323_VA_OFFSET_SF_MAX    (0.557)
#define DRV8323_VB_OFFSET_SF_MAX    (0.5554)
#define DRV8323_VC_OFFSET_SF_MAX    (0.5577)

#define DRV8323_VA_OFFSET_SF_MIN    (0.457)
#define DRV8323_VB_OFFSET_SF_MIN    (0.4554)
#define DRV8323_VC_OFFSET_SF_MIN    (0.4577)

//! \brief Defines the number of current sensors
//!
#define DRV8323_NUM_CURRENT_SENSORS                 (3)

//! \brief Defines the number of Current Sense Readings
//!
#define DRV8323_NUM_CURRENT_SENSE_READINGS           (3)

//! \brief Defines the number of current sensor reading
//!
#define DRV8323_NUM_CURRENT_SENS_VAL                (3)

//! \brief Defines the number of voltage sensors Including DC Bus
//!
#define DRV8323_NUM_VOLTAGE_SENSORS                 (4)  // 1 , IF Phase Voltage Sensing is not critical


//! \brief      Defines the HAL Gate Driver object
//!
typedef struct HAL_DRV8323_Obj
{

  uint32_t       gateCalGPIO;
  uint32_t       gateCalGPIONum;

  uint32_t       gateEnableGPIO;
  uint32_t       gateEnableGPIONum;


} HAL_DRV8323_Obj;

//*****************************************************************************
//
//! \brief Defines the Gate Driver handle
//
//*****************************************************************************

#endif // DRV8323RH EVM
#endif // DRV8323RH_INTERFACE_H
