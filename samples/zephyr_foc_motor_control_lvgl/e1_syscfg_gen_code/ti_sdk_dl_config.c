
/* This file is AUTO GENERATED, DO NOT EDIT manually */


/*
 * Copyright (c) 2024, Texas Instruments Incorporated
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
 */

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform any initialization needed before using any board APIs
 */

#include "ti_sdk_dl_config.h"

void SYSCFG_DL_init(void)
{
    // SYSCFG_DL_SYSCTL_CLK_init();
    SYSCFG_DL_initPower();
    SYSCFG_DL_Pinmux_init();
    SYSCFG_DL_SYSCTL_init();
	SYSCFG_DL_INPUTXBAR_init();
	SYSCFG_DL_OUTPUTXBAR_init();
	SYSCFG_DL_PWMXBAR_init();
    SYSCFG_DL_ADC_0_init();
    SYSCFG_DL_ADC_1_init();
    SYSCFG_DL_ADC_2_init();
    SYSCFG_DL_MTR1_CMPSS_U_BASE_init();
    SYSCFG_DL_MTR1_CMPSS_V_BASE_init();
    SYSCFG_DL_MTR1_CMPSS_W_BASE_init();
    SYSCFG_DL_FOC_DAC_OUT_init();
    SYSCFG_DL_IPD_CAPTURE_init();
    SYSCFG_DL_MTR1_MCPWM_BASE_init();
    SYSCFG_DL_SERIAL_DATA_PLOT_init();
}

void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIO0);
    DL_GPIO_reset(GPIO1);
    DL_GPIO_reset(GPIO2);
    DL_GPIO_reset(GPIO3);

	DL_SYSCTL_resetPeripheral(DL_SYSCTL_RESET_XBAR);


    DL_ADC_reset(ADC_0_INST);
    DL_ADC_reset(ADC_1_INST);
    DL_ADC_reset(ADC_2_INST);
    DL_SYSCTL_resetPeripheral(DL_SYSCTL_RESET_CMPSS2);
    DL_SYSCTL_resetPeripheral(DL_SYSCTL_RESET_CMPSS1);
    DL_SYSCTL_resetPeripheral(DL_SYSCTL_RESET_CMPSS0);
    DL_SYSCTL_resetPeripheral(DL_SYSCTL_RESET_CMPSS3);
    DL_SYSCTL_resetPeripheral(DL_SYSCTL_RESET_ECAP0);
    DL_MCPWM_disableTBCLK();
    DL_SYSCTL_resetPeripheral(DL_SYSCTL_RESET_PWM4);
    //DL_UART_reset(SERIAL_DATA_PLOT_INST);

    DL_GPIO_enablePower(GPIO0);
    DL_GPIO_enablePower(GPIO1);
    DL_GPIO_enablePower(GPIO2);
    DL_GPIO_enablePower(GPIO3);

	DL_SYSCTL_enablePower(DL_SYSCTL_PWREN_XBAR);


    DL_ADC_enablePower(ADC_0_INST);
    DL_ADC_enablePower(ADC_1_INST);
    DL_ADC_enablePower(ADC_2_INST);
    DL_SYSCTL_enablePower(DL_SYSCTL_PWREN_CMPSS2);
    DL_SYSCTL_enablePower(DL_SYSCTL_PWREN_CMPSS1);
    DL_SYSCTL_enablePower(DL_SYSCTL_PWREN_CMPSS0);
    DL_SYSCTL_enablePower(DL_SYSCTL_PWREN_CMPSS3);
    DL_SYSCTL_enablePower(DL_SYSCTL_PWREN_ECAP0);
    DL_SYSCTL_enablePower(DL_SYSCTL_PWREN_PWM4);
    DL_UART_enablePower(SERIAL_DATA_PLOT_INST);
}

void SYSCFG_DL_Pinmux_init(void)
{
    DL_GPIO_initDigitalOutputFeatures(MTR1_GATE_DRIVER_GATE_EN_GPIO_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_DRIVE_STRENGTH_LOW, DL_GPIO_HIZ_DISABLE);

    DL_GPIO_initDigitalInput(MTR1_GATE_DRIVER_nFAULT_GPIO_IOMUX);

    DL_GPIO_initDigitalOutputFeatures(MTR1_GATE_DRIVER_GATE_MODE_GPIO_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_DOWN,
		 DL_GPIO_DRIVE_STRENGTH_LOW, DL_GPIO_HIZ_DISABLE);

    DL_GPIO_initDigitalInput(MTR1_GATE_DRIVER_GATE_GAIN_GPIO_IOMUX);

    DL_GPIO_initDigitalOutput(MTR1_GATE_DRIVER_GATE_CAL_GPIO_IOMUX);

    DL_GPIO_initDigitalOutput(MTR1_FOC_CTRL_FAULT_OUTPUT_GPIO_IOMUX);

    DL_GPIO_initDigitalOutput(MTR1_FOC_CTRL_ISR_TIMING_IOMUX);

    DL_GPIO_clearPins(GPIO0, MTR1_GATE_DRIVER_GATE_MODE_GPIO_PIN |
		MTR1_GATE_DRIVER_GATE_CAL_GPIO_PIN |
		MTR1_FOC_CTRL_FAULT_OUTPUT_GPIO_PIN);
    DL_GPIO_enableOutput(GPIO0, MTR1_GATE_DRIVER_GATE_MODE_GPIO_PIN |
		MTR1_GATE_DRIVER_GATE_CAL_GPIO_PIN |
		MTR1_FOC_CTRL_FAULT_OUTPUT_GPIO_PIN);
    DL_GPIO_clearPins(GPIO1, MTR1_FOC_CTRL_ISR_TIMING_PIN);
    DL_GPIO_setPins(GPIO1, MTR1_GATE_DRIVER_GATE_EN_GPIO_PIN);
    DL_GPIO_enableOutput(GPIO1, MTR1_GATE_DRIVER_GATE_EN_GPIO_PIN |
		MTR1_FOC_CTRL_ISR_TIMING_PIN);

    DL_GPIO_initPeripheralOutputFunction(IOMUX_PINCM_PA11, IOMUX_PA11_OUTPUTXBAR5);

    DL_GPIO_initPeripheralAnalogFunction(IOMUX_PINCM_PA17);
    DL_GPIO_initPeripheralAnalogFunction(IOMUX_PINCM_PA16);
    DL_GPIO_initPeripheralAnalogFunction(IOMUX_PINCM_PA27);
    DL_GPIO_initPeripheralAnalogFunction(IOMUX_PINCM_PA18);
    DL_GPIO_initPeripheralAnalogFunction(IOMUX_PINCM_PA30);
    DL_GPIO_initPeripheralAnalogFunction(IOMUX_PINCM_PA28);
    DL_GPIO_initPeripheralAnalogFunction(IOMUX_PINCM_PA26);

    DL_GPIO_initPeripheralAnalogFunction(IOMUX_PINCM_PA30);
    DL_GPIO_initPeripheralAnalogFunction(IOMUX_PINCM_PA27);
    DL_GPIO_initPeripheralAnalogFunction(IOMUX_PINCM_PA17);
    DL_GPIO_initPeripheralAnalogFunction(IOMUX_PINCM_PA6);


    // IOMUX Setting for MCPWM pins are done after the module initialization.
    // (part of SYSCFG_DL_<MCPWM_Instance>_init)

    // DL_GPIO_initPeripheralOutputFunction(
    //     GPIO_SERIAL_DATA_PLOT_IOMUX_TX, GPIO_SERIAL_DATA_PLOT_IOMUX_TX_FUNC);
    // DL_GPIO_initPeripheralInputFunction(
    //     GPIO_SERIAL_DATA_PLOT_IOMUX_RX, GPIO_SERIAL_DATA_PLOT_IOMUX_RX_FUNC);

}

static const DL_SYSCTL_SYSPLLConfig gSYSPLLConfig = {
    .sysPLLRef   = DL_SYSCTL_SYSPLL_REF_HFCLK,
    .inputFreq   = DL_SYSCTL_SYSPLL_INPUT_FREQ_8_16_MHZ,
    .pDiv        = DL_SYSCTL_SYSPLL_PDIV_2,
    .qDiv        = 31,
    .enableCLK1  = DL_SYSCTL_SYSPLL_CLK1_ENABLE,
    .enableCLK0  = DL_SYSCTL_SYSPLL_CLK0_ENABLE,
    .rDivClk1    = DL_SYSCTL_SYSPLL_RDIVCLK1_DIV2,
    .rDivClk0    = DL_SYSCTL_SYSPLL_RDIVCLK0_DIV2,
};
void  SYSCFG_DL_SYSCTL_CLK_init(void)
{
    // DL_SYSCTL_setHFCLKSourceXTAL(255, false);
    // DL_SYSCTL_configSYSPLL((DL_SYSCTL_SYSPLLConfig *)&gSYSPLLConfig);

    // // Before switching to PLL output, step down to a lower frequency and gradually increase
    // DL_SYSCTL_enablePLLDivider(DL_SYSCTL_PLL_DIVIDER_DIV4);
    // DL_SYSCTL_switchMCLKfromSYSOSCtoHSCLK(DL_SYSCTL_HSCLK_SOURCE_SYSPLL);
    // DL_SYSCTL_enablePLLDivider(DL_SYSCTL_PLL_DIVIDER_DIV2);
    // DL_Common_delayCycles(20);
    // while ((DL_SYSCTL_getClockStatus() & SYSCTL_CLKSTATUS_HSCLKMUX_MASK) != DL_SYSCTL_CLK_STATUS_MCLK_SOURCE_HSCLK);
    // DL_SYSCTL_disablePLLDivider();
    // DL_Common_delayCycles(20);
    // while ((DL_SYSCTL_getClockStatus() & SYSCTL_CLKSTATUS_HSCLKMUX_MASK) != DL_SYSCTL_CLK_STATUS_MCLK_SOURCE_HSCLK);


    // DL_SYSCTL_setMCLKDivider(DL_SYSCTL_MCLK_DIV_2_DIV_4);
    // DL_SYSCTL_setCANCLKSource(DL_SYSCTL_CANCLK_SOURCE_SYSPLL_DIV2);

}

void SYSCFG_DL_SYSCTL_init(void)
{
    DL_SYSCTL_setVREF(DL_SYSCTL_VREF_INTERNAL_3_3_V);

}

void	SYSCFG_DL_INPUTXBAR_init()
{
	DL_XBAR_setInputXBAR(DL_XBAR_INPUT1, DL_XBAR_DRIVE_LOW);
	DL_XBAR_setInputXBAR(DL_XBAR_INPUT2, 40);

}

void	SYSCFG_DL_OUTPUTXBAR_init()
{
    DL_XBAR_selectOutputXBARSource(DL_XBAR_OUTPUT5, DL_XBAR_OUT_CMPSS0_CTRIPOUTH);
    DL_XBAR_selectOutputXBARSource(DL_XBAR_OUTPUT5, DL_XBAR_OUT_CMPSS1_CTRIPOUTH);
    DL_XBAR_selectOutputXBARSource(DL_XBAR_OUTPUT5, DL_XBAR_OUT_CMPSS2_CTRIPOUTH);

}

void	SYSCFG_DL_PWMXBAR_init()
{
    DL_XBAR_selectPWMXBARSource(DL_XBAR_TRIP1, DL_XBAR_PWM_INPUTXBAR1);

    DL_XBAR_selectPWMXBARSource(DL_XBAR_TRIP2, DL_XBAR_PWM_INPUTXBAR2);

    DL_XBAR_selectPWMXBARSource(DL_XBAR_TRIP3, DL_XBAR_PWM_CMPSS0_CTRIPH);
    DL_XBAR_selectPWMXBARSource(DL_XBAR_TRIP3, DL_XBAR_PWM_CMPSS1_CTRIPH);
    DL_XBAR_selectPWMXBARSource(DL_XBAR_TRIP3, DL_XBAR_PWM_CMPSS2_CTRIPH);
    DL_XBAR_invertPWMXBARSignal(DL_XBAR_TRIP3, true);

}

static const DL_ADC_SocConfig gADC_0_SOC0_SocConfig = {
    .channel = DL_ADC_CH_ADCIN12,
    .enableComparator = false
};

static const DL_ADC_SocConfig gADC_0_SOC1_SocConfig = {
    .channel = DL_ADC_CH_ADCIN15,
    .enableComparator = false
};

static const DL_ADC_SeqNConfig gADC_0_SEQ1_SeqNConfig = {
    .enableSequencer = true,
    .sampleWindow = 15,
    .trigger = DL_ADC_TRIGGER_PWM4_SOCA,
    .socStartNumber = DL_ADC_SOC_NUMBER0,
    .enableSampleCapReset = false,
    .capResetVal = DL_ADC_SAMPLE_CAP_RESET_SELECT_VREFLO
};

static const DL_ADC_IntNConfig gADC_0_INT1_IntNConfig = {
    .enableContinuousMode = true,
    .enableInterrupt = true,
    .trigger = DL_ADC_SOC_NUMBER0
};

void SYSCFG_DL_ADC_0_init(void)
{
    DL_ADC_setPrescaler(ADC_0_INST, DL_ADC_CLOCK_DIVIDE_4_0);
    DL_ADC_setOffsetTrim(ADC_0_INST, DL_ADC_OFF_TRIM_0LSB);
    DL_ADC_socInit(ADC_0_INST, DL_ADC_SOC_NUMBER0, (DL_ADC_SocConfig *)&gADC_0_SOC0_SocConfig);
    DL_ADC_socInit(ADC_0_INST, DL_ADC_SOC_NUMBER1, (DL_ADC_SocConfig *)&gADC_0_SOC1_SocConfig);
    DL_ADC_setInterruptPulseMode(ADC_0_INST, DL_ADC_PULSE_END_OF_CONV);
    DL_ADC_setInterruptCycleDelay(ADC_0_INST, 0);
    DL_ADC_interruptInit(ADC_0_INST, DL_ADC_INT_NUMBER1, (DL_ADC_IntNConfig *)&gADC_0_INT1_IntNConfig);
    DL_ADC_setSeqEndSOC(ADC_0_INST, DL_ADC_SOC_NUMBER1);
    DL_ADC_setSeqPreemptMode(ADC_0_INST, DL_ADC_SEQ_PREEMPT_DISABLE);
    DL_ADC_sequencerInit(ADC_0_INST, DL_ADC_SEQ_NUMBER1, (DL_ADC_SeqNConfig *)&gADC_0_SEQ1_SeqNConfig);
}
static const DL_ADC_SocConfig gADC_1_SOC0_SocConfig = {
    .channel = DL_ADC_CH_ADCIN14,
    .enableComparator = false
};

static const DL_ADC_SocConfig gADC_1_SOC1_SocConfig = {
    .channel = DL_ADC_CH_ADCIN12,
    .enableComparator = false
};

static const DL_ADC_SeqNConfig gADC_1_SEQ1_SeqNConfig = {
    .enableSequencer = true,
    .sampleWindow = 15,
    .trigger = DL_ADC_TRIGGER_PWM4_SOCA,
    .socStartNumber = DL_ADC_SOC_NUMBER0,
    .enableSampleCapReset = false,
    .capResetVal = DL_ADC_SAMPLE_CAP_RESET_SELECT_VREFLO
};

static const DL_ADC_PpbConfig gADC_1_PPB1_PpbConfig = {
    .evtFlags = 0x0,
    .evtIntFlags = 0x0,
    .cbcClear = false,
    .tripHiLimit = 0,
    .tripLoLimit = 0,
    .oversamplingLimit = DL_ADC_PPB_LIMIT_NIL,
    .shiftVal = DL_ADC_PPB_SHIFT_0,
    .compSrc = DL_ADC_PPB_COMPSOURCE_RESULT,
    .syncIn = DL_ADC_SYNCIN_DISABLE
};

void SYSCFG_DL_ADC_1_init(void)
{
    DL_ADC_setPrescaler(ADC_1_INST, DL_ADC_CLOCK_DIVIDE_4_0);
    DL_ADC_setOffsetTrim(ADC_1_INST, DL_ADC_OFF_TRIM_0LSB);
    DL_ADC_socInit(ADC_1_INST, DL_ADC_SOC_NUMBER0, (DL_ADC_SocConfig *)&gADC_1_SOC0_SocConfig);
    DL_ADC_socInit(ADC_1_INST, DL_ADC_SOC_NUMBER1, (DL_ADC_SocConfig *)&gADC_1_SOC1_SocConfig);
    DL_ADC_ppbInit(ADC_1_INST, DL_ADC_PPB_NUMBER1, (DL_ADC_PpbConfig *)&gADC_1_PPB1_PpbConfig);
    DL_ADC_setInterruptPulseMode(ADC_1_INST, DL_ADC_PULSE_END_OF_ACQ_WIN);
    DL_ADC_setInterruptCycleDelay(ADC_1_INST, 0);
    DL_ADC_setSeqEndSOC(ADC_1_INST, DL_ADC_SOC_NUMBER1);
    DL_ADC_setSeqPreemptMode(ADC_1_INST, DL_ADC_SEQ_PREEMPT_DISABLE);
    DL_ADC_sequencerInit(ADC_1_INST, DL_ADC_SEQ_NUMBER1, (DL_ADC_SeqNConfig *)&gADC_1_SEQ1_SeqNConfig);
}
static const DL_ADC_SocConfig gADC_2_SOC0_SocConfig = {
    .channel = DL_ADC_CH_ADCIN27,
    .enableComparator = false
};

static const DL_ADC_SocConfig gADC_2_SOC1_SocConfig = {
    .channel = DL_ADC_CH_ADCIN30,
    .enableComparator = false
};

static const DL_ADC_SocConfig gADC_2_SOC2_SocConfig = {
    .channel = DL_ADC_CH_ADCIN17,
    .enableComparator = false
};

static const DL_ADC_SeqNConfig gADC_2_SEQ1_SeqNConfig = {
    .enableSequencer = true,
    .sampleWindow = 50,
    .trigger = DL_ADC_TRIGGER_PWM4_SOCA,
    .socStartNumber = DL_ADC_SOC_NUMBER0,
    .enableSampleCapReset = true,
    .capResetVal = DL_ADC_SAMPLE_CAP_RESET_SELECT_VREFLO
};

void SYSCFG_DL_ADC_2_init(void)
{
    DL_ADC_setPrescaler(ADC_2_INST, DL_ADC_CLOCK_DIVIDE_4_0);
    DL_ADC_setOffsetTrim(ADC_2_INST, DL_ADC_OFF_TRIM_0LSB);
    DL_ADC_socInit(ADC_2_INST, DL_ADC_SOC_NUMBER0, (DL_ADC_SocConfig *)&gADC_2_SOC0_SocConfig);
    DL_ADC_socInit(ADC_2_INST, DL_ADC_SOC_NUMBER1, (DL_ADC_SocConfig *)&gADC_2_SOC1_SocConfig);
    DL_ADC_socInit(ADC_2_INST, DL_ADC_SOC_NUMBER2, (DL_ADC_SocConfig *)&gADC_2_SOC2_SocConfig);
    DL_ADC_setInterruptPulseMode(ADC_2_INST, DL_ADC_PULSE_END_OF_ACQ_WIN);
    DL_ADC_setInterruptCycleDelay(ADC_2_INST, 0);
    DL_ADC_setSeqEndSOC(ADC_2_INST, DL_ADC_SOC_NUMBER2);
    DL_ADC_setSeqPreemptMode(ADC_2_INST, DL_ADC_SEQ_PREEMPT_DISABLE);
    DL_ADC_sequencerInit(ADC_2_INST, DL_ADC_SEQ_NUMBER1, (DL_ADC_SeqNConfig *)&gADC_2_SEQ1_SeqNConfig);
}


static const DL_CMPSSLITE_Config gMTR1_CMPSS_U_BASEConfig = {
    .enableModule      = true,
    .hysteresis        = DL_CMPSSLITE_HYSTERESIS_NONE,
    .dacSwLoadSelect   = DL_CMPSSLITE_DACVAL_MCLK,
    .highCompSSConfig  =
    {
        .useComp       = true,
        .useDac        = true,
        .useFilter     = true,
        .syncSource    = DL_CMPSSLITE_SYNCSOURCE_MCPWM0_SYNCPER,
        .compConfig =
        {
            .sourceSelect         = DL_CMPSSLITE_INSRC_DAC,
            .invert               = true,
            .tripSourceSelect     = DL_CMPSSLITE_TRIP_FILTER,
            .tripOutSourceSelect  = DL_CMPSSLITE_TRIPOUT_ASYNC_COMP,
        },
        .dacConfig =
        {
            .initialDacValue = 1800,
        },
        .filterConfig  =
        {
            .initFilter                = true,
            .filtInput                 = DL_CMPSSLITE_FILTIN_COMPOUT,
            .samplePrescale            = 1,
            .sampleWindow              = 8,
            .threshold                 = 8,
            .asyncORLatch              = true,
            .enableLatchResetOnPWMSYNC = true,
        },
    },
    .lowCompSSConfig =
    {
        .useComp     = false,
        .useDac      = false,
        .useFilter   = false,
        .syncSource  = DL_CMPSSLITE_SYNCSOURCE_MCPWM4_SYNCPER,
    }
};

void SYSCFG_DL_MTR1_CMPSS_U_BASE_init(void){
    DL_SYSCTL_setCompartorHPMux(DL_SYSCTL_CMPSS2_MUX, DL_SYSCTL_CMP_HP3);
    DL_CMPSSLITE_init(MTR1_CMPSS_U_BASE_INST, (DL_CMPSSLITE_Config *)&gMTR1_CMPSS_U_BASEConfig);
}


static const DL_CMPSSLITE_Config gMTR1_CMPSS_V_BASEConfig = {
    .enableModule      = true,
    .hysteresis        = DL_CMPSSLITE_HYSTERESIS_NONE,
    .dacSwLoadSelect   = DL_CMPSSLITE_DACVAL_MCLK,
    .highCompSSConfig  =
    {
        .useComp       = true,
        .useDac        = true,
        .useFilter     = true,
        .syncSource    = DL_CMPSSLITE_SYNCSOURCE_MCPWM0_SYNCPER,
        .compConfig =
        {
            .sourceSelect         = DL_CMPSSLITE_INSRC_DAC,
            .invert               = true,
            .tripSourceSelect     = DL_CMPSSLITE_TRIP_FILTER,
            .tripOutSourceSelect  = DL_CMPSSLITE_TRIPOUT_ASYNC_COMP,
        },
        .dacConfig =
        {
            .initialDacValue = 1800,
        },
        .filterConfig  =
        {
            .initFilter                = true,
            .filtInput                 = DL_CMPSSLITE_FILTIN_COMPOUT,
            .samplePrescale            = 1,
            .sampleWindow              = 8,
            .threshold                 = 8,
            .asyncORLatch              = true,
            .enableLatchResetOnPWMSYNC = true,
        },
    },
    .lowCompSSConfig =
    {
        .useComp     = false,
        .useDac      = false,
        .useFilter   = false,
        .syncSource  = DL_CMPSSLITE_SYNCSOURCE_MCPWM4_SYNCPER,
    }
};

void SYSCFG_DL_MTR1_CMPSS_V_BASE_init(void){
    DL_SYSCTL_setCompartorHPMux(DL_SYSCTL_CMPSS1_MUX, DL_SYSCTL_CMP_HP2);
    DL_CMPSSLITE_init(MTR1_CMPSS_V_BASE_INST, (DL_CMPSSLITE_Config *)&gMTR1_CMPSS_V_BASEConfig);
}


static const DL_CMPSSLITE_Config gMTR1_CMPSS_W_BASEConfig = {
    .enableModule      = true,
    .hysteresis        = DL_CMPSSLITE_HYSTERESIS_NONE,
    .dacSwLoadSelect   = DL_CMPSSLITE_DACVAL_MCLK,
    .highCompSSConfig  =
    {
        .useComp       = true,
        .useDac        = true,
        .useFilter     = true,
        .syncSource    = DL_CMPSSLITE_SYNCSOURCE_MCPWM0_SYNCPER,
        .compConfig =
        {
            .sourceSelect         = DL_CMPSSLITE_INSRC_DAC,
            .invert               = true,
            .tripSourceSelect     = DL_CMPSSLITE_TRIP_FILTER,
            .tripOutSourceSelect  = DL_CMPSSLITE_TRIPOUT_ASYNC_COMP,
        },
        .dacConfig =
        {
            .initialDacValue = 1800,
        },
        .filterConfig  =
        {
            .initFilter                = true,
            .filtInput                 = DL_CMPSSLITE_FILTIN_COMPOUT,
            .samplePrescale            = 1,
            .sampleWindow              = 8,
            .threshold                 = 8,
            .asyncORLatch              = true,
            .enableLatchResetOnPWMSYNC = true,
        },
    },
    .lowCompSSConfig =
    {
        .useComp     = false,
        .useDac      = false,
        .useFilter   = false,
        .syncSource  = DL_CMPSSLITE_SYNCSOURCE_MCPWM4_SYNCPER,
    }
};

void SYSCFG_DL_MTR1_CMPSS_W_BASE_init(void){
    DL_SYSCTL_setCompartorHPMux(DL_SYSCTL_CMPSS0_MUX, DL_SYSCTL_CMP_HP0);
    DL_CMPSSLITE_init(MTR1_CMPSS_W_BASE_INST, (DL_CMPSSLITE_Config *)&gMTR1_CMPSS_W_BASEConfig);
}


static const DL_CMPSSLITE_Config gFOC_DAC_OUTConfig = {
    .enableModule      = true,
    .hysteresis        = DL_CMPSSLITE_HYSTERESIS_NONE,
    .dacSwLoadSelect   = DL_CMPSSLITE_DACVAL_MCLK,
    .highCompSSConfig  =
    {
        .useComp       = false,
        .useDac        = false,
        .useFilter     = false,
        .syncSource    = DL_CMPSSLITE_SYNCSOURCE_MCPWM0_SYNCPER,
    },
    .lowCompSSConfig =
    {
        .useComp     = false,
        .useDac      = true,
        .useFilter   = false,
        .syncSource  = DL_CMPSSLITE_SYNCSOURCE_MCPWM0_SYNCPER,
        .dacConfig =
        {
            .initialDacValue = 1000,
        },
    }
};

void SYSCFG_DL_FOC_DAC_OUT_init(void){
    DL_SYSCTL_enableDACOUT(DL_SYSCTL_DACOUTEN_CMPSS3_DACL);
    DL_CMPSSLITE_init(FOC_DAC_OUT_INST, (DL_CMPSSLITE_Config *)&gFOC_DAC_OUTConfig);
}



static const DL_ECAP_Config gIPD_CAPTUREConfig = {
    .modeSelect = DL_ECAP_OPERATING_MODE_CAPTURE,
    .captureModeConfig =
    {
        .input                              = DL_ECAP_INPUT_OUTPUTXBAR5,
        .prescalerValue                     = 0,
        .continouousOrOneShot               = DL_ECAP_CONTINUOUS_CAPTURE_MODE,
        .wrapOrStopAtEvent                  = DL_ECAP_EVENT_1,
        .enableCounterResetOnCaptureEvent1  = false,
        .enableCounterResetOnCaptureEvent2  = false,
        .enableCounterResetOnCaptureEvent3  = false,
        .enableCounterResetOnCaptureEvent4  = false,
        .captureEvent1Polarity              = DL_ECAP_EVENT_RISING_EDGE,
        .captureEvent2Polarity              = DL_ECAP_EVENT_RISING_EDGE,
        .captureEvent3Polarity              = DL_ECAP_EVENT_RISING_EDGE,
        .captureEvent4Polarity              = DL_ECAP_EVENT_RISING_EDGE,
        .resetCounter                       = true,
        .reArm                              = true
    },
    .syncConfig =
    {
        .ecapSyncInSel     = DL_ECAP_SYNC_IN_PULSE_SRC_DISABLE,
        .phaseShiftCount   = 0,
        .syncOutSelect     = DL_ECAP_SYNC_OUT_DISABLED,
        .enableLoadCounter = false
        //todo loadCounter in js file unused
    },
    .interruptsConfig =
    {
        .interruptSourceEnableMask = 0,
        .dmaEventSelect            = DL_ECAP_EVENT_1
    },
    .emulationMode  = DL_ECAP_EMULATION_FREE_RUN
};

void SYSCFG_DL_IPD_CAPTURE_init(void){
    DL_ECAP_init(IPD_CAPTURE_INST, (DL_ECAP_Config *)&gIPD_CAPTUREConfig);
}


static const DL_MCPWM_TimeBaseConfig gMTR1_MCPWM_BASE_TimeBaseConfig =
{
    .clockDivider              = DL_MCPWM_CLOCK_DIVIDER_1,
    .counterMode               = DL_MCPWM_COUNTER_MODE_UP_DOWN,
    .counterDirectionAfterSync = DL_MCPWM_COUNT_MODE_DOWN_AFTER_SYNC,
    .periodValue               = 10000,
    .initialCounterValue       = 0,
    .enablePhaseShift          = false,
    .phaseShiftValue           = 0,
    .syncInSource              = DL_MCPWM_SYNC_IN_PULSE_SRC_DISABLE,
    .syncPerSource             = DL_MCPWM_SYNCPERSEL_SOURCE_PERIOD,
    .syncOutPulseMode          = DL_MCPWM_SYNC_OUT_PULSE_DISABLED,
    .emulationMode             = DL_MCPWM_EMULATION_FREE_RUN,
};

static const DL_MCPWM_CounterCompareConfig gMTR1_MCPWM_BASE_CounterCompareConfig =
{
    .pwm1CompareAValue = 0,
    .pwm1CompareBValue = 0,
    .pwm2CompareAValue = 0,
    .pwm2CompareBValue = 0,
    .pwm3CompareAValue = 0,
    .pwm3CompareBValue = 0,
    .compareCValue     = 0,
    .compareDValue     = 0,
};

static const DL_MCPWM_Actions gMTR1_MCPWM_BASE_ActionQualifierActions_PWM1A =
{
    .actionAtZero          = DL_MCPWM_AQ_OUTPUT_LOW,
    .actionAtPeriod        = DL_MCPWM_AQ_OUTPUT_HIGH,
    .actionAtCountUpCmpA   = DL_MCPWM_AQ_OUTPUT_HIGH,
    .actionAtCountDownCmpA = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountUpCmpB   = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountDownCmpB = DL_MCPWM_AQ_OUTPUT_LOW,
    .pwmSwForceAction      = DL_MCPWM_AQ_SW_FORCE_DISABLED,
};

static const DL_MCPWM_Actions gMTR1_MCPWM_BASE_ActionQualifierActions_PWM1B =
{
    .actionAtZero          = DL_MCPWM_AQ_OUTPUT_HIGH,
    .actionAtPeriod        = DL_MCPWM_AQ_OUTPUT_LOW,
    .actionAtCountUpCmpA   = DL_MCPWM_AQ_OUTPUT_LOW,
    .actionAtCountDownCmpA = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountUpCmpB   = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountDownCmpB = DL_MCPWM_AQ_OUTPUT_HIGH,
    .pwmSwForceAction      = DL_MCPWM_AQ_SW_FORCE_DISABLED,
};

static const DL_MCPWM_Actions gMTR1_MCPWM_BASE_ActionQualifierActions_PWM2A =
{
    .actionAtZero          = DL_MCPWM_AQ_OUTPUT_LOW,
    .actionAtPeriod        = DL_MCPWM_AQ_OUTPUT_HIGH,
    .actionAtCountUpCmpA   = DL_MCPWM_AQ_OUTPUT_HIGH,
    .actionAtCountDownCmpA = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountUpCmpB   = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountDownCmpB = DL_MCPWM_AQ_OUTPUT_LOW,
    .pwmSwForceAction      = DL_MCPWM_AQ_SW_FORCE_DISABLED,
};

static const DL_MCPWM_Actions gMTR1_MCPWM_BASE_ActionQualifierActions_PWM2B =
{
    .actionAtZero          = DL_MCPWM_AQ_OUTPUT_HIGH,
    .actionAtPeriod        = DL_MCPWM_AQ_OUTPUT_LOW,
    .actionAtCountUpCmpA   = DL_MCPWM_AQ_OUTPUT_LOW,
    .actionAtCountDownCmpA = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountUpCmpB   = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountDownCmpB = DL_MCPWM_AQ_OUTPUT_HIGH,
    .pwmSwForceAction      = DL_MCPWM_AQ_SW_FORCE_DISABLED,
};

static const DL_MCPWM_Actions gMTR1_MCPWM_BASE_ActionQualifierActions_PWM3A =
{
    .actionAtZero          = DL_MCPWM_AQ_OUTPUT_LOW,
    .actionAtPeriod        = DL_MCPWM_AQ_OUTPUT_HIGH,
    .actionAtCountUpCmpA   = DL_MCPWM_AQ_OUTPUT_HIGH,
    .actionAtCountDownCmpA = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountUpCmpB   = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountDownCmpB = DL_MCPWM_AQ_OUTPUT_LOW,
    .pwmSwForceAction      = DL_MCPWM_AQ_SW_FORCE_DISABLED,
};

static const DL_MCPWM_Actions gMTR1_MCPWM_BASE_ActionQualifierActions_PWM3B =
{
    .actionAtZero          = DL_MCPWM_AQ_OUTPUT_HIGH,
    .actionAtPeriod        = DL_MCPWM_AQ_OUTPUT_LOW,
    .actionAtCountUpCmpA   = DL_MCPWM_AQ_OUTPUT_LOW,
    .actionAtCountDownCmpA = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountUpCmpB   = DL_MCPWM_AQ_OUTPUT_NO_CHANGE,
    .actionAtCountDownCmpB = DL_MCPWM_AQ_OUTPUT_HIGH,
    .pwmSwForceAction      = DL_MCPWM_AQ_SW_FORCE_DISABLED,
};

static const DL_MCPWM_DeadBandConfig gMTR1_MCPWM_BASE_DeadBandConfig =
{
    .enableRisingEdgeDelayOnPathA   = true,
    .enableFallingEdgeDelayOnPathB  = true,
    .enableOutputSwapA              = false,
    .enableOutputSwapB              = false,
    .risingEdgeDelayInputSource     = DL_MCPWM_DB_INPUT_PWMA,
    .fallingEdgeDelayInputSource    = DL_MCPWM_DB_INPUT_PWMA,
    .risingEdgeDelayCount           = 20,
    .fallingEdgeDelayCount          = 10,
    .risingEdgeDelayPolarity        = DL_MCPWM_DB_POLARITY_ACTIVE_HIGH,
    .fallingEdgeDelayPolarity       = DL_MCPWM_DB_POLARITY_ACTIVE_LOW,
};

static const DL_MCPWM_TripZoneConfig gMTR1_MCPWM_BASE_TripZoneConfig =
{
    .enableTripZoneMask   = DL_MCPWM_TZ_SIGNAL_OSHT1 | DL_MCPWM_TZ_SIGNAL_OSHT2 | DL_MCPWM_TZ_SIGNAL_OSHT3,
    .cbcClearEvent        = DL_MCPWM_TZ_CBC_PULSE_CLR_NONE,
    .actionOnA            = DL_MCPWM_TZ_ACTION_LOW,
    .actionOnB            = DL_MCPWM_TZ_ACTION_LOW,
};

static const DL_MCPWM_EventTriggerConfig gMTR1_MCPWM_BASE_EventTriggerConfig =
{
    .socPulseGenConfig =
    {
        .SOCA =
        {
            .enable      = true,
            .eventSource = DL_MCPWM_SOC_TBCTR_U_CMPC,
            .eventPeriod = 1,
        },
        .SOCB =
        {
            .enable      = false,
            .eventSource = DL_MCPWM_SOC_DISABLED,
            .eventPeriod = 1,
        },
        .SOCC =
        {
            .enable      = false,
            .eventSource = DL_MCPWM_SOC_DISABLED,
            .eventPeriod = 1,
        },
        .SOCD =
        {
            .enable      = false,
            .eventSource = DL_MCPWM_SOC_DISABLED,
            .eventPeriod = 1,
        }
    },
    .etConfig =
    {
        .ET1 =
        {
            .eventSource = DL_MCPWM_EVT_DISABLED,
            .eventPeriod = 1,
        },
        .ET2 =
        {
            .eventSource = DL_MCPWM_EVT_DISABLED,
            .eventPeriod = 1,
        }
    },
    .interruptGenConfig = 0
};

static const DL_MCPWM_LoadModeConfig gMTR1_MCPWM_BASE_LoadModeConfig =
{
    .pwm1CmpALoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm1CmpBLoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm2CmpALoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm2CmpBLoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm3CmpALoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm3CmpBLoadMode   = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .cmpCLoadMode       = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .cmpDLoadMode       = DL_MCPWM_COMP_LOAD_ON_CNTR_ZERO,
    .pwm1AaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm1BaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm2AaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm2BaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm3AaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .pwm3BaqLoadMode    = DL_MCPWM_AQ_LOAD_ON_CNTR_ZERO,
    .globalLoadConfig   =
    {
        .enableGlobalLoad   = false,
        .triggerEvent       = DL_MCPWM_GL_LOAD_PULSE_CNTR_ZERO,
        .enableOneShotMode  = false,
        .enableOneShotLatch = false,
        .forceLoadEvent     = false,
    }
};

void SYSCFG_DL_MTR1_MCPWM_BASE_init(void)
{
    DL_MCPWM_configureTimeBase(MTR1_MCPWM_BASE_INST, (DL_MCPWM_TimeBaseConfig *)&gMTR1_MCPWM_BASE_TimeBaseConfig);
    DL_MCPWM_configureCounterCompare(MTR1_MCPWM_BASE_INST, (DL_MCPWM_CounterCompareConfig *)&gMTR1_MCPWM_BASE_CounterCompareConfig);
    DL_MCPWM_configureActionQualifierActions(MTR1_MCPWM_BASE_INST, DL_MCPWM_AQ_OUTPUT_1A, (DL_MCPWM_Actions *)&gMTR1_MCPWM_BASE_ActionQualifierActions_PWM1A);
    DL_MCPWM_configureActionQualifierActions(MTR1_MCPWM_BASE_INST, DL_MCPWM_AQ_OUTPUT_1B, (DL_MCPWM_Actions *)&gMTR1_MCPWM_BASE_ActionQualifierActions_PWM1B);
    DL_MCPWM_configureActionQualifierActions(MTR1_MCPWM_BASE_INST, DL_MCPWM_AQ_OUTPUT_2A, (DL_MCPWM_Actions *)&gMTR1_MCPWM_BASE_ActionQualifierActions_PWM2A);
    DL_MCPWM_configureActionQualifierActions(MTR1_MCPWM_BASE_INST, DL_MCPWM_AQ_OUTPUT_2B, (DL_MCPWM_Actions *)&gMTR1_MCPWM_BASE_ActionQualifierActions_PWM2B);
    DL_MCPWM_configureActionQualifierActions(MTR1_MCPWM_BASE_INST, DL_MCPWM_AQ_OUTPUT_3A, (DL_MCPWM_Actions *)&gMTR1_MCPWM_BASE_ActionQualifierActions_PWM3A);
    DL_MCPWM_configureActionQualifierActions(MTR1_MCPWM_BASE_INST, DL_MCPWM_AQ_OUTPUT_3B, (DL_MCPWM_Actions *)&gMTR1_MCPWM_BASE_ActionQualifierActions_PWM3B);
    DL_MCPWM_configureDeadBand(MTR1_MCPWM_BASE_INST, (DL_MCPWM_DeadBandConfig *)&gMTR1_MCPWM_BASE_DeadBandConfig);
    DL_MCPWM_configureTripZone(MTR1_MCPWM_BASE_INST, (DL_MCPWM_TripZoneConfig *)&gMTR1_MCPWM_BASE_TripZoneConfig);
    DL_MCPWM_configureEventTrigger(MTR1_MCPWM_BASE_INST, (DL_MCPWM_EventTriggerConfig *)&gMTR1_MCPWM_BASE_EventTriggerConfig);
    DL_MCPWM_configureLoadMode(MTR1_MCPWM_BASE_INST, (DL_MCPWM_LoadModeConfig *)&gMTR1_MCPWM_BASE_LoadModeConfig);

    // IOMUX Setting
    DL_GPIO_initPeripheralOutputFunction(IOMUX_PINCM_PB3, IOMUX_PB3_MCPWM4_1A);
    DL_GPIO_initPeripheralOutputFunction(IOMUX_PINCM_PA1, IOMUX_PA1_MCPWM4_1B);
    DL_GPIO_initPeripheralOutputFunction(IOMUX_PINCM_PA0, IOMUX_PA0_MCPWM4_2A);
    DL_GPIO_initPeripheralOutputFunction(IOMUX_PINCM_PA2, IOMUX_PA2_MCPWM4_2B);
    DL_GPIO_initPeripheralOutputFunction(IOMUX_PINCM_PB2, IOMUX_PB2_MCPWM4_3A);
    DL_GPIO_initPeripheralOutputFunction(IOMUX_PINCM_PA4, IOMUX_PA4_MCPWM4_3B);
}

static const DL_UART_ClockConfig gSERIAL_DATA_PLOT_ClockConfig = {
    .clockSel    = DL_UART_CLOCK_BUSCLK,
    .divideRatio = DL_UART_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Config gSERIAL_DATA_PLOT_Config = {
    .mode        = DL_UART_MODE_NORMAL,
    .direction   = DL_UART_DIRECTION_TX_RX,
    .flowControl = DL_UART_FLOW_CONTROL_NONE,
    .parity      = DL_UART_PARITY_NONE,
    .wordLength  = DL_UART_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_STOP_BITS_ONE
};

void SYSCFG_DL_SERIAL_DATA_PLOT_init(void)
{
    // DL_UART_setClockConfig(SERIAL_DATA_PLOT_INST, (DL_UART_ClockConfig *) &gSERIAL_DATA_PLOT_ClockConfig);

    // DL_UART_init(SERIAL_DATA_PLOT_INST, (DL_UART_Config *) &gSERIAL_DATA_PLOT_Config);
    // /*
    //  * Configure baud rate by setting oversampling and baud rate divisors.
    //  *  Target baud rate: 2000000
    //  *  Actual baud rate: 2000000
    //  */
    // DL_UART_setOversampling(SERIAL_DATA_PLOT_INST, DL_UART_OVERSAMPLING_RATE_16X);
    // DL_UART_setBaudRateDivisor(SERIAL_DATA_PLOT_INST, SERIAL_DATA_PLOT_IBRD_100_MHZ_2000000_BAUD, SERIAL_DATA_PLOT_FBRD_100_MHZ_2000000_BAUD);

    // // /* Configure FIFOs */
    // // DL_UART_setRXFIFOThreshold(SERIAL_DATA_PLOT_INST, DL_UART_RX_FIFO_LEVEL_FULL);
    // // DL_UART_setTXFIFOThreshold(SERIAL_DATA_PLOT_INST, DL_UART_TX_FIFO_LEVEL_1_2_EMPTY);

    // DL_UART_enable(SERIAL_DATA_PLOT_INST);
}

