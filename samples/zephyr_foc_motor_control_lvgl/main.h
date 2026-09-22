#ifndef MAIN_H
#define MAIN_H

#include "math.h"

#include "motor_common.h"
#include "application.h"

#include "datalogIF.h"
#include "cpu_time.h"

#define LED_BLINK_FREQ_Hz           (0.5f)       // 1Hz
#define POWER_RELAY_WAIT_TIME_ms    (1000)       // 1s

#define LOW_PRIORITY_WAIT_TIME_1ms  (uint16_t)(1000.0/USER_PWM_FREQ_kHz)       // 1s

extern CPU_TIME_Obj     cpuTime;
extern CPU_TIME_Handle  cpuTimeHandle;

#endif