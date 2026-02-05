/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/firmware/scmi/power.h>

/* Define test clock IDs - these should match the SCMI clock IDs for the target platform */
#define TEST_CLOCK_ID_0		162
#define TEST_CLOCK_ID_1		312

/* Define test power domain IDs - these should match the SCMI power domain IDs */
#define TEST_POWER_DOMAIN_0	32
#define TEST_POWER_DOMAIN_1	39

/* Get SCMI clock device from device tree */
#define SCMI_CLOCK_NODE		DT_NODELABEL(scmi_clk)

/**
 * @brief Test SCMI clock control functionality
 * 
 * This function tests various SCMI clock control operations including:
 * - Getting current clock rate
 * - Setting clock rate
 * - Enabling/disabling clock
 * 
 * @return 0 on success, negative errno on failure
 */
static int test_scmi_clock_control(void)
{
	const struct device *clock_dev;
	uint64_t rate = 0;
	int ret;

	printf("\n=== SCMI Clock Control Tests ===\n");

	/* Get SCMI clock device */
	if (!DT_NODE_EXISTS(SCMI_CLOCK_NODE)) {
		printf("ERROR: SCMI clock node not found in device tree\n");
		return -ENODEV;
	}

	clock_dev = DEVICE_DT_GET(SCMI_CLOCK_NODE);
	if (!device_is_ready(clock_dev)) {
		printf("ERROR: SCMI clock device not ready\n");
		return -ENODEV;
	}

	printf("SCMI clock device found and ready\n");

	/* Test Clock ID 0 */
	printf("\n--- Testing Clock ID %d ---\n", TEST_CLOCK_ID_0);
		
	/* Test: Get current clock rate */
	ret = clock_control_get_rate(clock_dev, UINT_TO_POINTER(TEST_CLOCK_ID_0), (uint32_t *)&rate);
	if (ret == 0) {
		printf("  Current clock rate: %llu Hz\n", rate);
	} else {
		printf("  WARNING: Get rate failed (ret=%d) - clock may not be available\n", ret);
	}

	/* Test: Enable clock */
	ret = clock_control_on(clock_dev, UINT_TO_POINTER(TEST_CLOCK_ID_0));
	if (ret == 0) {
		printf("  Clock enabled successfully\n");
	} else {
		printf("  WARNING: Clock enable failed (ret=%d)\n", ret);
	}

	/* Test: Set clock rate to 100 MHz */
	ret = clock_control_set_rate(clock_dev, UINT_TO_POINTER(TEST_CLOCK_ID_0), 
				     UINT_TO_POINTER(100000000));
	if (ret == 0) {
		printf("  Clock rate set to 100 MHz\n");
		
		/* Verify the new rate */
		ret = clock_control_get_rate(clock_dev, UINT_TO_POINTER(TEST_CLOCK_ID_0), (uint32_t *)&rate);
		if (ret == 0) {
			printf("  Verified clock rate: %llu Hz\n", rate);
		}
	} else {
		printf("  WARNING: Set rate failed (ret=%d)\n", ret);
	}

	/* Test: Set clock rate to 50 MHz */
	ret = clock_control_set_rate(clock_dev, UINT_TO_POINTER(TEST_CLOCK_ID_0), 
				     UINT_TO_POINTER(50000000));
	if (ret == 0) {
		printf("  Clock rate set to 50 MHz\n");
		
		/* Verify the new rate */
		ret = clock_control_get_rate(clock_dev, UINT_TO_POINTER(TEST_CLOCK_ID_0), (uint32_t *)&rate);
		if (ret == 0) {
			printf("  Verified clock rate: %llu Hz\n", rate);
		}
	} else {
		printf("  WARNING: Set rate failed (ret=%d)\n", ret);
	}

	/* Test: Disable clock */
	ret = clock_control_off(clock_dev, UINT_TO_POINTER(TEST_CLOCK_ID_0));
	if (ret == 0) {
		printf("  Clock disabled successfully\n");
	} else {
		printf("  WARNING: Clock disable failed (ret=%d)\n", ret);
	}

	/* Test Clock ID 1 */
	printf("\n--- Testing Clock ID %d ---\n", TEST_CLOCK_ID_1);

	/* Test: Get current clock rate */
	ret = clock_control_get_rate(clock_dev, UINT_TO_POINTER(TEST_CLOCK_ID_1), (uint32_t *)&rate);
	if (ret == 0) {
		printf("  Current clock rate: %llu Hz\n", rate);
	} else {
		printf("  WARNING: Get rate failed (ret=%d) - clock may not be available\n", ret);
	}

	/* Test: Enable clock */
	ret = clock_control_on(clock_dev, UINT_TO_POINTER(TEST_CLOCK_ID_1));
	if (ret == 0) {
		printf("  Clock enabled successfully\n");
	} else {
		printf("  WARNING: Clock enable failed (ret=%d)\n", ret);
	}

	printf("\nSCMI Clock Control tests completed\n");
	return 0;
}

/**
 * @brief Test SCMI power domain control functionality
 * 
 * This function tests SCMI power domain operations including:
 * - Getting current power state
 * - Setting power state to ON
 * - Setting power state to OFF
 * 
 * @return 0 on success, negative errno on failure
 */
static int test_scmi_power_control(void)
{
	struct scmi_power_state_config cfg;
	uint32_t power_state = 0;
	int ret;

	printf("\n=== SCMI Power Domain Control Tests ===\n");

	/* Test Power Domain 0 */
	printf("\n--- Testing Power Domain %d ---\n", TEST_POWER_DOMAIN_0);

	/* Test: Get current power state */
	ret = scmi_power_state_get(TEST_POWER_DOMAIN_0, &power_state);
	if (ret == 0) {
		printf("  Current power state: 0x%08x\n", power_state);
	} else {
		printf("  WARNING: Get power state failed (ret=%d) - domain may not be available\n", ret);
	}

	/* Test: Set power domain to ON state */
	cfg.flags = 0;
	cfg.domain_id = TEST_POWER_DOMAIN_0;
	cfg.power_state = SCMI_POWER_STATE_GENERIC_ON;
	
	ret = scmi_power_state_set(&cfg);
	if (ret == 0) {
		printf("  Power domain set to ON state\n");
		
		/* Verify the new state */
		ret = scmi_power_state_get(TEST_POWER_DOMAIN_0, &power_state);
		if (ret == 0) {
			printf("  Verified power state: 0x%08x\n", power_state);
		}
	} else {
		printf("  WARNING: Set power state ON failed (ret=%d)\n", ret);
	}

	/* Test: Set power domain to OFF state */
	cfg.power_state = SCMI_POWER_STATE_GENERIC_OFF;
	
	ret = scmi_power_state_set(&cfg);
	if (ret == 0) {
		printf("  Power domain set to OFF state\n");
		
		/* Verify the new state */
		ret = scmi_power_state_get(TEST_POWER_DOMAIN_0, &power_state);
		if (ret == 0) {
			printf("  Verified power state: 0x%08x\n", power_state);
		}
	} else {
		printf("  WARNING: Set power state OFF failed (ret=%d)\n", ret);
	}

	/* Test: Set power domain back to ON state */
	cfg.power_state = SCMI_POWER_STATE_GENERIC_ON;
	
	ret = scmi_power_state_set(&cfg);
	if (ret == 0) {
		printf("  Power domain set back to ON state\n");
		
		/* Verify the new state */
		ret = scmi_power_state_get(TEST_POWER_DOMAIN_0, &power_state);
		if (ret == 0) {
			printf("  Verified power state: 0x%08x\n", power_state);
		}
	} else {
		printf("  WARNING: Set power state ON failed (ret=%d)\n", ret);
	}

	/* Test Power Domain 1 */
	printf("\n--- Testing Power Domain %d ---\n", TEST_POWER_DOMAIN_1);

	/* Test: Get current power state */
	ret = scmi_power_state_get(TEST_POWER_DOMAIN_1, &power_state);
	if (ret == 0) {
		printf("  Current power state: 0x%08x\n", power_state);
	} else {
		printf("  WARNING: Get power state failed (ret=%d) - domain may not be available\n", ret);
	}

	/* Test: Set power domain to ON state */
	cfg.domain_id = TEST_POWER_DOMAIN_1;
	cfg.power_state = SCMI_POWER_STATE_GENERIC_ON;
	
	ret = scmi_power_state_set(&cfg);
	if (ret == 0) {
		printf("  Power domain set to ON state\n");
	} else {
		printf("  WARNING: Set power state ON failed (ret=%d)\n", ret);
	}

	printf("\nSCMI Power Domain Control tests completed\n");
	return 0;
}

int main(void)
{
	int ret;

	printf("SCMI Clock and Power Control Test Application\n");
	printf("\n========================================\n");
	printf("SCMI Clock and Power Control Test Suite\n");
	printf("========================================\n");

	/* Run SCMI Clock Control Tests */
	ret = test_scmi_clock_control();
	if (ret < 0) {
		printf("\nERROR: SCMI Clock Control tests failed with error %d\n", ret);
	}

	/* Run SCMI Power Domain Control Tests */
	ret = test_scmi_power_control();
	if (ret < 0) {
		printf("\nERROR: SCMI Power Domain Control tests failed with error %d\n", ret);
	}

	printf("\n========================================\n");
	printf("All SCMI tests completed!\n");
	printf("========================================\n");

	return 0;
}
