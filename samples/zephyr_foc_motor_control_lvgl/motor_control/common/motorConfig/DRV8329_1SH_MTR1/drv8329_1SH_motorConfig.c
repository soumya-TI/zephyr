/**
 * \file drv8329_1SH_motorConfig.c
 *
 * \brief This file contains the implementation of the motor configuration
 *        for the DRV8329A gate driver.
 *
 * \author a0132555
 * \date   2025
 */

// the globals
#include "drv8329_1SH_motorConfig.h"

// Explicitly include user.h to access USER_M1_PWM_TBPRD_NUM and USER_M1_NUM_PWM_TICKS_PER_ISR_TICK
// Don't rely on transitive includes through application.h
#include "user.h"
#include "application.h"

/* Interrupt Defines */
#define MTR1_FOC_INT1_INT                               ADC0_INT1_INT_IRQn

// Define the name of the ISR routine and Map with FOC ISR ADC Interrupt Handler
#define motor1CtrlISR                                   ADC2_INT1_IRQHandler

const HAL_FOC_Config_t hal1SFOCConfig_M1 = {

    .adcCurrentCfg[0].adcResultBase = (uint32_t) MTR1_IDC1_SEN_ADC_RESULT_INST,

    .adcCurrentCfg[1].adcResultBase = (uint32_t) MTR1_IDC2_SEN_ADC_RESULT_INST,

    .adcCurrentCfg[2].adcResultBase = (uint32_t) MTR1_IDC3_SEN_ADC_RESULT_INST,

    .adcCurrentCfg[3].adcResultBase = (uint32_t) MTR1_IDC4_SEN_ADC_RESULT_INST,

    .adcCurrentCfg[0].socNum = (uint16_t) MTR1_IDC1_SEN,

    .adcCurrentCfg[1].socNum = (uint16_t) MTR1_IDC2_SEN,

    .adcCurrentCfg[2].socNum = (uint16_t) MTR1_IDC3_SEN,

    .adcCurrentCfg[3].socNum = (uint16_t) MTR1_IDC4_SEN,

    .adcVoltageCfg[0].adcResultBase = (uint32_t) MTR1_VU_SEN_ADC_RESULT_INST,

    .adcVoltageCfg[1].adcResultBase = (uint32_t) MTR1_VV_SEN_ADC_RESULT_INST,

    .adcVoltageCfg[2].adcResultBase = (uint32_t) MTR1_VW_SEN_ADC_RESULT_INST,

    .adcVoltageCfg[3].adcResultBase = (uint32_t) MTR1_VDC_SEN_ADC_RESULT_INST,

    .adcVoltageCfg[0].socNum = (uint16_t) MTR1_VU_SEN,

    .adcVoltageCfg[1].socNum = (uint16_t) MTR1_VV_SEN,

    .adcVoltageCfg[2].socNum = (uint16_t) MTR1_VW_SEN,

    .adcVoltageCfg[3].socNum = (uint16_t) MTR1_VDC_SEN,

    .pwmCfg.pwmHandle = (uint32_t)MTR1_MCPWM_BASE_INST,        //!< the PWM halFOCHandle

    .pwmCfg.pwmTrigHandle = (uint32_t)MTR1_SINGLE_SH_TRIG_INST,

    .pwmCfg.MotorStoptripInputXBARSource = (uint16_t)DL_XBAR_INPUT1,

    .pwmCfg.overCurrentTZSouce = (uint32_t)DL_MCPWM_TZ_SIGNAL_OSHT3,

    .pwmCfg.GateDriverFaultTZSource = (uint32_t)DL_MCPWM_TZ_SIGNAL_OSHT2,

    .pwmCfg.MotorStopTZSource = (uint32_t)DL_MCPWM_TZ_SIGNAL_OSHT1,

    .pwmCfg.adcTrigPWMSource = (DL_MCPWM_SOC_TYPE)DL_MCPWM_SOC_D,

    //Config for Single Shunt
    .pwmCfg.pwmUpCountTriggerSourceA  = &((MCPWM_Regs *)MTR1_SINGLE_SH_TRIG_INST)->PWM1_CMPAS,
    .pwmCfg.pwmUpCountTriggerSourceB  = &((MCPWM_Regs *)MTR1_SINGLE_SH_TRIG_INST)->PWM2_CMPAS,
    .pwmCfg.pwmDownCountTriggerSourceA  = &((MCPWM_Regs *)MTR1_SINGLE_SH_TRIG_INST)->CMPCS,
    .pwmCfg.pwmDownCountTriggerSourceB  = &((MCPWM_Regs *)MTR1_SINGLE_SH_TRIG_INST)->CMPDS,

    .cmpCfg.cmpssHandle[0] = (uint32_t) MTR1_CMPSS_IDC_INST,  //!< the CMPSS halFOCHandle
    .cmpCfg.compType[0] = COMP_HIGH,                // High Side Comparator is used.

    .ecapIPDConfig.base = (uint32_t )IPD_CAPTURE_INST, // Capture Handle for IPD Pulse Measurement
    .ecapIPDConfig.highEvent = DL_ECAP_EVENT_1, // Capture Handle for IPD Pulse Measurement
//
    .intCfg.intNVICNum = MTR1_FOC_INT1_INT,
    .intCfg.intPeripheralBase = (uint32_t) MTR1_VU_SEN_ADC_INST,
    .intCfg.intPeripheralNum = (uint16_t) DL_ADC_INT_NUMBER1, // SOCNAME_INT

    .dclinkHandle   = &dcLinkObj,

    .pwmCfg.pwmPeriod = USER_M1_PWM_TBPRD_NUM,
    .pwmCfg.numPWMTicksPerISRTick = USER_M1_NUM_PWM_TICKS_PER_ISR_TICK,
};

HAL_DRV8329_Obj drv8329Inst_M1 =
{
     .gateNSleepGPIO = (uint32_t)MTR1_GATE_DRIVER_PORT,
      .gateNSleepGPIONum = (uint32_t)MTR1_GATE_DRIVER_GATE_N_SLEEP_PIN,

      .gateDriverOffGPIO = (uint32_t) MTR1_GATE_DRIVER_PORT,
      .gateDriverOffGPIONum = (uint32_t) MTR1_GATE_DRIVER_DRV_OFF_PIN,
};

/**
 * \brief The Gate Driver Interface object instance.
 */
const HAL_GD_Interface_t halGDInst_M1 = {
    /**
     * \brief The pointer to the Gate Driver Configuration table.
     */
    .vGDTable = &halDRV8329FnTable,

    /**
     * \brief The pointer to the Gate Driver handle.
     */
    .halGDHandle =  &drv8329Inst_M1,

    /**
     * \brief The pointer to the Gate Driver data handle.
     */
    .gdDataHandle = &drv8329Data_M1,
};

HAL_FOC_Data_Obj hal1SFOCDataObj_M1;

/**
 * \brief The FOC Interface object instance.
 */
const HAL_FOC_Interface_t halFOCInterface_M1 = {
    /**
     * \brief The pointer to the FOC Configuration table.
     */
    .vFOCTable = &hal1SFOCFnTable,

    /**
     * \brief The pointer to the FOC handle.
     */
    .halFOCDataHandle = &hal1SFOCDataObj_M1,

    .halFOCConfigHandle = &hal1SFOCConfig_M1,

};

/**
 * \brief The motor variables for motor 1.
 */
MOTOR_Vars_t motorVars_M1 = {
    /**
     * \brief The pointer to the Gate Driver handle.
     */
    .gateDriveHandle = &halGDInst_M1,

    /**
     * \brief The pointer to the FOC interface.
     */
    .halInterface = &halFOCInterface_M1,
};
