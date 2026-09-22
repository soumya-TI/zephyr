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

#ifndef DRV8329_INTERFACE_H
#define DRV8329_INTERFACE_H


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

#ifdef DRV8329AEVM_REVA
#include "stdint.h"
#include "hal.h"

//! \brief Defines the maximum voltage at the AD converter
#define DRV8329_ADC_FULL_SCALE_VOLTAGE_V         (76.04549098f)     // 110k/4.99k

//! \brief Defines the analog voltage filter pole location, Hz
#define DRV8329_VOLTAGE_FILTER_POLE_Hz           (326.6255675f)     // 110k/4.99k/100nF

//! \brief Defines the maximum current at the AD converter
//! Set the gain of the CSA, the value should be set accordingly.
//! CSAGAIN=5V/V(J2=GND), CSAGAIN=10V/V(J2=50K), CSAGAIN=20V/V(J2=200K), CSAGAIN=40V/V(J2=DNP)
//#define DRV8329_ADC_FULL_SCALE_CURRENT_A       (82.5f)            // Rs=0.001/gain=40/J2=DNP
//#define DRV8329_ADC_FULL_SCALE_CURRENT_A       (66.0f)            // Rs=0.0025/gain=20/J2=200K
#define DRV8329_ADC_FULL_SCALE_CURRENT_A       (33.0f)              // Rs=0.010/gain=10/J2=50K
//#define DRV8329_ADC_FULL_SCALE_CURRENT_A         (16.5f)          // Rs=0.050/gain=40/J2=DNP

//! \brief Defines the voltage scale factor for the system
//!
#define DRV8329_VOLTAGE_SF          (DRV8329_ADC_FULL_SCALE_VOLTAGE_V / 4096.0f)

//! \brief Defines the current scale factor for the system
//!
#define DRV8329_CURRENT_SF          (DRV8329_ADC_FULL_SCALE_CURRENT_A / 4096.0f)


//! \brief Defines the current scale invert factor for the system
//!
#define DRV8329_CURRENT_INV_SF      (4096.0f / DRV8329_ADC_FULL_SCALE_CURRENT_A)


//! \brief Defines the analog voltage filter pole location, rad/s
//!
#define DRV8329_VOLTAGE_FILTER_POLE_rps  (MATH_TWO_PI * DRV8329_VOLTAGE_FILTER_POLE_Hz)

//! \brief Defines the sign of the current_sf based on
//!        the polarity of the current feedback circuit
//!
//!        the "sign" = -1.0f if the current feedback polarity is positive that
//!        means the same pin of the shunt resistor is connected to ground and
//!        is also connected to the noninverting pin (+) of the operational amplifier
//!
//!        the "sign" = 1.0f if the current feedback polarity is negative that
//!        means the same pin of the shunt resistor is connected to ground and
//!        is also connected to the inverting pin (-) of the operational amplifier
#define DRV8329_SIGN_CURRENT_SF         (1.0f)

//! \brief ADC current offsets for dc-link
#define DRV8329_IDC_OFFSET_AD           (516.717f)       //~=4096.0/8.0

//! \brief ADC current offset for CMPSS
#define DRV8329_IDC_OFFSET_CMPSS        (uint16_t)(DRV8329_IDC_OFFSET_AD)

#define DRV8329_IDC_OFFSET_AD_MAX       (DRV8329_IDC_OFFSET_AD + 100.0f)
#define DRV8329_IDC_OFFSET_AD_MIN       (DRV8329_IDC_OFFSET_AD - 100.0f)

//! \brief ADC voltage offsets for A, B, and C phases
#define DRV8329_VA_OFFSET_SF    (0.507042527f)
#define DRV8329_VB_OFFSET_SF    (0.505379438f)
#define DRV8329_VC_OFFSET_SF    (0.50771445f)

//! \brief ADC voltage offsets for A, B, and C phases
#define DRV8329_VA_OFFSET_SF_MAX    (DRV8329_VA_OFFSET_SF + 0.05)
#define DRV8329_VB_OFFSET_SF_MAX    (DRV8329_VB_OFFSET_SF + 0.05)
#define DRV8329_VC_OFFSET_SF_MAX    (DRV8329_VC_OFFSET_SF + 0.05)

#define DRV8329_VA_OFFSET_SF_MIN    (DRV8329_VA_OFFSET_SF - 0.05)
#define DRV8329_VB_OFFSET_SF_MIN    (DRV8329_VB_OFFSET_SF - 0.05)
#define DRV8329_VC_OFFSET_SF_MIN    (DRV8329_VC_OFFSET_SF - 0.05)

//! \brief DC bus over voltage threshold
#define DRV8329_OVER_VOLTAGE_FAULT_V        (54.5f)

//! \brief DC bus over voltage threshold
#define DRV8329_OVER_VOLTAGE_NORM_V         (52.5f)

//! \brief DC bus under voltage threshold
#define DRV8329_UNDER_VOLTAGE_FAULT_V       (0.0f)

//! \brief DC bus under voltage threshold
#define DRV8329_UNDER_VOLTAGE_NORM_V        (0.0f)

// Tdt  =  55 ns (Dead-time between top and bottom switch)
// Tpd  = 140 ns (Gate driver propagation delay)
// Tr   = 136 ns (Rise time of amplifier including power switches turn on time)
// Ts   = 800 ns (Settling time of amplifier)
// Ts&h = 100 ns (ADC sample&holder = 1+(9)+2 = 12 SYSCLK)
// T_MinAVDuration = Tdt+Tr+Tpd+Ts+Ts&h
//                 = 55+140+136+800+100 = 1231(ns) => 148 SYSCLK cycles


//! \brief Defines the minimum duration, Clock Cycle - Dead Time + Propagation Delay + Amplifier Settling Time + ADC Sampling Time + Rise Time
#define DRV8329_DCLINKSS_MIN_DURATION   525 // (300U)

// T_SampleDelay   = Tdt+Tpd+Tr+Ts
//                 = 55+140+136+800     = 1131(ns) => 136 SYSCLK cycles

//! \brief Defines the sample delay, Clock Cycle : DRV8329_DCLINKSS_SAMPLE_DELAY - ADC Sampling Time.

#define DRV8329_DCLINKSS_SAMPLE_DELAY   490 //(280)      //


/* ADC Interrupt configurations for Single Shunt - To be Moved to the Common Interface*/

typedef struct _HAL_MTR_Obj_ *HAL_MTR_Handle;

//! \brief      Defines the HAL Gate Driver object
//!
typedef struct _HAL_DRV8329_Obj_
{
  uint32_t       gateNSleepGPIO;
  uint32_t       gateNSleepGPIONum;

  uint32_t       gateDriverOffGPIO;
  uint32_t       gateDriverOffGPIONum;

} HAL_DRV8329_Obj;


//*****************************************************************************
//
//! \brief Defines the Gate Driver handle
//
//*****************************************************************************

#endif // DRV8329_INTERFACE_H
#endif // DRV8329_INTERFACE_H
