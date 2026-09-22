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

#include "3phGANINV_3SH_motorConfig_MTR2.h"

// Explicitly include user.h to access USER_M2_PWM_TBPRD_NUM and USER_M2_NUM_PWM_TICKS_PER_ISR_TICK
// Don't rely on transitive includes through application.h
#include "user_mtr2.h"
#include "application.h"

#define MTR2_ADC_INT_BASE       ADCB_BASE               // ADCA-A14 -SOC4
#define MTR2_ADC_INT_NUM        ADC_INT_NUMBER1         // ADCA_INT1-SOC4

#define MTR2_PIE_INT_NUM        INT_ADCB1               // ADCA_INT1-SOC4
#define MTR2_INT_ACK_GROUP      INTERRUPT_ACK_GROUP1    // ADCA_INT1-CPU_INT1

/**
 * @var halGDInst
 * @brief The instance of the Gate Driver Interface object.
 */

const HAL_FOC_Config_t halFOCConfig_M2 = {

    .adcCurrentCfg[0].adcResultBase = MTR2_IU_ADC_SOC_NUM_RESULT_BASE,

    .adcCurrentCfg[1].adcResultBase = MTR2_IV_ADC_SOC_NUM_RESULT_BASE,

    .adcCurrentCfg[2].adcResultBase = MTR2_IW_ADC_SOC_NUM_RESULT_BASE,

    .adcCurrentCfg[0].adcBase = MTR2_IU_ADC_SOC_NUM_ADC_BASE,

    .adcCurrentCfg[1].adcBase = MTR2_IV_ADC_SOC_NUM_ADC_BASE,

    .adcCurrentCfg[2].adcBase = MTR2_IW_ADC_SOC_NUM_ADC_BASE,

    .adcCurrentCfg[0].ppbNum = MTR2_IU_ADC_PPB_NUM,

    .adcCurrentCfg[1].ppbNum = MTR2_IV_ADC_PPB_NUM,

    .adcCurrentCfg[2].ppbNum = MTR2_IW_ADC_PPB_NUM,

    .adcVoltageCfg[0].adcResultBase = MTR2_VU_ADC_SOC_NUM_RESULT_BASE,

    .adcVoltageCfg[1].adcResultBase = MTR2_VV_ADC_SOC_NUM_RESULT_BASE,

    .adcVoltageCfg[2].adcResultBase = MTR2_VW_ADC_SOC_NUM_RESULT_BASE,

    .adcVoltageCfg[3].adcResultBase = MTR2_VDC_ADC_SOC_NUM_RESULT_BASE,

    .adcVoltageCfg[0].socNum = MTR2_VU_ADC_SOC_NUM,

    .adcVoltageCfg[1].socNum = MTR2_VV_ADC_SOC_NUM,

    .adcVoltageCfg[2].socNum = MTR2_VW_ADC_SOC_NUM,

    .adcVoltageCfg[3].socNum = MTR2_VDC_ADC_SOC_NUM,
    //
    .pwmCfg.pwmHandle[0] = MTR2_EPWM_U_BASE,        //!< the PWM halFOCHandle
    .pwmCfg.pwmHandle[1] = MTR2_EPWM_V_BASE,        //!< the PWM halFOCHandle
    .pwmCfg.pwmHandle[2] = MTR2_EPWM_W_BASE,        //!< the PWM halFOCHandle

    .cmpCfg.cmpssHandle[0] = MTR2_IU_CMP_SEL_BASE,  //!< the CMPSS halFOCHandle
    .cmpCfg.cmpssHandle[1] = MTR2_IV_CMP_SEL_BASE,  //!< the CMPSS halFOCHandle
    .cmpCfg.cmpssHandle[2] = MTR2_IW_CMP_SEL_BASE,  //!< the CMPSS halFOCHandle

    .cmpCfg.compType[0] = COMP_HIGH,  //!< the CMPSS halFOCHandle
    .cmpCfg.compType[1] = COMP_HIGH,  //!< the CMPSS halFOCHandle
    .cmpCfg.compType[2] = COMP_HIGH,  //!< the CMPSS halFOCHandle

    .ecapIPDConfig.base = MTR2_IPD_CAPTURE_BASE, // Capture Handle for IPD Pulse Measurement
    .ecapIPDConfig.highEvent = ECAP_EVENT_1, // Capture Handle for IPD Pulse Measurement

    .intCfg.intPIENum = INT_ADCB1,
    .intCfg.intPeripheralBase =  DUAL_MTR_ADCB_BASE,
    .intCfg.intPeripheralNum = ADC_INT_NUMBER1,
    .intCfg.intAckGroup = INTERRUPT_ACK_GROUP1,

    .dclinkHandle   = NULL,

    .pwmCfg.pwmPeriod = USER_M2_PWM_TBPRD_NUM,
    .pwmCfg.numPWMTicksPerISRTick = USER_M2_NUM_PWM_TICKS_PER_ISR_TICK,
};

const HAL_3PHGANINV_Obj hal3PHGANINVInst_M2 =
{
     .gateEnableGPIO = MTR2_GATE_EN_GPIO,
};


/* Configure Which Specific Gate Driver To be used For Motor1 Configuration */
const HAL_GD_Interface_t halGDInst_M2 = {
    /**
     * @var halGDInst.vGDTable
     * @brief The pointer to the Gate Driver Configuration table.
     */
    .vGDTable = &hal3PHGANINVFnTable,

    /**
     * @var halGDInst.halGDHandle
     * @brief The pointer to the Gate Driver handle.
     */
    .halGDHandle =  &hal3PHGANINVInst_M2,

    /**
     * @var halGDInst.gdDataHandle
     * @brief The pointer to the Gate Driver data handle.
     */
    .gdDataHandle = &hal3PHGANINVData,
};

HAL_FOC_Data_Obj hal3SFOCDataObj_M2;

/**
 * @var halFOCInterface
 * @brief Link the instance of the FOC Interface object.
 */
const HAL_FOC_Interface_t halFOCInterface_M2 = {
    /**
     * @var halFOCInterface.vFOCTable
     * @brief The pointer to the FOC Configuration table.
     */
    .vFOCTable = &hal3SFOCFnTable,

    /**
     * @var halFOCInterface.halFOCDataHandle
     * @brief The pointer to the FOC Data handle.
     */
    .halFOCDataHandle = &hal3SFOCDataObj_M2,
    /**
     * @var halFOCInterface.halFOCConfigHandle
     * @brief The pointer to the FOC Configurations handle.
     */
    .halFOCConfigHandle = &halFOCConfig_M2,
};


/**
 * @var motorVars_M1
 * @brief The instance of the motor variables for motor 1.
 */
volatile MOTOR_Vars_t motorVars_M2 = {
    /**
     * @var motorVars_M1.gateDriveHandle
     * @brief The pointer to the Gate Driver handle.
     */
    .gateDriveHandle = &halGDInst_M2,

    /**
     * @var motorVars_M1.halInterface
     * @brief The pointer to the FOC interface.
     */
    .halInterface = &halFOCInterface_M2,
};

#pragma DATA_SECTION(motorVars_M2, "foc_data");





