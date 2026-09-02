/*
 * Copyright (c) 2026 Texas Instruments
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT ti_mspm33_qspi

#include <zephyr/device.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/clock_control/mspm0_clock_control.h>
#include <zephyr/drivers/mspi.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/irq.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/sys_io.h>
#include <zephyr/sys/util.h>

LOG_MODULE_REGISTER(mspi_mspm33_qspi, CONFIG_MSPI_LOG_LEVEL);

/* ── GPRCM block ─────────────────────────────────────────────────────────── */
#define QSPI_PWREN_OFFSET		0x800u
#define QSPI_RSTCTL_OFFSET		0x804u
#define QSPI_GPRCM_STAT_OFFSET		0x814u

/* PWREN: KEY=0x26<<24, ENABLE=bit0 */
#define QSPI_PWREN_KEY_ENABLE		0x26000001u
/* RSTCTL: KEY=0xB1<<24, RESETSTKYCLR=bit1, RESETASSERT=bit0 */
#define QSPI_RSTCTL_KEY_ASSERT		0xB1000003u
#define QSPI_RSTCTL_KEY_CLEAR		0xB1000002u

/* GPRCM_STAT: bit16 = RESETSTKY (reset sticky, 1 while in reset) */
#define QSPI_GPRCM_STAT_RESETSTKY	BIT(16)

/* ── Clock block ─────────────────────────────────────────────────────────── */
#define QSPI_CLKDIV_OFFSET		0x1000u
#define QSPI_CLKSEL_OFFSET		0x1004u

/* CLKDIV RATIO[2:0]: 0=/1 */
#define QSPI_CLKDIV_RATIO_DIV1		0u

/* ── SPI / frame control ─────────────────────────────────────────────────── */
#define QSPI_CTL0_OFFSET		0x1100u
#define QSPI_CTL1_OFFSET		0x1104u
#define QSPI_CLKCTL_OFFSET		0x1108u
#define QSPI_IFLS_OFFSET		0x110Cu
#define QSPI_RXDATA_OFFSET		0x1130u
#define QSPI_TXDATA_OFFSET		0x1140u

/* CTL1 bits */
#define QSPI_CTL1_ENABLE		BIT(0)
#define QSPI_CTL1_CP			BIT(2)   /* 1 = controller mode */
#define QSPI_CTL1_MSB			BIT(4)   /* 1 = MSB first */

/* ── QSPI command engine ─────────────────────────────────────────────────── */
#define QSPI_QSPICTL0_OFFSET		0x1200u
#define QSPI_QSPICTL1_OFFSET		0x1204u
#define QSPI_QSPITIMING_OFFSET		0x1208u
#define QSPI_QSPIFIFOCTL_OFFSET		0x120Cu
#define QSPI_QSPICSCTL_OFFSET		0x1210u
#define QSPI_QSPISTATUS_OFFSET		0x1234u

/* QSPICTL0 field definitions */
#define QSPI_CTL0_DUMMYCLK_SHIFT	0u
#define QSPI_CTL0_DUMMYCLK_MASK		(0xFu)
#define QSPI_CTL0_CMDBYTE_SHIFT		8u
#define QSPI_CTL0_FORMAT_SHIFT		24u
#define QSPI_CTL0_ADDRMODE_3BYTE	0u
#define QSPI_CTL0_ADDRMODE_4BYTE	BIT(28)
#define QSPI_CTL0_PERFMODE_OFF		0u
#define QSPI_CTL0_MODE_PERIPH		0u   /* bit31=0: indirect/PERIPH mode */

/* QSPICTL0 FORMAT values (placed at bits[27:24]) */
#define QSPI_FORMAT_LEGACY		(0x0u << QSPI_CTL0_FORMAT_SHIFT)
#define QSPI_FORMAT_SPI_HALF		(0x1u << QSPI_CTL0_FORMAT_SHIFT)
#define QSPI_FORMAT_QPI_HALF		(0x2u << QSPI_CTL0_FORMAT_SHIFT)
#define QSPI_FORMAT_SSS			(0x8u << QSPI_CTL0_FORMAT_SHIFT)
#define QSPI_FORMAT_SSD			(0x9u << QSPI_CTL0_FORMAT_SHIFT)
#define QSPI_FORMAT_SDD			(0xAu << QSPI_CTL0_FORMAT_SHIFT)
#define QSPI_FORMAT_SSQ			(0xBu << QSPI_CTL0_FORMAT_SHIFT)
#define QSPI_FORMAT_SQQ			(0xCu << QSPI_CTL0_FORMAT_SHIFT)
#define QSPI_FORMAT_QQQ			(0xDu << QSPI_CTL0_FORMAT_SHIFT)

/* QSPICTL1 bit/field positions */
#define QSPI_CTL1_TXCOUNT_SHIFT		0u
#define QSPI_CTL1_RXCOUNT_SHIFT		16u
#define QSPI_CTL1_PREFETCH		BIT(31)

/* QSPIFIFOCTL flush bits */
#define QSPI_FIFOCTL_RXFLUSH		BIT(0)
#define QSPI_FIFOCTL_TXFLUSH		BIT(1)

/* QSPISTATUS bits */
#define QSPI_STS_RXFIFOE		BIT(0)  /* RX FIFO empty */
#define QSPI_STS_RXFIFONF		BIT(1)  /* RX FIFO not full */
#define QSPI_STS_TXFIFOE		BIT(2)  /* TX FIFO empty */
#define QSPI_STS_TXFIFONF		BIT(3)  /* TX FIFO not full */
#define QSPI_STS_STALL			BIT(6)  /* engine stalled */
#define QSPI_STS_IDLE			BIT(7)  /* bus idle */

/* ── CPU_INT block ───────────────────────────────────────────────────────── */
#define QSPI_INT_IIDX_OFFSET		0x1020u
#define QSPI_INT_IMASK_OFFSET		0x1028u
#define QSPI_INT_RIS_OFFSET		0x1030u
#define QSPI_INT_MIS_OFFSET		0x1038u
#define QSPI_INT_ISET_OFFSET		0x1040u
#define QSPI_INT_ICLR_OFFSET		0x1048u

/* Interrupt mask bits */
#define QSPI_INT_RXFIFO_OVF		BIT(0)
#define QSPI_INT_RTOUT			BIT(2)
#define QSPI_INT_RX			BIT(3)
#define QSPI_INT_TX			BIT(4)
#define QSPI_INT_IDLE			BIT(6)

/* IIDX encoded values (bit-position + 1) for each source */
#define QSPI_IIDX_RXFIFO_OVF		1u
#define QSPI_IIDX_RTOUT			3u
#define QSPI_IIDX_RX			4u
#define QSPI_IIDX_TX			5u
#define QSPI_IIDX_IDLE			7u

/* Handy IMASKs */
#define QSPI_INT_ALL_ACTIVE	(QSPI_INT_RX | QSPI_INT_TX | QSPI_INT_IDLE)

/* ── Register I/O helpers ────────────────────────────────────────────────── */
static inline uint32_t qspi_rd(uintptr_t base, uint32_t off)
{
	return sys_read32(base + off);
}

static inline void qspi_wr(uintptr_t base, uint32_t off, uint32_t val)
{
	sys_write32(val, base + off);
}

/* ── Driver structures ───────────────────────────────────────────────────── */

struct qspi_mspm33_config {
	DEVICE_MMIO_ROM;
	void (*irq_config_fn)(void);
	const struct pinctrl_dev_config *pcfg;
	struct mspm0_sys_clock clock_subsys;
	uint32_t clksel_bit;          /* CLKSEL bit from DT clocks cell */
	struct mspi_cfg mspicfg;
};

struct qspi_mspm33_data {
	DEVICE_MMIO_RAM;
	struct k_sem completion;      /* posted by ISR on IDLE event */
	struct k_mutex bus_lock;      /* serialises concurrent transceive calls */
	struct mspi_dev_cfg dev_cfg;  /* current device-level configuration */
	const struct mspi_dev_id *dev_id;

	/* ISR-shared transfer state (written before enabling irqs) */
	uint8_t *rx_buf;
	uint32_t rx_remaining;
	uint8_t *tx_buf;
	uint32_t tx_remaining;
};

/* ── Private helpers ─────────────────────────────────────────────────────── */

/**
 * io_mode to FORMAT for the data-read engine (FORMAT >= 0x8).
 * Used for RX transfers with address.
 */
static uint32_t qspi_data_format(enum mspi_io_mode io_mode)
{
	switch (io_mode) {
	case MSPI_IO_MODE_DUAL_1_1_2:
		return QSPI_FORMAT_SSD;
	case MSPI_IO_MODE_DUAL_1_2_2:
		return QSPI_FORMAT_SDD;
	case MSPI_IO_MODE_QUAD_1_1_4:
		return QSPI_FORMAT_SSQ;
	case MSPI_IO_MODE_QUAD_1_4_4:
		return QSPI_FORMAT_SQQ;
	case MSPI_IO_MODE_QUAD:
		return QSPI_FORMAT_QQQ;
	case MSPI_IO_MODE_SINGLE:
	default:
		return QSPI_FORMAT_SSS;
	}
}

/**
 * io_mode to half-duplex FORMAT.
 * Used for cmd-only, status-read, and page-program frames.
 */
static uint32_t qspi_half_duplex_format(enum mspi_io_mode io_mode)
{
	switch (io_mode) {
	case MSPI_IO_MODE_QUAD:
	case MSPI_IO_MODE_QUAD_1_1_4:
	case MSPI_IO_MODE_QUAD_1_4_4:
		return QSPI_FORMAT_QPI_HALF;
	default:
		return QSPI_FORMAT_SPI_HALF;
	}
}

/** Push address bytes MSB-first into TX FIFO. */
static void qspi_push_address(uintptr_t base, uint32_t address, uint8_t addr_len)
{
	if (addr_len >= 4) {
		qspi_wr(base, QSPI_TXDATA_OFFSET, (address >> 24) & 0xFFu);
	}
	if (addr_len >= 3) {
		qspi_wr(base, QSPI_TXDATA_OFFSET, (address >> 16) & 0xFFu);
	}
	if (addr_len >= 2) {
		qspi_wr(base, QSPI_TXDATA_OFFSET, (address >> 8) & 0xFFu);
	}
	if (addr_len >= 1) {
		qspi_wr(base, QSPI_TXDATA_OFFSET, address & 0xFFu);
	}
}

/**
 * Setup and start a single QSPI frame.
 *
 * Caller must already hold bus_lock.  The function returns after the
 * completion semaphore is taken (blocking until ISR signals IDLE) or
 * after the timeout expires.
 *
 * Transfer types handled:
 *  1. num_bytes==0 (cmd-only, e.g. WREN)
 *  2. RX, addr_length==0 (RDID/RDSR)
 *  3. TX, addr_length>0  (page program)
 *  4. RX, addr_length>0  (fast/quad read)
 */
static int qspi_do_frame(const struct device *dev, const struct mspi_xfer_packet *pkt,
			 const struct mspi_xfer *xfer)
{
	struct qspi_mspm33_data *data = dev->data;
	uintptr_t base = DEVICE_MMIO_GET(dev);
	uint8_t addr_len = xfer->addr_length;
	uint16_t rx_dummy = xfer->rx_dummy;
	uint32_t ctl0_val;
	uint32_t ctl1_val;
	uint32_t iqmask;
	k_timeout_t timeout;
	int ret = 0;

	enum mspi_io_mode io_mode = data->dev_cfg.io_mode;

	/* ── Step 1: Disable engine ─────────────────────────────── */
	uint32_t ctl1_base = qspi_rd(base, QSPI_CTL1_OFFSET) & ~QSPI_CTL1_ENABLE;

	qspi_wr(base, QSPI_CTL1_OFFSET, ctl1_base);

	/* ── Step 2: Build QSPICTL0 / QSPICTL1 ──────────────────── */

	if (pkt->num_bytes == 0) {
		/* Case 1: cmd-only (e.g. WREN) */
		ctl0_val = qspi_half_duplex_format(io_mode) |
			   (pkt->cmd << QSPI_CTL0_CMDBYTE_SHIFT);
		ctl1_val = (1u << QSPI_CTL1_TXCOUNT_SHIFT);  /* 1 cmd byte */
		data->tx_buf = NULL;
		data->tx_remaining = 0;
		data->rx_buf = NULL;
		data->rx_remaining = 0;
		iqmask = QSPI_INT_IDLE;

	} else if (pkt->dir == MSPI_RX && addr_len == 0) {
		/* Case 2: RX, no address (RDID / RDSR) */
		ctl0_val = qspi_half_duplex_format(io_mode);
		ctl1_val = (1u << QSPI_CTL1_TXCOUNT_SHIFT) |
			   ((uint32_t)pkt->num_bytes << QSPI_CTL1_RXCOUNT_SHIFT) |
			   QSPI_CTL1_PREFETCH;
		data->tx_buf = NULL;
		data->tx_remaining = 0;
		data->rx_buf = pkt->data_buf;
		data->rx_remaining = pkt->num_bytes;
		iqmask = QSPI_INT_RX | QSPI_INT_IDLE;

	} else if (pkt->dir == MSPI_TX && addr_len > 0) {
		/* Case 3: TX with address (page program) */
		uint32_t total_tx = (uint32_t)addr_len + pkt->num_bytes;

		ctl0_val = qspi_half_duplex_format(io_mode) |
			   (addr_len == 4 ? QSPI_CTL0_ADDRMODE_4BYTE : QSPI_CTL0_ADDRMODE_3BYTE);
		ctl1_val = (total_tx << QSPI_CTL1_TXCOUNT_SHIFT);
		data->tx_buf = pkt->data_buf;
		data->tx_remaining = pkt->num_bytes;
		data->rx_buf = NULL;
		data->rx_remaining = 0;
		iqmask = QSPI_INT_TX | QSPI_INT_IDLE;

	} else if (pkt->dir == MSPI_RX && addr_len > 0) {
		/* Case 4: RX with address (fast/quad read) */
		uint32_t rx_cnt = (pkt->num_bytes < 256u) ? pkt->num_bytes : 0u;

		ctl0_val = qspi_data_format(io_mode) |
			   ((uint32_t)pkt->cmd << QSPI_CTL0_CMDBYTE_SHIFT) |
			   ((rx_dummy & QSPI_CTL0_DUMMYCLK_MASK) << QSPI_CTL0_DUMMYCLK_SHIFT) |
			   (addr_len == 4 ? QSPI_CTL0_ADDRMODE_4BYTE : QSPI_CTL0_ADDRMODE_3BYTE);
		ctl1_val = ((uint32_t)addr_len << QSPI_CTL1_TXCOUNT_SHIFT) |
			   (rx_cnt << QSPI_CTL1_RXCOUNT_SHIFT) |
			   QSPI_CTL1_PREFETCH;
		data->tx_buf = NULL;
		data->tx_remaining = 0;
		data->rx_buf = pkt->data_buf;
		data->rx_remaining = pkt->num_bytes;
		iqmask = QSPI_INT_RX | QSPI_INT_IDLE;

	} else {
		/* Fallback: simple TX-only (cmd + optional data, no address) */
		uint32_t tx_cnt = 1u + pkt->num_bytes;

		ctl0_val = qspi_half_duplex_format(io_mode);
		ctl1_val = (tx_cnt << QSPI_CTL1_TXCOUNT_SHIFT);
		data->tx_buf = pkt->data_buf;
		data->tx_remaining = pkt->num_bytes;
		data->rx_buf = NULL;
		data->rx_remaining = 0;
		iqmask = QSPI_INT_TX | QSPI_INT_IDLE;
	}

	/* ── Step 3: Write QSPICTL0 / QSPICTL1 ─────────────────── */
	qspi_wr(base, QSPI_QSPICTL0_OFFSET, ctl0_val);
	qspi_wr(base, QSPI_QSPICTL1_OFFSET, ctl1_val);

	/* ── Step 4: Enable engine ──────────────────────────────── */
	qspi_wr(base, QSPI_CTL1_OFFSET, ctl1_base | QSPI_CTL1_ENABLE);

	/* ── Step 5: Flush FIFOs ────────────────────────────────── */
	qspi_wr(base, QSPI_QSPIFIFOCTL_OFFSET, QSPI_FIFOCTL_RXFLUSH | QSPI_FIFOCTL_TXFLUSH);

	/* ── Step 6: Prime the TX FIFO ──────────────────────────── */
	if (pkt->num_bytes == 0) {
		/* cmd-only: push the command byte */
		qspi_wr(base, QSPI_TXDATA_OFFSET, pkt->cmd & 0xFFu);

	} else if (pkt->dir == MSPI_RX && addr_len == 0) {
		/* RDID/RDSR: push cmd byte */
		qspi_wr(base, QSPI_TXDATA_OFFSET, pkt->cmd & 0xFFu);

	} else if (pkt->dir == MSPI_TX && addr_len > 0) {
		/* Page program: push address bytes first */
		qspi_push_address(base, pkt->address, addr_len);
		/* then as much data as fits in FIFO */
		while (data->tx_remaining > 0 &&
		       (qspi_rd(base, QSPI_QSPISTATUS_OFFSET) & QSPI_STS_TXFIFONF)) {
			qspi_wr(base, QSPI_TXDATA_OFFSET, *data->tx_buf++);
			data->tx_remaining--;
		}

	} else if (pkt->dir == MSPI_RX && addr_len > 0) {
		/* Quad read: push address bytes (engine auto-emits cmd+addr+dummy) */
		qspi_push_address(base, pkt->address, addr_len);
	} else {
		/* Fallback TX-only: cmd then data */
		qspi_wr(base, QSPI_TXDATA_OFFSET, pkt->cmd & 0xFFu);
		while (data->tx_remaining > 0 &&
		       (qspi_rd(base, QSPI_QSPISTATUS_OFFSET) & QSPI_STS_TXFIFONF)) {
			qspi_wr(base, QSPI_TXDATA_OFFSET, *data->tx_buf++);
			data->tx_remaining--;
		}
	}

	/* ── Step 7: Arm the interrupt mask then wait ────────────── */
	qspi_wr(base, QSPI_INT_IMASK_OFFSET, iqmask);

	timeout = (xfer->timeout == 0) ? K_FOREVER : K_MSEC(xfer->timeout);
	if (k_sem_take(&data->completion, timeout) != 0) {
		/* Timeout: disable all interrupts and abort */
		qspi_wr(base, QSPI_INT_IMASK_OFFSET, 0);
		qspi_wr(base, QSPI_CTL1_OFFSET, ctl1_base); /* disable engine */
		LOG_ERR("frame timeout");
		ret = -ETIMEDOUT;
	}

	return ret;
}

/** Update the SCLK prescaler for a new target frequency. */
static int qspi_apply_freq(const struct device *dev, uint32_t freq)
{
	const struct qspi_mspm33_config *cfg = dev->config;
	uintptr_t base = DEVICE_MMIO_GET(dev);
	uint32_t clk_rate = 0;
	int ret;

	if (freq == 0) {
		return 0;
	}

	ret = clock_control_get_rate(DEVICE_DT_GET(DT_NODELABEL(ckm)),
				     (clock_control_subsys_t)&cfg->clock_subsys,
				     &clk_rate);
	if (ret < 0) {
		LOG_ERR("clock_control_get_rate failed: %d", ret);
		return ret;
	}

	if (clk_rate == 0 || freq > clk_rate) {
		return -EINVAL;
	}

	/* SCLK = periclk / (2 * (SCR + 1))  =>  SCR = ceil(periclk / (2*freq)) - 1 */
	uint32_t scr = DIV_ROUND_UP(clk_rate, 2u * freq);

	if (scr > 0) {
		scr--;
	}
	if (scr > 0x3FFu) {
		scr = 0x3FFu; /* 10-bit field */
	}

	qspi_wr(base, QSPI_CLKCTL_OFFSET, scr);
	return 0;
}

/* ── ISR ─────────────────────────────────────────────────────────────────── */

static void qspi_mspm33_isr(const struct device *dev)
{
	struct qspi_mspm33_data *data = dev->data;
	uintptr_t base = DEVICE_MMIO_GET(dev);
	uint32_t iidx;
	uint32_t status;

	/*
	 * Process all pending interrupt events.  Reading IIDX returns the
	 * highest-priority pending interrupt index and auto-clears that event.
	 */
	while ((iidx = qspi_rd(base, QSPI_INT_IIDX_OFFSET)) != 0) {
		switch (iidx) {
		case QSPI_IIDX_RX:
			/* Drain RX FIFO into the receive buffer. */
			status = qspi_rd(base, QSPI_QSPISTATUS_OFFSET);
			while (data->rx_remaining > 0 && !(status & QSPI_STS_RXFIFOE)) {
				*data->rx_buf++ = (uint8_t)qspi_rd(base, QSPI_RXDATA_OFFSET);
				data->rx_remaining--;
				status = qspi_rd(base, QSPI_QSPISTATUS_OFFSET);
			}
			break;

		case QSPI_IIDX_TX:
			/* Refill TX FIFO from the transmit buffer. */
			status = qspi_rd(base, QSPI_QSPISTATUS_OFFSET);
			while (data->tx_remaining > 0 && (status & QSPI_STS_TXFIFONF)) {
				qspi_wr(base, QSPI_TXDATA_OFFSET, *data->tx_buf++);
				data->tx_remaining--;
				status = qspi_rd(base, QSPI_QSPISTATUS_OFFSET);
			}
			break;

		case QSPI_IIDX_IDLE:
			/*
			 * Transfer complete.  Drain any remaining RX data that
			 * arrived after the last RX interrupt.
			 */
			status = qspi_rd(base, QSPI_QSPISTATUS_OFFSET);
			while (data->rx_remaining > 0 && !(status & QSPI_STS_RXFIFOE)) {
				*data->rx_buf++ = (uint8_t)qspi_rd(base, QSPI_RXDATA_OFFSET);
				data->rx_remaining--;
				status = qspi_rd(base, QSPI_QSPISTATUS_OFFSET);
			}
			/* Disable all interrupts before posting the semaphore. */
			qspi_wr(base, QSPI_INT_IMASK_OFFSET, 0);
			k_sem_give(&data->completion);
			return;

		default:
			/* Clear unknown/spurious interrupt. */
			qspi_wr(base, QSPI_INT_ICLR_OFFSET, qspi_rd(base, QSPI_INT_RIS_OFFSET));
			break;
		}
	}
}

/* ── MSPI API implementation ─────────────────────────────────────────────── */

/**
 * mspi_config: (re-)initialise the QSPI controller hardware.
 */
static int qspi_mspm33_config(const struct mspi_dt_spec *spec)
{
	const struct device *dev = spec->bus;
	const struct mspi_cfg *mcfg = &spec->config;
	const struct qspi_mspm33_config *cfg = dev->config;
	uintptr_t base = DEVICE_MMIO_GET(dev);
	int ret;

	if (mcfg->op_mode != MSPI_OP_MODE_CONTROLLER) {
		LOG_ERR("only controller mode is supported");
		return -ENOTSUP;
	}
	if (mcfg->duplex != MSPI_HALF_DUPLEX) {
		LOG_ERR("only half-duplex mode is supported");
		return -ENOTSUP;
	}

	/* Apply pin configuration */
	ret = pinctrl_apply_state(cfg->pcfg, PINCTRL_STATE_DEFAULT);
	if (ret < 0) {
		LOG_ERR("pinctrl_apply_state failed: %d", ret);
		return ret;
	}

	/* Power-on the peripheral via GPRCM */
	qspi_wr(base, QSPI_PWREN_OFFSET, QSPI_PWREN_KEY_ENABLE);

	/* Brief startup delay (~10 µs) for supply/reset ramp */
	k_busy_wait(10);

	/* Select the peripheral clock source from DT */
	qspi_wr(base, QSPI_CLKSEL_OFFSET, cfg->clksel_bit);
	qspi_wr(base, QSPI_CLKDIV_OFFSET, QSPI_CLKDIV_RATIO_DIV1);

	/*
	 * Configure CTL1: controller mode (CP=1), MSB-first (MSB=1).
	 * Leave ENABLE clear until the first frame.
	 */
	qspi_wr(base, QSPI_CTL1_OFFSET, QSPI_CTL1_CP | QSPI_CTL1_MSB);

	/* Wire up and enable the CPU-level interrupt. */
	cfg->irq_config_fn();

	/* Apply the max_freq from the controller-level config if provided */
	if (mcfg->max_freq != 0) {
		ret = qspi_apply_freq(dev, mcfg->max_freq);
		if (ret < 0) {
			return ret;
		}
	}

	LOG_INF("MSPM33 QSPI controller configured");
	return 0;
}

/**
 * mspi_dev_config: store per-device parameters (frequency, io_mode, etc.).
 */
static int qspi_mspm33_dev_config(const struct device *dev,
				  const struct mspi_dev_id *dev_id,
				  const enum mspi_dev_cfg_mask param_mask,
				  const struct mspi_dev_cfg *cfg)
{
	struct qspi_mspm33_data *data = dev->data;
	int ret = 0;

	if (k_mutex_lock(&data->bus_lock, K_MSEC(CONFIG_MSPI_COMPLETION_TIMEOUT_TOLERANCE))) {
		return -EBUSY;
	}

	data->dev_id = dev_id;

	if (param_mask == MSPI_DEVICE_CONFIG_NONE) {
		goto out;
	}

	if (param_mask & MSPI_DEVICE_CONFIG_CE_NUM) {
		data->dev_cfg.ce_num = cfg->ce_num;
	}
	if (param_mask & MSPI_DEVICE_CONFIG_FREQUENCY) {
		ret = qspi_apply_freq(dev, cfg->freq);
		if (ret < 0) {
			goto out;
		}
		data->dev_cfg.freq = cfg->freq;
	}
	if (param_mask & MSPI_DEVICE_CONFIG_IO_MODE) {
		if (cfg->io_mode >= MSPI_IO_MODE_OCTAL) {
			LOG_ERR("octal/hex modes not supported");
			ret = -ENOTSUP;
			goto out;
		}
		data->dev_cfg.io_mode = cfg->io_mode;
	}
	if (param_mask & MSPI_DEVICE_CONFIG_DATA_RATE) {
		if (cfg->data_rate != MSPI_DATA_RATE_SINGLE) {
			LOG_ERR("only single data rate is supported");
			ret = -ENOTSUP;
			goto out;
		}
		data->dev_cfg.data_rate = cfg->data_rate;
	}
	if (param_mask & MSPI_DEVICE_CONFIG_CPP) {
		data->dev_cfg.cpp = cfg->cpp;
	}
	if (param_mask & MSPI_DEVICE_CONFIG_ENDIAN) {
		data->dev_cfg.endian = cfg->endian;
	}
	if (param_mask & MSPI_DEVICE_CONFIG_CE_POL) {
		data->dev_cfg.ce_polarity = cfg->ce_polarity;
	}
	if (param_mask & MSPI_DEVICE_CONFIG_RX_DUMMY) {
		data->dev_cfg.rx_dummy = cfg->rx_dummy;
	}
	if (param_mask & MSPI_DEVICE_CONFIG_TX_DUMMY) {
		data->dev_cfg.tx_dummy = cfg->tx_dummy;
	}
	if (param_mask & MSPI_DEVICE_CONFIG_READ_CMD) {
		data->dev_cfg.read_cmd = cfg->read_cmd;
	}
	if (param_mask & MSPI_DEVICE_CONFIG_WRITE_CMD) {
		data->dev_cfg.write_cmd = cfg->write_cmd;
	}
	if (param_mask & MSPI_DEVICE_CONFIG_CMD_LEN) {
		data->dev_cfg.cmd_length = cfg->cmd_length;
	}
	if (param_mask & MSPI_DEVICE_CONFIG_ADDR_LEN) {
		data->dev_cfg.addr_length = cfg->addr_length;
	}

out:
	k_mutex_unlock(&data->bus_lock);
	return ret;
}

/**
 * mspi_get_channel_status: returns 0 when bus is idle.
 */
static int qspi_mspm33_get_channel_status(const struct device *dev, uint8_t ch)
{
	struct qspi_mspm33_data *data = dev->data;
	uintptr_t base = DEVICE_MMIO_GET(dev);

	ARG_UNUSED(ch);

	if (!(qspi_rd(base, QSPI_QSPISTATUS_OFFSET) & QSPI_STS_IDLE)) {
		return -EBUSY;
	}

	/* End the session started by mspi_dev_config */
	data->dev_id = NULL;
	k_mutex_unlock(&data->bus_lock);
	return 0;
}

/**
 * mspi_transceive: execute a multi-packet transfer.
 */
static int qspi_mspm33_transceive(const struct device *dev,
				  const struct mspi_dev_id *dev_id,
				  const struct mspi_xfer *xfer)
{
	struct qspi_mspm33_data *data = dev->data;
	int ret = 0;

	if (dev_id != data->dev_id) {
		LOG_ERR("transceive: dev_id mismatch (call mspi_dev_config first)");
		return -ESTALE;
	}

	if (xfer == NULL || xfer->packets == NULL || xfer->num_packet == 0) {
		return -EINVAL;
	}

	if (xfer->xfer_mode != MSPI_PIO) {
		return -ENOTSUP;
	}

	for (uint32_t i = 0; i < xfer->num_packet; i++) {
		ret = qspi_do_frame(dev, &xfer->packets[i], xfer);
		if (ret != 0) {
			LOG_ERR("frame %u failed: %d", i, ret);
			break;
		}
	}

	return ret;
}

/* ── Driver API table ────────────────────────────────────────────────────── */

static DEVICE_API(mspi, qspi_mspm33_driver_api) = {
	.config            = qspi_mspm33_config,
	.dev_config        = qspi_mspm33_dev_config,
	.get_channel_status = qspi_mspm33_get_channel_status,
	.transceive        = qspi_mspm33_transceive,
};

/* ── Init function ───────────────────────────────────────────────────────── */

static int qspi_mspm33_init(const struct device *dev)
{
	const struct qspi_mspm33_config *cfg = dev->config;

	DEVICE_MMIO_MAP(dev, K_MEM_CACHE_NONE);

	const struct mspi_dt_spec spec = {
		.bus    = dev,
		.config = cfg->mspicfg,
	};

	return qspi_mspm33_config(&spec);
}

/* ── Per-instance instantiation macro ───────────────────────────────────── */

#define QSPI_MSPM33_IRQ_HANDLER(inst)						\
	static void qspi_mspm33_irq_config_##inst(void)				\
	{									\
		IRQ_CONNECT(DT_INST_IRQN(inst),					\
			    DT_INST_IRQ(inst, priority),			\
			    qspi_mspm33_isr,					\
			    DEVICE_DT_INST_GET(inst), 0);			\
		irq_enable(DT_INST_IRQN(inst));					\
	}

#define QSPI_MSPM33_INIT(inst)							\
									\
	PINCTRL_DT_INST_DEFINE(inst);						\
									\
	QSPI_MSPM33_IRQ_HANDLER(inst)						\
									\
	static const struct qspi_mspm33_config qspi_mspm33_cfg_##inst = {	\
		DEVICE_MMIO_ROM_INIT(DT_DRV_INST(inst)),			\
		.irq_config_fn = qspi_mspm33_irq_config_##inst,		\
		.pcfg          = PINCTRL_DT_INST_DEV_CONFIG_GET(inst),		\
		.clock_subsys  = {.clk = DT_INST_CLOCKS_CELL(inst, clk)},	\
		.clksel_bit    = MSPM0_CLOCK_PERIPH_REG_MASK(			\
					DT_INST_CLOCKS_CELL(inst, clk)),	\
		.mspicfg = {							\
			.channel_num  = 0,					\
			.op_mode      = MSPI_OP_MODE_CONTROLLER,		\
			.duplex       = MSPI_HALF_DUPLEX,			\
			.dqs_support  = false,					\
			.num_periph   = DT_INST_CHILD_NUM(inst),		\
		},								\
	};									\
									\
	static struct qspi_mspm33_data qspi_mspm33_data_##inst = {		\
		.completion = Z_SEM_INITIALIZER(				\
			qspi_mspm33_data_##inst.completion, 0, 1),		\
		.bus_lock   = Z_MUTEX_INITIALIZER(				\
			qspi_mspm33_data_##inst.bus_lock),			\
		.dev_cfg    = {0},						\
	};									\
									\
	DEVICE_DT_INST_DEFINE(inst, qspi_mspm33_init, NULL,			\
			      &qspi_mspm33_data_##inst,			\
			      &qspi_mspm33_cfg_##inst,				\
			      POST_KERNEL,					\
			      CONFIG_MSPI_INIT_PRIORITY,			\
			      &qspi_mspm33_driver_api);

DT_INST_FOREACH_STATUS_OKAY(QSPI_MSPM33_INIT)
