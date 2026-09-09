/*
 * Copyright (c) 2026 Texas Instruments
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Minimal GT911 touch verification. No display/LVGL: it reports whether the
 * GT911 probed on I2C, then logs every input event. Touch the panel and watch
 * for ABS_X / ABS_Y / BTN_TOUCH lines.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/input/input.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(gt911_test, LOG_LEVEL_INF);

#define TOUCH_NODE DT_NODELABEL(gt911_touch)

static void touch_cb(struct input_event *evt, void *user_data)
{
	ARG_UNUSED(user_data);

	const char *name = "?";

	switch (evt->code) {
	case INPUT_ABS_X:
		name = "ABS_X";
		break;
	case INPUT_ABS_Y:
		name = "ABS_Y";
		break;
	case INPUT_BTN_TOUCH:
		name = "BTN_TOUCH";
		break;
	default:
		break;
	}

	LOG_INF("touch event: %-9s (type=%u code=%u) value=%d sync=%u",
		name, evt->type, evt->code, evt->value, evt->sync);
}
INPUT_CALLBACK_DEFINE(NULL, touch_cb, NULL);

int main(void)
{
#if DT_NODE_HAS_STATUS(TOUCH_NODE, okay)
	const struct device *touch = DEVICE_DT_GET(TOUCH_NODE);

	if (!device_is_ready(touch)) {
		LOG_ERR("GT911 NOT ready: probe failed (see gt911 init errors above)");
		LOG_ERR("-> check TP_SDA/PA0, TP_SCL/PA1, TP_INT/PA13, TP_RST/PB14, 5V, GND");
		return 0;
	}

	LOG_INF("GT911 ready (%s). Touch the panel - events should print below.",
		touch->name);
#else
	LOG_ERR("gt911_touch DT node not enabled - is the shield applied?");
#endif
	return 0;
}
