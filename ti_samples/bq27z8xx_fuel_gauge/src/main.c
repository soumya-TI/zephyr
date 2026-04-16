/*
 * Copyright (c) 2026 Texas Instruments Incorporated
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/fuel_gauge.h>
#include "bq27z8xx_common.h"

#define STACKSIZE 2048
#define PRIORITY  7

const struct device *const fuel_gauge = DEVICE_DT_GET(DT_ALIAS(fuel_gauge0));
K_MUTEX_DEFINE(i2c_mutex);
K_MUTEX_DEFINE(serial_mutex);

void read_fuel_gauge_current(void)
{
	union fuel_gauge_prop_val prop;

	while (1) {
		k_mutex_lock(&i2c_mutex, K_FOREVER);
		int rc = fuel_gauge_get_prop(fuel_gauge, FUEL_GAUGE_AVG_CURRENT, &prop);

		k_mutex_unlock(&i2c_mutex);

		k_mutex_lock(&serial_mutex, K_FOREVER);
		if (rc) {
			printk("Error reading current: %d\n", rc);
		} else {
			printk("Current reading: %d uA\n", prop.avg_current);
		}
		k_mutex_unlock(&serial_mutex);

		k_msleep(1000);
	}
}

void read_fuel_gauge_voltage(void)
{
	union fuel_gauge_prop_val prop;

	while (1) {
		k_mutex_lock(&i2c_mutex, K_FOREVER);
		int rc = fuel_gauge_get_prop(fuel_gauge, FUEL_GAUGE_VOLTAGE, &prop);

		k_mutex_unlock(&i2c_mutex);

		k_mutex_lock(&serial_mutex, K_FOREVER);
		if (rc) {
			printk("Error reading voltage: %d\n", rc);
		} else {
			printk("Voltage reading: %d uV\n", prop.voltage);
		}
		k_mutex_unlock(&serial_mutex);

		k_msleep(1000);
	}
}

void read_fuel_gauge_status(void)
{
	union fuel_gauge_prop_val prop;

	while (1) {
		k_mutex_lock(&i2c_mutex, K_FOREVER);
		int rc = fuel_gauge_get_prop(fuel_gauge, FUEL_GAUGE_STATUS, &prop);

		k_mutex_unlock(&i2c_mutex);

		k_mutex_lock(&serial_mutex, K_FOREVER);
		if (rc) {
			printk("Error reading status: %d\n", rc);
		} else {
			printk("Status: 0x%04x\n", prop.fg_status);
		}
		k_mutex_unlock(&serial_mutex);

		k_msleep(1000);
	}
}

K_THREAD_DEFINE(read_fuel_gauge_current_id, STACKSIZE, read_fuel_gauge_current, NULL, NULL, NULL,
		PRIORITY, 0, 0);
K_THREAD_DEFINE(read_fuel_gauge_voltage_id, STACKSIZE, read_fuel_gauge_voltage, NULL, NULL, NULL,
		PRIORITY, 0, 0);
K_THREAD_DEFINE(read_fuel_gauge_status_id, STACKSIZE, read_fuel_gauge_status, NULL, NULL, NULL,
		PRIORITY, 0, 0);

int main(void)
{
	const struct device *const i2c_bus = DEVICE_DT_GET(DT_BUS(DT_ALIAS(fuel_gauge0)));
	uint8_t first_addr = 0;

	if (!device_is_ready(fuel_gauge)) {
		printk("Fuel gauge not ready (device detection failed during init)\n");
		return -ENODEV;
	}

	k_mutex_lock(&i2c_mutex, K_FOREVER);

	k_mutex_lock(&serial_mutex, K_FOREVER);
	printk("Scanning I2C bus...\n");
	k_mutex_unlock(&serial_mutex);

	for (uint8_t addr = 0x08; addr <= 0x77; addr++) {
		if (i2c_write(i2c_bus, NULL, 0, addr) == 0) {
			k_mutex_lock(&serial_mutex, K_FOREVER);
			printk("  Device found at address 0x%02x\n", addr);
			k_mutex_unlock(&serial_mutex);
			if (first_addr == 0) {
				first_addr = addr;
			}
		}
	}

	k_mutex_lock(&serial_mutex, K_FOREVER);
	if (first_addr == 0) {
		printk("  No devices found\n");
	}
	printk("Scan complete. Starting fuel gauge reads...\n");
	k_mutex_unlock(&serial_mutex);

	if (first_addr != 0 && first_addr != 0x55) {
		uint8_t cmd = 0x08;
		int rc = i2c_write(i2c_bus, &cmd, sizeof(cmd), first_addr);

		k_mutex_lock(&serial_mutex, K_FOREVER);
		if (rc) {
			printk("Failed to send SMBus command 0x08 to 0x%02x: %d\n", first_addr, rc);
		} else {
			printk("Sent SMBus command 0x08 to 0x%02x\n", first_addr);
		}
		k_mutex_unlock(&serial_mutex);
	}

	struct bq27z8xx_device_type dev_type;
	struct bq27z8xx_firmware_version fw_ver;
	struct bq27z8xx_hardware_version hw_ver;
	int rc;

	rc = fuel_gauge_get_buffer_prop(fuel_gauge, BQ27Z8XX_BUFFER_PROP_DEVICETYPE, &dev_type,
					sizeof(dev_type));
	k_mutex_lock(&serial_mutex, K_FOREVER);
	if (rc) {
		printk("Error reading device type: %d\n", rc);
	} else {
		const char *dev_name;

		switch (dev_type.device_type) {
		case BQ27Z8XX_DEVICE_TYPE_BQ27Z758:
			dev_name = "BQ27Z758";
			break;
		case BQ27Z8XX_DEVICE_TYPE_BQ27Z855:
			dev_name = "BQ27Z855";
			break;
		default:
			dev_name = "Unknown";
			break;
		}
		printk("Device type: %s (0x%04x)\n", dev_name, dev_type.device_type);
	}
	k_mutex_unlock(&serial_mutex);

	rc = fuel_gauge_get_buffer_prop(fuel_gauge, BQ27Z8XX_BUFFER_PROP_FIRMWAREVERSION, &fw_ver,
					sizeof(fw_ver));
	k_mutex_lock(&serial_mutex, K_FOREVER);
	if (rc) {
		printk("Error reading firmware version: %d\n", rc);
	} else {
		printk("Firmware: device=%d version=%d.%d build=%d type=%d dzt=%d\n",
		       fw_ver.device_number, fw_ver.version >> 8, fw_ver.version & 0xFF,
		       fw_ver.build_number, fw_ver.firmware_type, fw_ver.dynamic_z_track_version);
	}
	k_mutex_unlock(&serial_mutex);

	rc = fuel_gauge_get_buffer_prop(fuel_gauge, BQ27Z8XX_BUFFER_PROP_HARDWAREVERSION, &hw_ver,
					sizeof(hw_ver));
	k_mutex_lock(&serial_mutex, K_FOREVER);
	if (rc) {
		printk("Error reading hardware version: %d\n", rc);
	} else {
		printk("Hardware version: 0x%04x\n", hw_ver.hardware_version);
	}
	k_mutex_unlock(&serial_mutex);

	struct bq27z8xx_chem_id chem_id;

	rc = fuel_gauge_get_buffer_prop(fuel_gauge, BQ27Z8XX_BUFFER_PROP_CHEMID, &chem_id,
					sizeof(chem_id));
	k_mutex_lock(&serial_mutex, K_FOREVER);
	if (rc) {
		printk("Error reading chem ID: %d\n", rc);
	} else {
		printk("Chem ID: 0x%04x\n", chem_id.chem_id);
	}
	k_mutex_unlock(&serial_mutex);

	k_mutex_unlock(&i2c_mutex);

	return 0;
}
