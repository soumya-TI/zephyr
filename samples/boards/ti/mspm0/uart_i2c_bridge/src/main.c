/*
 * Copyright (c) 2025
 * SPDX-License-Identifier: Apache-2.0
 *
 * uart_i2c_bridge protocol validation test.
 *
 * Exercises the driver with all scenarios relevant from the SHared_i2c
 * bridge firmware test suite. Run bridge_test.py on the PC to validate
 * the exact UART packet sequence.
 *
 * Scenario coverage:
 *   Write  : lengths 1, 2, 3, 8, 9, 16 (≤FIFO and >FIFO boundaries)
 *   Read   : lengths 1, 2, 8, 9, 16
 *   Errors : zero length, >16 length → -EINVAL (no packet sent)
 *   Errors : write_len>1 in write-then-read → -ENOTSUP (no packet sent)
 *   Sequential: W→R→W, R→R, W→W (state machine correctness)
 *   Boundary addresses: 0x08 (min), 0x7F (max)
 *   Timeout: Python delays → -EIO → driver recovers (next op succeeds)
 *   Rapid: 3 back-to-back writes with no sleep
 *
 * LED (lp_mspm0g3519):
 *   Blue solid   = all tests passed (expected pass ops returned 0)
 *   Red  solid   = unexpected failure
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>

/* UART-I2C bridge bus */
#define BRIDGE_NODE  DT_NODELABEL(uart_i2c0)

static const struct device *const bus = DEVICE_DT_GET(BRIDGE_NODE);

/* Specs for each I2C address under test */
static const struct i2c_dt_spec S50 = { .bus = DEVICE_DT_GET(BRIDGE_NODE), .addr = 0x50 };
static const struct i2c_dt_spec S08 = { .bus = DEVICE_DT_GET(BRIDGE_NODE), .addr = 0x08 };
static const struct i2c_dt_spec S7F = { .bus = DEVICE_DT_GET(BRIDGE_NODE), .addr = 0x7F };

static const struct gpio_dt_spec led_pass = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec led_fail = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);

static int errors;   /* unexpected failures */

/* ── helpers ─────────────────────────────────────────────────────────────── */

static void pass_check(int ret)
{
	if (ret != 0) {
		errors++;
	}
}

/* Expected to return a specific non-zero code; counts as error if different. */
static void err_check(int ret, int expected)
{
	if (ret != expected) {
		errors++;
	}
}

/*
 * Write `data_len` bytes starting at reg.
 * Packet: [0xF8][addr][0xFB][data_len][reg][data[1]..data[data_len-1]]
 */
static void do_write(const struct i2c_dt_spec *spec,
		     uint8_t reg, const uint8_t *extra, uint8_t extra_len)
{
	uint8_t buf[17]; /* 1 reg byte + up to 16 data bytes */

	buf[0] = reg;
	if (extra && extra_len) {
		memcpy(&buf[1], extra, extra_len);
	}
	pass_check(i2c_write_dt(spec, buf, 1 + extra_len));
	k_sleep(K_MSEC(50));
}

/*
 * Read `read_len` bytes from reg.
 * Packet: [0xF8][addr][0xFA][read_len][reg]
 */
static void do_read(const struct i2c_dt_spec *spec,
		    uint8_t reg, uint8_t read_len)
{
	uint8_t buf[16];

	pass_check(i2c_write_read_dt(spec, &reg, 1, buf, read_len));
	k_sleep(K_MSEC(50));
}

/* Generate a data pattern of `len` bytes: 0x01, 0x02, ..., 0x(len) */
static void fill_pattern(uint8_t *buf, uint8_t len)
{
	for (uint8_t i = 0; i < len; i++) {
		buf[i] = i + 1;
	}
}

/* ── main ─────────────────────────────────────────────────────────────────── */

int main(void)
{
	uint8_t extra[16];

	gpio_pin_configure_dt(&led_pass, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led_fail, GPIO_OUTPUT_INACTIVE);

	if (!device_is_ready(bus)) {
		gpio_pin_set_dt(&led_fail, 1);
		return -ENODEV;
	}

	/* ── Write length variants (TC1, TC3) ──────────────────────────── */

	/* S1  len=1: reg only, no extra data */
	do_write(&S50, 0x00, NULL, 0);

	/* S2  len=2: reg + 1 byte */
	extra[0] = 0x20;
	do_write(&S50, 0x00, extra, 1);

	/* S3  len=3: reg + 2 bytes (PWM target pattern) */
	extra[0] = 0x80; extra[1] = 0x00;
	do_write(&S50, 0x40, extra, 2);

	/* S4  len=8: ≤ hardware FIFO size */
	fill_pattern(extra, 7);
	do_write(&S50, 0x00, extra, 7);

	/* S5  len=9: > hardware FIFO size (crosses boundary) */
	fill_pattern(extra, 8);
	do_write(&S50, 0x00, extra, 8);

	/* S6  len=16: maximum payload */
	fill_pattern(extra, 15);
	do_write(&S50, 0x00, extra, 15);

	/* ── Error cases: no packet should be sent ──────────────────────── */

	/* S7  len=0 write → -EINVAL, no packet */
	{
		uint8_t empty[] = {};
		err_check(i2c_write_dt(&S50, empty, 0), -EINVAL);
	}

	/* S8  len=17 write → -EINVAL, no packet */
	{
		uint8_t big[17];
		fill_pattern(big, 17);
		err_check(i2c_write_dt(&S50, big, 17), -EINVAL);
	}

	/* S9  write_len>1 in write-then-read → -ENOTSUP, no packet */
	{
		uint8_t w[2] = {0x00, 0x01};
		uint8_t r[1];
		err_check(i2c_write_read_dt(&S50, w, 2, r, 1), -ENOTSUP);
	}

	k_sleep(K_MSEC(50)); /* gap so Python sees silence after error cases */

	/* ── Read length variants (TC2) ─────────────────────────────────── */

	/* S10 len=1 */
	do_read(&S50, 0x00, 1);

	/* S11 len=2 */
	do_read(&S50, 0x20, 2);

	/* S12 len=8: ≤ FIFO size */
	do_read(&S50, 0x10, 8);

	/* S13 len=9: > FIFO size */
	do_read(&S50, 0x10, 9);

	/* S14 len=16: maximum */
	do_read(&S50, 0x3F, 16);

	/* ── Sequential patterns (TC9) ───────────────────────────────────── */

	/* S15 W→R→W */
	extra[0] = 0x42;
	do_write(&S50, 0x00, extra, 1);
	do_read(&S50, 0x00, 1);
	extra[0] = 0x43;
	do_write(&S50, 0x00, extra, 1);

	/* S16 R→R */
	do_read(&S50, 0x02, 1);
	do_read(&S50, 0x08, 1);

	/* S17 W→W */
	extra[0] = 0x01;
	do_write(&S50, 0x00, extra, 1);
	extra[0] = 0x02;
	do_write(&S50, 0x00, extra, 1);

	/* ── Boundary addresses (TC13 from test matrix) ─────────────────── */

	/* S18 addr=0x08 write */
	extra[0] = 0xAA;
	do_write(&S08, 0x00, extra, 1);

	/* S19 addr=0x08 read */
	do_read(&S08, 0x00, 1);

	/* S20 addr=0x7F write */
	extra[0] = 0xBB;
	do_write(&S7F, 0x00, extra, 1);

	/* S21 addr=0x7F read */
	do_read(&S7F, 0x00, 1);

	/* ── Timeout + recovery (TC11) ───────────────────────────────────── */

	/*
	 * S22 timeout: reg=0xFE is the agreed marker.
	 * bridge_test.py deliberately delays >200ms before responding.
	 * Driver returns -EIO; NOT counted as an unexpected error.
	 */
	{
		uint8_t marker = 0xFE;
		uint8_t dummy[1];
		int ret = i2c_write_read_dt(&S50, &marker, 1, dummy, 1);

		if (ret != -EIO) {
			errors++; /* expected -EIO, got something else */
		}
	}
	k_sleep(K_MSEC(100));

	/* S23 recovery: normal read must succeed after the timeout */
	do_read(&S50, 0x00, 1);

	/* ── Rapid back-to-back (TC12) ───────────────────────────────────── */

	/*
	 * S24-S26: three writes with NO sleep between them.
	 * Tests mutex serialization at high rate.
	 */
	{
		uint8_t b1[2] = {0x00, 0x11};
		uint8_t b2[2] = {0x00, 0x22};
		uint8_t b3[2] = {0x00, 0x33};
		pass_check(i2c_write_dt(&S50, b1, 2));
		pass_check(i2c_write_dt(&S50, b2, 2));
		pass_check(i2c_write_dt(&S50, b3, 2));
	}
	k_sleep(K_MSEC(50));

	/* ── Result ──────────────────────────────────────────────────────── */

	if (errors == 0) {
		gpio_pin_set_dt(&led_pass, 1);
	} else {
		gpio_pin_set_dt(&led_fail, 1);
	}

	while (1) {
		k_sleep(K_FOREVER);
	}

	return 0;
}
