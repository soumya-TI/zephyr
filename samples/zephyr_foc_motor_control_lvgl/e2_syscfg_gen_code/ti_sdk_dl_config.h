
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

#ifndef ti_sdk_dl_config_h
#define ti_sdk_dl_config_h

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#endif

#include <driverlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_Pinmux_init(void);
void SYSCFG_DL_FLASH_init(void);


/* Defines for Clock Frequencies */
#define CPUCLK_FREQ_HZ                                                  200000000
#define MCLK_FREQ_HZ                                                    200000000
#define SEMIMCLK_FREQ_HZ                                                100000000
#define QTRMCLK_FREQ_HZ                                                  50000000
#define CANCLK_FREQ_HZ                                                  100000000
#define SYSOSC_FREQ_HZ                                                   32000000
#define LFOSC_FREQ_HZ                                                       32000
#define HFCLK_FREQ_HZ                                                    25000000
#define SYSPLLCLK0_FREQ_HZ                                              200000000
#define SYSPLLCLK1_FREQ_HZ                                              200000000


/*
Macros to call DL_Common_delayCycles to achieve a delay in microseconds / milliseconds.
These macros convert the desired delay in us/ms to the count value expected
by the function.
Note that the function DL_Common_delayCycles only guarantees at least this
number of cycles will be delayed, not that exactly this number of cycles will be
delayed. Please refer to the function documentation for more details.
*/
#define DEVICE_DELAY_US(us)   DL_Common_delayCycles((uint32_t)(us) * (CPUCLK_FREQ_HZ / 1000000))
#define DEVICE_DELAY_MS(ms)   DL_Common_delayCycles((uint32_t)(ms) * (CPUCLK_FREQ_HZ / 1000))




/* Defines for GATE_EN_GPIO: GPIO0.3 with pinCMx PA3 on package pin 17 */
#define MTR1_GATE_DRIVER_GATE_EN_GPIO_PORT                               (GPIO0)
#define MTR1_GATE_DRIVER_GATE_EN_GPIO_PORT_BASE                     (GPIO0_BASE)
#define MTR1_GATE_DRIVER_GATE_EN_GPIO_PIN                       (DL_GPIO_PIN(3))
#define MTR1_GATE_DRIVER_GATE_EN_GPIO_IOMUX                    (IOMUX_PINCM_PA3)
/* Defines for nFAULT_GPIO: GPIO1.4 with pinCMx PB4 on package pin 22 */
#define MTR1_GATE_DRIVER_nFAULT_GPIO_PORT                                (GPIO1)
#define MTR1_GATE_DRIVER_nFAULT_GPIO_PORT_BASE                      (GPIO1_BASE)
#define MTR1_GATE_DRIVER_nFAULT_GPIO_PIN                        (DL_GPIO_PIN(4))
#define MTR1_GATE_DRIVER_nFAULT_GPIO_IOMUX                     (IOMUX_PINCM_PB4)
/* Defines for GATE_MODE_GPIO: GPIO1.7 with pinCMx PB7 on package pin 39 */
#define MTR1_GATE_DRIVER_GATE_MODE_GPIO_PORT                             (GPIO1)
#define MTR1_GATE_DRIVER_GATE_MODE_GPIO_PORT_BASE                   (GPIO1_BASE)
#define MTR1_GATE_DRIVER_GATE_MODE_GPIO_PIN                     (DL_GPIO_PIN(7))
#define MTR1_GATE_DRIVER_GATE_MODE_GPIO_IOMUX                  (IOMUX_PINCM_PB7)
/* Defines for GATE_GAIN_GPIO: GPIO1.12 with pinCMx PB12 on package pin 54 */
#define MTR1_GATE_DRIVER_GATE_GAIN_GPIO_PORT                             (GPIO1)
#define MTR1_GATE_DRIVER_GATE_GAIN_GPIO_PORT_BASE                   (GPIO1_BASE)
#define MTR1_GATE_DRIVER_GATE_GAIN_GPIO_PIN                    (DL_GPIO_PIN(12))
#define MTR1_GATE_DRIVER_GATE_GAIN_GPIO_IOMUX                 (IOMUX_PINCM_PB12)
/* Defines for GATE_CAL_GPIO: GPIO1.11 with pinCMx PB11 on package pin 53 */
#define MTR1_GATE_DRIVER_GATE_CAL_GPIO_PORT                              (GPIO1)
#define MTR1_GATE_DRIVER_GATE_CAL_GPIO_PORT_BASE                    (GPIO1_BASE)
#define MTR1_GATE_DRIVER_GATE_CAL_GPIO_PIN                     (DL_GPIO_PIN(11))
#define MTR1_GATE_DRIVER_GATE_CAL_GPIO_IOMUX                  (IOMUX_PINCM_PB11)
/* Defines for FAULT_OUTPUT_GPIO: GPIO1.6 with pinCMx PB6 on package pin 38 */
#define MTR1_FOC_CTRL_FAULT_OUTPUT_GPIO_PORT                             (GPIO1)
#define MTR1_FOC_CTRL_FAULT_OUTPUT_GPIO_PORT_BASE                   (GPIO1_BASE)
#define MTR1_FOC_CTRL_FAULT_OUTPUT_GPIO_PIN                     (DL_GPIO_PIN(6))
#define MTR1_FOC_CTRL_FAULT_OUTPUT_GPIO_IOMUX                  (IOMUX_PINCM_PB6)
/* Defines for ISR_TIMING: GPIO0.23 with pinCMx PA23 on package pin 60 */
#define MTR1_FOC_CTRL_ISR_TIMING_PORT                                    (GPIO0)
#define MTR1_FOC_CTRL_ISR_TIMING_PORT_BASE                          (GPIO0_BASE)
#define MTR1_FOC_CTRL_ISR_TIMING_PIN                           (DL_GPIO_PIN(24))
#define MTR1_FOC_CTRL_ISR_TIMING_IOMUX                        (IOMUX_PINCM_PA24)


/* Defines for MTR1_STOP_ACTION */
#define MTR1_STOP_ACTION                                          DL_XBAR_INPUT1
/* Defines for MTR1_nFAULT_TRIP */
#define MTR1_nFAULT_TRIP                                          DL_XBAR_INPUT2



/* Defines for IPD_ECAP_INPUT */
#define IPD_ECAP_INPUT                                              XBAR_OUTPUT5



/* Defines for MTR_STOP_INPUT */
#define MTR_STOP_INPUT                                             DL_XBAR_TRIP1
#define MTR_STOP_INPUT_CBC                               DL_MCPWM_TZ_SIGNAL_CBC1
#define MTR_STOP_INPUT_OSHT                             DL_MCPWM_TZ_SIGNAL_OSHT1
/* Defines for MTR1_GD_nFAULT_XBAR_INP */
#define MTR1_GD_nFAULT_XBAR_INP                                    DL_XBAR_TRIP2
#define MTR1_GD_nFAULT_XBAR_INP_CBC                      DL_MCPWM_TZ_SIGNAL_CBC2
#define MTR1_GD_nFAULT_XBAR_INP_OSHT                    DL_MCPWM_TZ_SIGNAL_OSHT2
/* Defines for MTR1_CMP_nFAULT_XBAR_INP */
#define MTR1_CMP_nFAULT_XBAR_INP                                   DL_XBAR_TRIP3
#define MTR1_CMP_nFAULT_XBAR_INP_CBC                     DL_MCPWM_TZ_SIGNAL_CBC3
#define MTR1_CMP_nFAULT_XBAR_INP_OSHT                   DL_MCPWM_TZ_SIGNAL_OSHT3



#define ADC_0_INST                                                           ADC0
#define ADC_0_BASE                                                      ADC0_BASE
#define ADC_0_RESULT_INST                                              ADC0RESULT
#define ADC_0_RESULT_BASE                                         ADC0RESULT_BASE
#define MTR1_IW_SEN                                            DL_ADC_SOC_NUMBER0
#define MTR1_IW_SEN_ADC_INST                                                 ADC0
#define MTR1_IW_SEN_ADC_BASE                                            ADC0_BASE
#define MTR1_IW_SEN_ADC_RESULT_INST                                    ADC0RESULT
#define MTR1_IW_SEN_ADC_RESULT_BASE                               ADC0RESULT_BASE
#define MTR1_IW_SEN_CHANNEL                                     DL_ADC_CH_ADCIN12
#define MTR1_VU_SEN                                            DL_ADC_SOC_NUMBER1
#define MTR1_VU_SEN_ADC_INST                                                 ADC0
#define MTR1_VU_SEN_ADC_BASE                                            ADC0_BASE
#define MTR1_VU_SEN_ADC_RESULT_INST                                    ADC0RESULT
#define MTR1_VU_SEN_ADC_RESULT_BASE                               ADC0RESULT_BASE
#define MTR1_VU_SEN_CHANNEL                                     DL_ADC_CH_ADCIN15
#define ADC_1_INST                                                           ADC1
#define ADC_1_BASE                                                      ADC1_BASE
#define ADC_1_RESULT_INST                                              ADC1RESULT
#define ADC_1_RESULT_BASE                                         ADC1RESULT_BASE
#define MTR1_IV_SEN                                            DL_ADC_SOC_NUMBER0
#define MTR1_IV_SEN_ADC_INST                                                 ADC1
#define MTR1_IV_SEN_ADC_BASE                                            ADC1_BASE
#define MTR1_IV_SEN_ADC_RESULT_INST                                    ADC1RESULT
#define MTR1_IV_SEN_ADC_RESULT_BASE                               ADC1RESULT_BASE
#define MTR1_IV_SEN_CHANNEL                                     DL_ADC_CH_ADCIN14
#define MTR1_VV_SEN                                            DL_ADC_SOC_NUMBER1
#define MTR1_VV_SEN_ADC_INST                                                 ADC1
#define MTR1_VV_SEN_ADC_BASE                                            ADC1_BASE
#define MTR1_VV_SEN_ADC_RESULT_INST                                    ADC1RESULT
#define MTR1_VV_SEN_ADC_RESULT_BASE                               ADC1RESULT_BASE
#define MTR1_VV_SEN_CHANNEL                                     DL_ADC_CH_ADCIN12
#define ADC_1_PPB1                                             DL_ADC_PPB_NUMBER1
#define ADC_2_INST                                                           ADC2
#define ADC_2_BASE                                                      ADC2_BASE
#define ADC_2_RESULT_INST                                              ADC2RESULT
#define ADC_2_RESULT_BASE                                         ADC2RESULT_BASE
#define MTR1_IU_SEN                                            DL_ADC_SOC_NUMBER0
#define MTR1_IU_SEN_ADC_INST                                                 ADC2
#define MTR1_IU_SEN_ADC_BASE                                            ADC2_BASE
#define MTR1_IU_SEN_ADC_RESULT_INST                                    ADC2RESULT
#define MTR1_IU_SEN_ADC_RESULT_BASE                               ADC2RESULT_BASE
#define MTR1_IU_SEN_CHANNEL                                     DL_ADC_CH_ADCIN27
#define MTR1_VW_SEN                                            DL_ADC_SOC_NUMBER1
#define MTR1_VW_SEN_ADC_INST                                                 ADC2
#define MTR1_VW_SEN_ADC_BASE                                            ADC2_BASE
#define MTR1_VW_SEN_ADC_RESULT_INST                                    ADC2RESULT
#define MTR1_VW_SEN_ADC_RESULT_BASE                               ADC2RESULT_BASE
#define MTR1_VW_SEN_CHANNEL                                     DL_ADC_CH_ADCIN30
#define MTR1_VDC_SEN                                           DL_ADC_SOC_NUMBER2
#define MTR1_VDC_SEN_ADC_INST                                                ADC2
#define MTR1_VDC_SEN_ADC_BASE                                           ADC2_BASE
#define MTR1_VDC_SEN_ADC_RESULT_INST                                   ADC2RESULT
#define MTR1_VDC_SEN_ADC_RESULT_BASE                              ADC2RESULT_BASE
#define MTR1_VDC_SEN_CHANNEL                                    DL_ADC_CH_ADCIN17



/* Defines for MTR1_CMPSS_U_BASE */
#define MTR1_CMPSS_U_BASE_INST                                            CMPSS2
#define MTR1_CMPSS_U_BASE_BASE                                       CMPSS2_BASE
#define MTR1_CMPSS_U_BASE_HYSTERESIS                DL_CMPSSLITE_HYSTERESIS_NONE
/* Defines for MTR1_CMPSS_V_BASE */
#define MTR1_CMPSS_V_BASE_INST                                            CMPSS1
#define MTR1_CMPSS_V_BASE_BASE                                       CMPSS1_BASE
#define MTR1_CMPSS_V_BASE_HYSTERESIS                DL_CMPSSLITE_HYSTERESIS_NONE
/* Defines for MTR1_CMPSS_W_BASE */
#define MTR1_CMPSS_W_BASE_INST                                            CMPSS0
#define MTR1_CMPSS_W_BASE_BASE                                       CMPSS0_BASE
#define MTR1_CMPSS_W_BASE_HYSTERESIS                DL_CMPSSLITE_HYSTERESIS_NONE



/* Defines for IPD_CAPTURE */
#define IPD_CAPTURE_INST                                                   ECAP0
#define IPD_CAPTURE_BASE                                              ECAP0_BASE



/* Defines for MTR1_MCPWM_BASE */
#define MTR1_MCPWM_BASE_INST                                              MCPWM4
#define MTR1_MCPWM_BASE_BASE                                         MCPWM4_BASE
#define MTR1_MCPWM_BASE_PERIOD                                             10000
#define MTR1_MCPWM_BASE_TBPHS                                                  0
#define MTR1_MCPWM_BASE_PWM1_CMPA                                              0
#define MTR1_MCPWM_BASE_PWM1_CMPB                                              0
#define MTR1_MCPWM_BASE_PWM2_CMPA                                              0
#define MTR1_MCPWM_BASE_PWM2_CMPB                                              0
#define MTR1_MCPWM_BASE_PWM3_CMPA                                              0
#define MTR1_MCPWM_BASE_PWM3_CMPB                                              0
#define MTR1_MCPWM_BASE_CMPC                                                   0
#define MTR1_MCPWM_BASE_CMPD                                                   0



/* Defines for SERIAL_DATA_PLOT */
#define SERIAL_DATA_PLOT_INST                                       UC1_INST_PTR
#define SERIAL_DATA_PLOT_INST_FREQUENCY                                100000000
#define GPIO_SERIAL_DATA_PLOT_TX_PORT                                      GPIO0
#define GPIO_SERIAL_DATA_PLOT_TX_PIN                               DL_GPIO_PIN_0
#define GPIO_SERIAL_DATA_PLOT_IOMUX_TX                         (IOMUX_PINCM_PA0)
#define GPIO_SERIAL_DATA_PLOT_IOMUX_TX_FUNC               IOMUX_PA0_UC1_TX_SDA_PICO
#define SERIAL_DATA_PLOT_BAUD_RATE                                     (4000000)
#define SERIAL_DATA_PLOT_IBRD_100_MHZ_4000000_BAUD                           (1)
#define SERIAL_DATA_PLOT_FBRD_100_MHZ_4000000_BAUD                          (36)



void SYSCFG_DL_SYSCTL_CLK_init(void);
void SYSCFG_DL_SYSCTL_init(void);

void SYSCFG_DL_INPUTXBAR_init(void);
void SYSCFG_DL_OUTPUTXBAR_init(void);
void SYSCFG_DL_PWMXBAR_init(void);
void SYSCFG_DL_ADC_0_init(void);
void SYSCFG_DL_ADC_1_init(void);
void SYSCFG_DL_ADC_2_init(void);
void SYSCFG_DL_MTR1_CMPSS_U_BASE_init(void);
void SYSCFG_DL_MTR1_CMPSS_V_BASE_init(void);
void SYSCFG_DL_MTR1_CMPSS_W_BASE_init(void);
void SYSCFG_DL_IPD_CAPTURE_init(void);
void SYSCFG_DL_MTR1_MCPWM_BASE_init(void);
void SYSCFG_DL_SERIAL_DATA_PLOT_init(void);


#ifdef __cplusplus
}
#endif

#endif /* ti_sdk_dl_config_h */