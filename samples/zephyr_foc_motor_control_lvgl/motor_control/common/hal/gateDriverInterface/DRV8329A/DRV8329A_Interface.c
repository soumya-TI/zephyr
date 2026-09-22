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
#ifdef DRV8329AEVM_REVA
#include "DRV8329A_Interface.h"
#include "halGateDriver.h"

void HAL_DRV8329_Init(void const *handle)
{

} // HAL_setupGate() function

void HAL_enableDRV8329(void const *handle)
{
    // Set EN_GATE to high for enabling the DRV
    HAL_DRV8329_Obj *obj = (HAL_DRV8329_Obj *)handle;

    HAL_setGPIOHigh(obj->gateNSleepGPIO, obj->gateNSleepGPIONum);

    HAL_setGPIOLow(obj->gateDriverOffGPIO, obj->gateDriverOffGPIONum);

    return;
} // HAL_setupGate() function

const HAL_GD_vTable_t halDRV8329FnTable =
{
     .initGateDriver  = HAL_DRV8329_Init,
     .enableGateDriver = HAL_enableDRV8329,
};
const HAL_GD_DATA_Obj drv8329Data_M1 = {
       .gdCurrOffset            = DRV8329_IDC_OFFSET_AD,
       .gdCurrSampDelay         = DRV8329_DCLINKSS_SAMPLE_DELAY,
       .gdCurrSensDelay         = DRV8329_DCLINKSS_MIN_DURATION,
       .gdCurrentSF             = DRV8329_CURRENT_SF,
       .gdMaxCurrentA           = DRV8329_ADC_FULL_SCALE_CURRENT_A,
       .gdVolSF                 = DRV8329_VOLTAGE_SF,
       .gdPhVoltOffsetSF        = DRV8329_VA_OFFSET_SF,
       .gdVolFilterPoleFreqRPS  = DRV8329_VOLTAGE_FILTER_POLE_rps,
       .gdOverVolVal            = DRV8329_OVER_VOLTAGE_FAULT_V,
       .gdUnderVolVal           = DRV8329_UNDER_VOLTAGE_FAULT_V,
       .gdCurrSign              = DRV8329_SIGN_CURRENT_SF
};
#endif
