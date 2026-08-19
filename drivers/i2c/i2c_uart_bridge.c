/*
 * Copyright (c) 2025
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#define DT_DRV_COMPAT uart_i2c_bridge

#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

LOG_MODULE_REGISTER(i2c_uart_bridge, CONFIG_I2C_LOG_LEVEL);


#define BRIDGE_START_BYTE    0xF8U
#define BRIDGE_WRITE_CMD     0xFBU
#define BRIDGE_READ_CMD      0xFAU
#define BRIDGE_HEADER_LEN    3U    /* addr + cmd + len (after start byte) */
#define BRIDGE_MAX_PAYLOAD   16U

/* Total TX buf: start(1) + header(3) + max payload(16) = 20 */
#define BRIDGE_TX_BUF_SIZE   (1U + BRIDGE_HEADER_LEN + BRIDGE_MAX_PAYLOAD)

struct bridge_config {
	const struct device *uart_dev;
	uint32_t response_timeout_ms;
};

struct bridge_data {
	struct k_mutex lock;
	struct k_sem   rx_sem;
	uint8_t        rx_buf[BRIDGE_MAX_PAYLOAD];
	uint8_t        rx_count;
	uint8_t        rx_expected;
};

/* UART interrupt callback — collects RX bytes into rx_buf */
static void uart_cb(const struct device *uart_dev, void *user_data)
{
	const struct device *dev = user_data;
	struct bridge_data *data = dev->data;

	if (!uart_irq_update(uart_dev) || !uart_irq_rx_ready(uart_dev)) {
		return;
	}

	uint8_t byte;

	while (uart_fifo_read(uart_dev, &byte, 1) == 1) {
		if (data->rx_count < data->rx_expected) {
			data->rx_buf[data->rx_count++] = byte;
			if (data->rx_count == data->rx_expected) {
				k_sem_give(&data->rx_sem);
			}
		}

	}
}

static void send_bytes(const struct device *dev,
		       const uint8_t *buf, size_t len)
{
	const struct bridge_config *cfg = dev->config;

	for (size_t i = 0; i < len; i++) {
		uart_poll_out(cfg->uart_dev, buf[i]);
	}
}

static int bridge_transfer(const struct device *dev,
			   struct i2c_msg *msgs, uint8_t num_msgs,
			   uint16_t addr)
{
	const struct bridge_config *cfg = dev->config;
	struct bridge_data *data = dev->data;
	uint8_t pkt[BRIDGE_TX_BUF_SIZE];
	int ret = 0;

	if (k_mutex_lock(&data->lock, K_MSEC(cfg->response_timeout_ms))) {
		return -EBUSY;
	}

	if (num_msgs == 2 &&
	    !(msgs[0].flags & I2C_MSG_READ) &&
	     (msgs[1].flags & I2C_MSG_READ)) {

		if (msgs[0].len != 1) {

			LOG_ERR("read: write len %u != 1 (unsupported)",
				msgs[0].len);
			ret = -ENOTSUP;
			goto out;
		}

		if (msgs[1].len == 0 || msgs[1].len > BRIDGE_MAX_PAYLOAD) {
			LOG_ERR("read: invalid read length %u", msgs[1].len);
			ret = -EINVAL;
			goto out;
		}
		
		pkt[0] = BRIDGE_START_BYTE;
		pkt[1] = (uint8_t)addr;
		pkt[2] = BRIDGE_READ_CMD;
		pkt[3] = (uint8_t)msgs[1].len;
		pkt[4] = msgs[0].buf[0];

		data->rx_count    = 0;
		data->rx_expected = (uint8_t)msgs[1].len;
		k_sem_reset(&data->rx_sem);

		send_bytes(dev, pkt, 5);

		if (k_sem_take(&data->rx_sem,
			       K_MSEC(cfg->response_timeout_ms)) != 0) {
			LOG_ERR("timeout waiting for %u byte(s) from bridge",
				msgs[1].len);
			data->rx_expected = 0;
			ret = -EIO;
			goto out;
		}

		memcpy(msgs[1].buf, data->rx_buf, msgs[1].len);
		goto out;
	}

	if (num_msgs == 1 && !(msgs[0].flags & I2C_MSG_READ)) {
		if (msgs[0].len == 0 || msgs[0].len > BRIDGE_MAX_PAYLOAD) {
			LOG_ERR("write: invalid length %u", msgs[0].len);
			ret = -EINVAL;
			goto out;
		}

		pkt[0] = BRIDGE_START_BYTE;
		pkt[1] = (uint8_t)addr;
		pkt[2] = BRIDGE_WRITE_CMD;
		pkt[3] = (uint8_t)msgs[0].len;
		memcpy(&pkt[4], msgs[0].buf, msgs[0].len);

		send_bytes(dev, pkt, 4 + msgs[0].len);
		goto out;
	}

	LOG_ERR("unsupported message pattern: num_msgs=%u flags=0x%02x",
		num_msgs, msgs[0].flags);
	ret = -ENOTSUP;

out:
	k_mutex_unlock(&data->lock);
	return ret;
}

static int bridge_init(const struct device *dev)
{
	const struct bridge_config *cfg = dev->config;
	struct bridge_data *data = dev->data;

	if (!device_is_ready(cfg->uart_dev)) {
		LOG_ERR("%s: UART device not ready", dev->name);
		return -ENODEV;
	}

	k_mutex_init(&data->lock);
	k_sem_init(&data->rx_sem, 0, 1);

	uart_irq_callback_user_data_set(cfg->uart_dev, uart_cb, (void *)dev);
	uart_irq_rx_enable(cfg->uart_dev);

	return 0;
}

static DEVICE_API(i2c, bridge_api) = {
	.transfer = bridge_transfer,
};

#define BRIDGE_DEVICE_INIT(n)                                              \
	static struct bridge_data bridge_data_##n;                         \
	static const struct bridge_config bridge_cfg_##n = {               \
		.uart_dev = DEVICE_DT_GET(DT_INST_PHANDLE(n, uart_dev)),   \
		.response_timeout_ms =                                     \
			DT_INST_PROP(n, response_timeout_ms),              \
	};                                                                 \
	I2C_DEVICE_DT_INST_DEFINE(n,                                       \
				  bridge_init, NULL,                       \
				  &bridge_data_##n, &bridge_cfg_##n,       \
				  POST_KERNEL, CONFIG_I2C_INIT_PRIORITY,   \
				  &bridge_api);

DT_INST_FOREACH_STATUS_OKAY(BRIDGE_DEVICE_INIT)
