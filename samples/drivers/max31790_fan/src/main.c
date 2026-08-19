/*
 * Copyright (c) 2025
 * SPDX-License-Identifier: Apache-2.0
 *
 * MAX31790 fan controller sample — PWM set, RPM read, fault monitor.
 *
 * Works with any I2C topology exposing a maxim,max31790 node, including
 * the uart-i2c-bridge driver on MSPM0G.
 *
 * PC testing (lp_mspm0g3519):
 *   Console is disabled; uart0 carries the bridge protocol.
 *   Run bridge_test.py on the PC to validate packets and respond to reads.
 *
 * LED feedback:
 *   Blue  (LED0) blinks 1 Hz = running normally
 *   Red   (LED1) solid on    = device init failed
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sensor/max31790.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(max31790_fan, LOG_LEVEL_INF);

#define FAN_PWM_NODE   DT_NODELABEL(fan_pwm)
#define FAN_SPEED_NODE DT_NODELABEL(fan_speed_1)
#define FAN_FAULT_NODE DT_NODELABEL(fan_fault)

#define FAN_PERIOD_NS  40000U   /* 25 kHz */
#define FAN_CHANNEL    1U
#define FAN_DUTY_PCT   50U

static const struct gpio_dt_spec led_run = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

#if DT_HAS_ALIAS(led1)
static const struct gpio_dt_spec led_fail = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
#define LED_FAIL_SET(v) gpio_pin_set_dt(&led_fail, v)
#define LED_FAIL_INIT() gpio_pin_configure_dt(&led_fail, GPIO_OUTPUT_INACTIVE)
#else
/* Board has only one LED — use it for run indicator; failures visible via log */
#define LED_FAIL_SET(v) (void)(v)
#define LED_FAIL_INIT() do {} while (0)
#endif

int main(void)
{
	const struct device *fan_pwm   = DEVICE_DT_GET(FAN_PWM_NODE);
	const struct device *fan_speed = DEVICE_DT_GET(FAN_SPEED_NODE);
	const struct device *fan_fault = DEVICE_DT_GET(FAN_FAULT_NODE);

	gpio_pin_configure_dt(&led_run, GPIO_OUTPUT_INACTIVE);
	LED_FAIL_INIT();

	if (!device_is_ready(fan_pwm) ||
	    !device_is_ready(fan_speed) ||
	    !device_is_ready(fan_fault)) {
		LED_FAIL_SET(1);
		LOG_ERR("MAX31790 device(s) not ready");
		return -ENODEV;
	}

	LOG_INF("MAX31790 ready via UART-I2C bridge");

	uint32_t pulse_ns = FAN_PERIOD_NS * FAN_DUTY_PCT / 100U;
	int ret = pwm_set(fan_pwm, FAN_CHANNEL, FAN_PERIOD_NS, pulse_ns, 0);

	if (ret) {
		LED_FAIL_SET(1);
		LOG_ERR("PWM set failed: %d", ret);
		return ret;
	}

	LOG_INF("Fan ch%u: %u%% duty", FAN_CHANNEL, FAN_DUTY_PCT);

	while (1) {
		gpio_pin_toggle_dt(&led_run);

		struct sensor_value rpm, fault;

		if (sensor_sample_fetch(fan_speed) == 0) {
			sensor_channel_get(fan_speed, SENSOR_CHAN_RPM, &rpm);
			LOG_INF("Fan %u: %d RPM", FAN_CHANNEL, rpm.val1);
		}

		if (sensor_sample_fetch(fan_fault) == 0) {
			sensor_channel_get(fan_fault,
					   SENSOR_CHAN_MAX31790_FAN_FAULT,
					   &fault);
			if (fault.val1) {
				LOG_WRN("Fault: 0x%02x", fault.val1);
			}
		}

		k_sleep(K_SECONDS(2));
	}

	return 0;
}
