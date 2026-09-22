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
// include the related header files
//
#include "user.h"
#include "sys_settings.h"
#include "main.h"

/* SDK includes */
#include "device.h"
#include "log.h"
#include "hal.h"

/* Linker symbols for motor control variables section */
extern uint32_t __motor_ctrl_vars_start__;
extern uint32_t __motor_ctrl_vars_end__;
extern uint32_t __motor_ctrl_vars_size__;

/*
 * System variables - placed in motor_ctrl_vars section for zeroing at startup
 */
//__attribute__((section(".motor_ctrl_vars")))
SYSTEM_Vars_t systemVars;


//! Motor handle array for multi-motor support (defined in sys_main.h)
//! For dual motor configuration, both motors are initialized here
//MOTOR_Handle motorHandles[NUM_MOTORS] ;

// **************************************************************************
// the functions
// !!! Please make sure that you had gone through the user guide, and follow the
// !!! guide to set up the kit and load the right code
int main(void)
{

    // Clear memory for motor control variables section at startup
    // Variables with __attribute__((section(".motor_ctrl_vars"))) are zeroed here
//    HAL_clearDataRAM((void *)&__motor_ctrl_vars_start__, (uint32_t)((uint32_t)&__motor_ctrl_vars_size__));

    Device_Init();
    SYSCFG_DL_init();

    HAL_disableGlobalInts();

    // initialize the Peripherals
    halHandle = HAL_init(&hal, sizeof(hal));

    // set the driver parameters
    HAL_setParams(halHandle);

    // Initialize the Handles for Motor Variables and Parameters
    initMotor1Handles((MOTOR_Handle)&motorVars_M1);

    // Initialize motor control parameters (PI controllers, trajectories, etc.)
    initMotor1CtrlParameters((MOTOR_Handle)&motorVars_M1);

    // Initialize motor control system (ISR, interrupts, PWM, gate driver, parameters, offset cal)
    initMotorControlSystem((MOTOR_Handle)&motorVars_M1, &motor1CtrlISR);

    // Initialize DAC outputs for real-time debugging
    initDACOutputs(&systemVars, (MOTOR_Handle)&motorVars_M1);

    systemVars.flagEnableSystem = true;

    // Enable global interrupts (PRIMASK)
    HAL_enableGlobalInts(halHandle);

    systemVars.powerRelayWaitTime_ms = POWER_RELAY_WAIT_TIME_ms;


    // Waiting for enable system flag to be set
//    while(systemVars.flagEnableSystem == false)
//    {
//        if(HAL_getCPUTimerStatus(halHandle, HAL_CPU_TIMER0))
//        {
//            HAL_clearCPUTimerFlag(halHandle, HAL_CPU_TIMER0);
//
//            systemVars.timerBase_1ms++;
//
//            if(systemVars.timerBase_1ms > systemVars.powerRelayWaitTime_ms)
//            {
                systemVars.flagEnableSystem = true;
//                systemVars.timerBase_1ms = 0;
//            }
//        }
//    }

    systemVars.flagInitializeDone = true;

    while(systemVars.flagEnableSystem == true)
    {
        if(systemVars.timerCntFOCCtrlISR > LOW_PRIORITY_WAIT_TIME_1ms)
        {
            systemVars.timerCntFOCCtrlISR = 0;

            FOC_runLowPriorityStateMachine(&motorVars_M1);
        }
#ifdef DUAL_MOTOR_CONFIG
        else
        {
            FOC_runLowPriorityStateMachine(&motorVars_M2);
        }
#endif
    } // end of while() loop

    return 0;
} // end of main() function
