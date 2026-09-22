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

#include "application.h"
#include "user.h"

void motor1CtrlISR(void)
{

}
/* ADC Trigger Interrupt to execute the FOC state machine*/
void ADC0_INT1_IRQHandler(void)
{
    // Acknowledge ADC interrupt
    HAL_ackMtrADCInt(&motorVars_M1.halInterface->halFOCConfigHandle->intCfg);

    HAL_setGPIOHigh((uint32_t)MTR1_FOC_CTRL_ISR_TIMING_PORT,MTR1_FOC_CTRL_ISR_TIMING_PIN);

    /* Runs the FOC Control for the Motor 1 */
    application_run((MOTOR_Handle)&motorVars_M1);

    HAL_setGPIOLow((uint32_t)MTR1_FOC_CTRL_ISR_TIMING_PORT,MTR1_FOC_CTRL_ISR_TIMING_PIN);

    systemVars.timerCntFOCCtrlISR++;

}

#ifdef DUAL_MOTOR_CONFIG
/* ADC Trigger Interrupt to execute the FOC state machine*/
void motor2CtrlISR(void)
{
    // Acknowledge ADC interrupt
    HAL_ackMtrADCInt(&motorVars_M2.halInterface->halFOCConfigHandle->intCfg);

    GPIO_writePin(ISR_TIMING, 1);
    /* Runs the FOC Control for the Motor 2 */
    application_full_run((MOTOR_Handle)&motorVars_M2);

    GPIO_writePin(ISR_TIMING, 0);
}
#endif
