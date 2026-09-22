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
#include "drv8329_1SH_motorConfig_MTR1.h"

// Explicitly include user.h to access USER_M1_PWM_TBPRD_NUM and USER_M1_NUM_PWM_TICKS_PER_ISR_TICK
// Don't rely on transitive includes through application.h
#include "user_mtr1.h"
#include "application.h"

#define MTR1_ADC_INT_BASE       ADCC_BASE               // ADCA-A14 -SOC4
#define MTR1_ADC_INT_NUM        ADC_INT_NUMBER1         // ADCA_INT1-SOC4

#define MTR1_PIE_INT_NUM        INT_ADCC1               // ADCA_INT1-SOC4
#define MTR1_INT_ACK_GROUP      INTERRUPT_ACK_GROUP1    // ADCA_INT1-CPU_INT1

const HAL_FOC_Config_t hal1SFOCConfig_M1 = {

    .adcCurrentCfg[0].adcResultBase = MTR1_IDC1_ADC_SOC_NUM_RESULT_BASE,

    .adcCurrentCfg[1].adcResultBase = MTR1_IDC2_ADC_SOC_NUM_RESULT_BASE,

    .adcCurrentCfg[2].adcResultBase = MTR1_IDC3_ADC_SOC_NUM_RESULT_BASE,

    .adcCurrentCfg[3].adcResultBase = MTR1_IDC4_ADC_SOC_NUM_RESULT_BASE,

    .adcCurrentCfg[0].adcBase = MTR1_IDC1_ADC_SOC_NUM_ADC_BASE,

    .adcCurrentCfg[1].adcBase = MTR1_IDC2_ADC_SOC_NUM_ADC_BASE,

    .adcCurrentCfg[2].adcBase = MTR1_IDC3_ADC_SOC_NUM_ADC_BASE,

    .adcCurrentCfg[3].adcBase = MTR1_IDC4_ADC_SOC_NUM_ADC_BASE,

    .adcCurrentCfg[0].ppbNum = MTR1_IDC1_ADC_PPB_NUM,

    .adcCurrentCfg[1].ppbNum = MTR1_IDC2_ADC_PPB_NUM,

    .adcCurrentCfg[2].ppbNum = MTR1_IDC3_ADC_PPB_NUM,

    .adcCurrentCfg[3].ppbNum = MTR1_IDC4_ADC_PPB_NUM,


//    .adcVoltageCfg[0].adcResultBase = MTR1_VU_ADC_SOC_NUM_RESULT_BASE,
//
//    .adcVoltageCfg[1].adcResultBase = MTR1_VV_ADC_SOC_NUM_RESULT_BASE,
//
//    .adcVoltageCfg[2].adcResultBase = MTR1_VW_ADC_SOC_NUM_RESULT_BASE,
//
    .adcVoltageCfg[3].adcResultBase = MTR1_VDC_ADC_SOC_NUM_RESULT_BASE,
//
//    .adcVoltageCfg[0].socNum = MTR1_VU_ADC_SOC_NUM,
//
//    .adcVoltageCfg[1].socNum = MTR1_VV_ADC_SOC_NUM,
//
//    .adcVoltageCfg[2].socNum = MTR1_VW_ADC_SOC_NUM,
//
    .adcVoltageCfg[3].socNum = MTR1_VDC_ADC_SOC_NUM,
    //
    .pwmCfg.pwmHandle[0] = MTR1_EPWM_U_BASE,        //!< the PWM halFOCHandle
    .pwmCfg.pwmHandle[1] = MTR1_EPWM_V_BASE,        //!< the PWM halFOCHandle
    .pwmCfg.pwmHandle[2] = MTR1_EPWM_W_BASE,        //!< the PWM halFOCHandle

    .cmpCfg.cmpssHandle[0] = MTR1_CMPSS_IDC_BASE_BASE,  //!< the CMPSS halFOCHandle
    .cmpCfg.compType[0] = COMP_HIGH,                // High Side Comparator is used.

    .ecapIPDConfig.base = MTR1_IPD_CAPTURE_BASE, // Capture Handle for IPD Pulse Measurement
    .ecapIPDConfig.highEvent = ECAP_EVENT_1, // Capture Handle for IPD Pulse Measurement

    .intCfg.intPIENum = MTR1_PIE_INT_NUM,
    .intCfg.intPeripheralBase =  MTR1_ADC_INT_BASE,
    .intCfg.intPeripheralNum = MTR1_ADC_INT_NUM,
    .intCfg.intAckGroup = MTR1_INT_ACK_GROUP,

    .dclinkHandle   = &dcLinkObj,

    .pwmCfg.pwmPeriod = USER_M1_PWM_TBPRD_NUM,
    .pwmCfg.numPWMTicksPerISRTick = USER_M1_NUM_PWM_TICKS_PER_ISR_TICK,
};

HAL_DRV8329_Obj drv8329Inst_M1 =
{
     .gateEnableGPIO = MTR1_GATE_EN_GPIO,

     .gateSleepGPIO = MTR1_GATE_nSLEEP_GPIO,
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
#pragma DATA_SECTION(motorVars_M1, "foc_data");
