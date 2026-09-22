/*
 * Copyright (c) 2026 Texas Instruments Incorporated
 * SPDX-License-Identifier: Apache-2.0
 *
 * FOC motor control + LVGL motor control UI — integrated application.
 *
 * Thread layout
 * -------------
 *  Priority -1 : Shell (waits for UART input — motor start/stop/set speed)
 *  Priority  0 : do_foc (ISR-driven FOC math + PWM update, ~20 kHz)
 *  Priority  0 : main() (FOC low-priority state machine, ~1 ms)
 *  Priority  5 : lvgl_thread (lv_timer_handler loop, wakes ~100 ms)
 *  Priority 10 : log backend (deferred LOG_INF drain, lowest of all)
 */

#include "user.h"
#include "sys_settings.h"
#include "main.h"

#include "device.h"
#include "log.h"
#include "hal.h"

#include <zephyr/arch/arch_interface.h>
#include <zephyr/kernel.h>
#include <zephyr/irq.h>
#include <zephyr/shell/shell.h>
#include <zephyr/drivers/display.h>
#include <lvgl.h>

#include "motor_control.h"
#include "ti_sdk_dl_config.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

/* ---------------------------------------------------------------------------
 * FOC globals (extern'd by motor library and shell commands)
 * ---------------------------------------------------------------------------*/
SYSTEM_Vars_t systemVars;

/* cmd_motor.c references these for its UART data-streaming plot command.
 * application.c references tx_complete to gate data transmission.
 * uart_transfer.c is not included in this app, so stub them out here. */
bool data_stream = false;
bool data_bin    = false;
bool tx_complete = true;

extern void ADC0_INT1_IRQHandler(void);

/* ---------------------------------------------------------------------------
 * Semaphores
 * ---------------------------------------------------------------------------*/
K_SEM_DEFINE(do_foc_sem, 0, 1);

/* ---------------------------------------------------------------------------
 * do_foc thread — priority 0, ISR-triggered (~20 kHz)
 *
 * Blocks on do_foc_sem. The ADC ISR gives the semaphore once per PWM cycle.
 * application_run() does the full FOC math chain and writes new PWM duties.
 * ---------------------------------------------------------------------------*/
#define FOC_THREAD_STACK_SIZE  4096
K_THREAD_STACK_DEFINE(do_foc_stack, FOC_THREAD_STACK_SIZE);
static struct k_thread foc_thread;

static void do_foc(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    printk("Entered do foc loop\n");

    while (1) {
        k_sem_take(&do_foc_sem, K_FOREVER);

        HAL_setGPIOHigh((uint32_t)MTR1_FOC_CTRL_ISR_TIMING_PORT,
                        MTR1_FOC_CTRL_ISR_TIMING_PIN);

        application_run((MOTOR_Handle)&motorVars_M1);

        HAL_setGPIOLow((uint32_t)MTR1_FOC_CTRL_ISR_TIMING_PORT,
                       MTR1_FOC_CTRL_ISR_TIMING_PIN);
    }
}

/* ---------------------------------------------------------------------------
 * LVGL thread — priority 5, sleeps between lv_timer_handler() calls
 *
 * Initialises the LVGL motor-control UI, then pumps lv_timer_handler().
 * The bridge timer (inside lv_timer_handler) fires every 100 ms and
 * synchronises motorVars_M1 ↔ LVGL subjects in both directions.
 * ---------------------------------------------------------------------------*/
#define LVGL_THREAD_STACK_SIZE 16384
K_THREAD_STACK_DEFINE(lvgl_stack, LVGL_THREAD_STACK_SIZE);
static struct k_thread lvgl_thread_data;

static void lvgl_thread(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    const struct device *display_dev =
        DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    if (!device_is_ready(display_dev)) {
        printk("Display device not ready — LVGL will not run\n");
        return;
    }

    /* Build every subject, register every observer, load the run screen. */
    motor_control_init("A:");

    printk("LVGL UI init OK — entering render loop\n");

    /* Render the first frame before turning the display on. */
    lv_timer_handler();

    int ret = display_blanking_off(display_dev);
    if (ret < 0 && ret != -ENOSYS) {
        /* -ENOSYS is expected for the dummy display backend. Any other error
         * is real and worth logging, but not fatal for verification. */
        printk("display_blanking_off returned %d\n", ret);
    }

    while (1) {
        uint32_t sleep_ms = lv_timer_handler();

        if (sleep_ms == LV_NO_TIMER_READY) {
            sleep_ms = LV_DEF_REFR_PERIOD;
        }
        k_sleep(K_MSEC(sleep_ms));
    }
}

/* ---------------------------------------------------------------------------
 * main() — FOC low-priority state machine, priority 0, ~1 ms cadence
 *
 * FOC_runLowPriorityStateMachine() handles fault checking, start/stop
 * sequencing, RMS calculation, protection monitors — everything that is
 * correct at 1 ms but does not need the 50 µs ISR cadence.
 * ---------------------------------------------------------------------------*/
int main(void)
{
    /* Enable TMU and FPU (Cortex-M33 CPACR: co-processors 0 and 10/11). */
    SCB->CPACR = 0x00300003;

    SYSCFG_DL_init();

    halHandle = HAL_init(&hal, sizeof(hal));
    HAL_setParams(halHandle);

    initMotor1Handles((MOTOR_Handle)&motorVars_M1);
    initMotor1CtrlParameters((MOTOR_Handle)&motorVars_M1);
    initMotorControlSystem((MOTOR_Handle)&motorVars_M1, &motor1CtrlISR);

    systemVars.flagEnableSystem  = true;
    
    HAL_enableGlobalInts(halHandle);
    
    systemVars.powerRelayWaitTime_ms = POWER_RELAY_WAIT_TIME_ms;

    systemVars.flagEnableSystem  = true;

    systemVars.flagInitializeDone = true;


    initDACOutputs(&systemVars, (MOTOR_Handle)&motorVars_M1);

    IRQ_CONNECT(8, 0, ADC0_INT1_IRQHandler, NULL, 0);
    irq_enable(8);

    /* Auto-start: set the speed reference and enable the drive immediately.
    * The low-priority state machine will sequence through alignment and
     * open-loop start before entering closed-loop control.
     * Default: 60 Hz electrical = 60/4*60 = 900 RPM shaft for a 4-pole-pair
     * motor. Adjust USER_MOTOR1_NUM_POLE_PAIRS in user_mtr1.h if needed. */
    motorVars_M1.speedRef_Hz             = 300.0f;
    motorVars_M1.flagClearFaults         = true;
    motorVars_M1.flagEnableRunAndIdentify = true;


    printk("FOC Motor Control + LVGL starting\n");
    printk("Shell available: 'motor start/stop/set speed <Hz>/clear'\n");

    /* Launch do_foc thread (priority 0, FPU registers saved on context switch). */
    k_tid_t foc_tid = k_thread_create(&foc_thread,
        do_foc_stack, FOC_THREAD_STACK_SIZE,
        do_foc, NULL, NULL, NULL,
        0, K_FP_REGS, K_USEC(50));
    k_thread_name_set(foc_tid, "do_foc");

    /* Launch LVGL thread (priority 5 — well below FOC, well above log). */
    k_tid_t lvgl_tid = k_thread_create(&lvgl_thread_data,
        lvgl_stack, LVGL_THREAD_STACK_SIZE,
        lvgl_thread, NULL, NULL, NULL,
        5, 0, K_NO_WAIT);
    k_thread_name_set(lvgl_tid, "lvgl");

    /* main() becomes the low-priority FOC state machine.
     * Sleeps 1 ms per iteration; runs FOC_runLowPriorityStateMachine()
     * when the ISR counter indicates ~1 ms of ADC activity has elapsed. */
    uint32_t wallClockMs = 0;

    while (systemVars.flagEnableSystem == true) {
        k_sleep(K_MSEC(1));
        wallClockMs++;

        if (systemVars.timerCntFOCCtrlISR > LOW_PRIORITY_WAIT_TIME_1ms) {
            systemVars.timerCntFOCCtrlISR = 0;
            wallClockMs = 0;
            FOC_runLowPriorityStateMachine(&motorVars_M1);
        } else if (wallClockMs >= 10u) {
            wallClockMs = 0;
            FOC_runLowPriorityStateMachine(&motorVars_M1);
        }
    }

    return 0;
}
