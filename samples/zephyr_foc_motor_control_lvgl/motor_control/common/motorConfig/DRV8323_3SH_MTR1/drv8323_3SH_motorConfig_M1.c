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
// Explicitly include user.h to access USER_M1_PWM_TBPRD_NUM and USER_M1_NUM_PWM_TICKS_PER_ISR_TICK
// Don't rely on transitive includes through application.h
#include "user.h"
#include "application.h"
#include "drv8323_3SH_motorConfig_M1.h"

/* Configure the Peripherals used for Motor 1 */

/* Interrupt Defines */
#define MTR1_FOC_INT1_INT                               ADC0_INT1_INT_IRQn

// Define the name of the ISR routine and Map with FOC ISR ADC Interrupt Handler
#define motor1CtrlISR                                   ADC0_INT1_IRQHandler

const HAL_FOC_Config_t halFOCConfig_M1 = {

    .adcCurrentCfg[0].adcResultBase = (uint32_t) MTR1_IU_SEN_ADC_RESULT_INST,

    .adcCurrentCfg[1].adcResultBase = (uint32_t) MTR1_IV_SEN_ADC_RESULT_INST,

    .adcCurrentCfg[2].adcResultBase = (uint32_t) MTR1_IW_SEN_ADC_RESULT_INST,

    .adcCurrentCfg[0].socNum = (uint16_t) MTR1_IU_SEN,

    .adcCurrentCfg[1].socNum = (uint16_t) MTR1_IV_SEN,

    .adcCurrentCfg[2].socNum = (uint16_t) MTR1_IW_SEN,

    .adcVoltageCfg[0].adcResultBase = (uint32_t) MTR1_VU_SEN_ADC_RESULT_INST,

    .adcVoltageCfg[1].adcResultBase = (uint32_t) MTR1_VV_SEN_ADC_RESULT_INST,

    .adcVoltageCfg[2].adcResultBase = (uint32_t) MTR1_VW_SEN_ADC_RESULT_INST,

    .adcVoltageCfg[3].adcResultBase = (uint32_t) MTR1_VDC_SEN_ADC_RESULT_INST,

    .adcVoltageCfg[0].socNum = (uint16_t) MTR1_VU_SEN,

    .adcVoltageCfg[1].socNum = (uint16_t) MTR1_VV_SEN,

    .adcVoltageCfg[2].socNum = (uint16_t) MTR1_VW_SEN,

    .adcVoltageCfg[3].socNum = (uint16_t) MTR1_VDC_SEN,


    .pwmCfg.pwmHandle = (uint32_t)MTR1_MCPWM_BASE_INST,        //!< the PWM halFOCHandle

    .pwmCfg.MotorStoptripInputXBARSource = (uint16_t)DL_XBAR_INPUT1,

    .pwmCfg.overCurrentTZSouce = (uint32_t)DL_MCPWM_TZ_SIGNAL_OSHT3,

    .pwmCfg.GateDriverFaultTZSource = (uint32_t)DL_MCPWM_TZ_SIGNAL_OSHT2,

    .pwmCfg.MotorStopTZSource = (uint32_t)DL_MCPWM_TZ_SIGNAL_OSHT1,

    .pwmCfg.pwmDownCountTriggerSource =  &((MCPWM_Regs *)MTR1_MCPWM_BASE_INST)->CMPCS,

    .pwmCfg.adcTrigPWMSource = (DL_MCPWM_SOC_TYPE)DL_MCPWM_SOC_A,

    //Config for Single Shunt
//    .pwmCfg.pwmUpCountTriggerSourceA  = &((MCPWM_Regs *)MTR1_MCPWM_BASE_INST)->CMPCS,

    .cmpCfg.cmpssHandle[0] = (uint32_t) MTR1_CMPSS_U_BASE_INST,  //!< the CMPSS halFOCHandle
    .cmpCfg.cmpssHandle[1] = (uint32_t) MTR1_CMPSS_V_BASE_INST,  //!< the CMPSS halFOCHandle
    .cmpCfg.cmpssHandle[2] = (uint32_t) MTR1_CMPSS_W_BASE_INST,  //!< the CMPSS halFOCHandle

    .cmpCfg.compType[0] = COMP_HIGH,  //!< the CMPSS halFOCHandle
    .cmpCfg.compType[1] = COMP_HIGH,  //!< the CMPSS halFOCHandle
    .cmpCfg.compType[2] = COMP_HIGH,  //!< the CMPSS halFOCHandle

//
    .ecapIPDConfig.base = (uint32_t )IPD_CAPTURE_INST, // Capture Handle for IPD Pulse Measurement
    .ecapIPDConfig.highEvent = DL_ECAP_EVENT_1, // Capture Handle for IPD Pulse Measurement
//
    .intCfg.intNVICNum = MTR1_FOC_INT1_INT,
    .intCfg.intPeripheralBase = (uint32_t) MTR1_IW_SEN_ADC_INST,
    .intCfg.intPeripheralNum = (uint16_t) DL_ADC_INT_NUMBER1, // SOCNAME_INT


    .dclinkHandle   = NULL,

    .pwmCfg.pwmPeriod = USER_M1_PWM_TBPRD_NUM,
    .pwmCfg.numPWMTicksPerISRTick = USER_M1_NUM_PWM_TICKS_PER_ISR_TICK,

};
const HAL_DRV8323_Obj drv8323Inst_M1 =
{
     .gateEnableGPIO = (uint32_t)MTR1_GATE_DRIVER_GATE_EN_GPIO_PORT,
     .gateEnableGPIONum = (uint32_t)MTR1_GATE_DRIVER_GATE_EN_GPIO_PIN,

     .gateCalGPIO = (uint32_t) MTR1_GATE_DRIVER_GATE_CAL_GPIO_PORT,
     .gateCalGPIONum = (uint32_t) MTR1_GATE_DRIVER_GATE_CAL_GPIO_PIN,
};
/**
 * @var halGDInst
 * @brief The instance of the Gate Driver Interface object.
 */
const HAL_GD_Interface_t halGDInst = {
    /**
     * @var halGDInst.vGDTable
     * @brief The pointer to the Gate Driver Configuration table.
     */
    .vGDTable = &halDRV8323FnTable,

    /**
     * @var halGDInst.halGDHandle
     * @brief The pointer to the Gate Driver handle.
     */
    .halGDHandle =  &drv8323Inst_M1,

    /**
     * @var halGDInst.gdDataHandle
     * @brief The pointer to the Gate Driver data handle.
     */
    .gdDataHandle = &drv8323Data_M1,
};

HAL_FOC_Data_Obj halFOCDataObj_M1;


/**
 * @var halFOCInterface
 * @brief The instance of the FOC Interface object.
 */
const HAL_FOC_Interface_t halFOCInterface = {
    /**
     * @var halFOCInterface.vFOCTable
     * @brief The pointer to the FOC Configuration table.
     */
    .vFOCTable = &hal3SFOCFnTable,

    /**
     * @var halFOCInterface.halFOCDataHandle
     * @brief The pointer to the FOC Data handle.
     */
    .halFOCDataHandle = &halFOCDataObj_M1,
    /**
     * @var halFOCInterface.halFOCConfigHandle
     * @brief The pointer to the FOC Configurations handle.
     */
    .halFOCConfigHandle = &halFOCConfig_M1,
};


/**
 * @var motorVars_M1
 * @brief The instance of the motor variables for motor 1.
 */
MOTOR_Vars_t motorVars_M1 = {
    /**
     * @var motorVars_M1.gateDriveHandle
     * @brief The pointer to the Gate Driver handle.
     */
    .gateDriveHandle = &halGDInst,

    /**
     * @var motorVars_M1.halInterface
     * @brief The pointer to the FOC interface.
     */
    .halInterface = &halFOCInterface,
};
