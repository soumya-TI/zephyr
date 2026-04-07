/*
 * Copyright (c) 2023, ithinx GmbH
 * Copyright (c) 2023, Tonies GmbH
 * Copyright (c) 2026, Texas Instruments Incorporated
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bq27z8xx_common.h"

#include <zephyr/kernel.h>
#include <zephyr/drivers/fuel_gauge.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/logging/log.h>
#include <string.h>

LOG_MODULE_REGISTER(BQ27Z8XX);

#define BQ27Z8XX_MAC_DATA_LEN     32
#define BQ27Z8XX_MAC_OVERHEAD_LEN 4 /* 2 cmd bytes, 1 checksum byte, 1 length byte */
#define BQ27Z8XX_MAC_COMPLETE_LEN (BQ27Z8XX_MAC_DATA_LEN + BQ27Z8XX_MAC_OVERHEAD_LEN)

/* ---------------------------------------------------------------------------
 * Transport helpers
 * ---------------------------------------------------------------------------
 */

const struct i2c_dt_spec *bq27z8xx_get_i2c_dt_spec(const struct device *dev)
{
	const struct bq27z8xx_config *cfg = dev->config;

	return &cfg->i2c;
}

int bq27z8xx_read8(const struct device *dev, uint8_t reg, uint8_t *value)
{
	const struct bq27z8xx_config *cfg = dev->config;
	const int status = i2c_burst_read_dt(&cfg->i2c, reg, value, 1);

	if (status < 0) {
		LOG_DBG("Unable to read register 0x%02x", reg);
	}
	return status;
}

int bq27z8xx_write8(const struct device *dev, uint8_t reg, uint8_t value)
{
	uint8_t buf[2] = {reg, value};
	const struct bq27z8xx_config *cfg = dev->config;
	const int status = i2c_write_dt(&cfg->i2c, buf, sizeof(buf));

	if (status < 0) {
		LOG_DBG("Unable to write register 0x%02x", reg);
	}
	return status;
}

int bq27z8xx_read16(const struct device *dev, uint8_t reg, uint16_t *value)
{
	uint8_t i2c_data[2];
	const struct bq27z8xx_config *cfg = dev->config;

	/*
	 * LOG_DBG rather than LOG_ERR — register reads are used for presence
	 * detection and a LOG_ERR here would be very noisy on an absent device.
	 */
	const int status = i2c_burst_read_dt(&cfg->i2c, reg, i2c_data, sizeof(i2c_data));

	if (status < 0) {
		LOG_DBG("Unable to read register 0x%02x", reg);
		return status;
	}
	*value = sys_get_le16(i2c_data);

	return 0;
}

int bq27z8xx_write16(const struct device *dev, uint8_t reg, uint16_t value)
{
	uint8_t buf[3];
	const struct bq27z8xx_config *cfg = dev->config;

	buf[0] = reg;
	sys_put_le16(value, &buf[1]);

	const int status = i2c_write_dt(&cfg->i2c, buf, sizeof(buf));

	if (status < 0) {
		LOG_DBG("Unable to write register 0x%02x", reg);
	}
	return status;
}

/* ---------------------------------------------------------------------------
 * AltManufacturerAccess (MAC) protocol
 * ---------------------------------------------------------------------------
 */

int bq27z8xx_read_mac(const struct device *dev, uint16_t cmd, uint8_t *data, int len,
		      k_timeout_t delay)
{
	if (len > BQ27Z8XX_MAC_DATA_LEN) {
		return -EINVAL;
	}

	uint8_t buf[BQ27Z8XX_MAC_COMPLETE_LEN];
	const struct bq27z8xx_config *cfg = dev->config;

	/* Instead of MAC (0x00), ALTMAC (0x3E) is used as recommended by the datasheet */
	int ret = bq27z8xx_write16(dev, BQ27Z8XX_ALTMANUFACTURERACCESS, cmd);

	if (ret != 0) {
		return ret;
	}

	/* Some commands require the device extra time to compute their response
	 * before the result can be read back.  Callers supply the required wait
	 * via the delay parameter; pass K_NO_WAIT when no delay is needed.
	 */
	k_sleep(delay);

	/*
	 * The data read from BQ27Z8XX_ALTMANUFACTURERACCESS is:
	 * 0..1:  The command (echoed back for verification)
	 * 2..33: The data (up to 32 bytes)
	 * 34:    Checksum = (uint8_t)(0xFF - sum(bytes[0..33]))
	 * 35:    Length including command, data, checksum and length byte
	 *        (i.e. data_length + 4)
	 */
	ret = i2c_burst_read_dt(&cfg->i2c, BQ27Z8XX_ALTMANUFACTURERACCESS, buf,
				BQ27Z8XX_MAC_COMPLETE_LEN);
	if (ret != 0) {
		return ret;
	}

	/* The first two bytes are the echoed command — verify they match */
	const uint16_t read_cmd = sys_get_le16(buf);

	if (read_cmd != cmd) {
		LOG_ERR("Read command 0x%04x != written command 0x%04x", read_cmd, cmd);
		return -EIO;
	}

	const uint8_t checksum_actual = buf[34];
	uint8_t sum = 0; /* Intentionally 8-bit wide and overflowing */

	for (int i = 0; i < BQ27Z8XX_MAC_COMPLETE_LEN - 2; i++) {
		sum += buf[i];
	}

	const uint8_t checksum_expected = 0xFF - sum;

	if (checksum_expected != checksum_actual) {
		LOG_ERR("MAC checksum mismatch (expected 0x%02x, got 0x%02x)", checksum_expected,
			checksum_actual);
		return -EIO;
	}

	if (buf[35] < BQ27Z8XX_MAC_OVERHEAD_LEN || buf[35] > BQ27Z8XX_MAC_COMPLETE_LEN) {
		LOG_ERR("MAC length byte out of range: %u", buf[35]);
		return -EIO;
	}

	/* data[0] receives the payload length (total length minus the 4 overhead bytes) */
	data[0] = buf[35] - BQ27Z8XX_MAC_OVERHEAD_LEN;

	/* Copy only the payload into the caller's buffer, skipping the 2-byte command echo */
	memcpy(&data[1], &buf[2], len);

	return 0;
}

int bq27z8xx_write_mac(const struct device *dev, uint16_t cmd, const uint8_t *data, size_t data_len)
{
	if (data_len > BQ27Z8XX_MAC_DATA_LEN) {
		return -EINVAL;
	}

	const struct bq27z8xx_config *cfg = dev->config;

	/* Write the command word to ALTMANUFACTURERACCESS (0x3E) */
	int ret = bq27z8xx_write16(dev, BQ27Z8XX_ALTMANUFACTURERACCESS, cmd);

	if (ret != 0) {
		return ret;
	}

	/* Write payload bytes to MACDATA (0x40) */
	if (data_len > 0) {
		uint8_t buf[BQ27Z8XX_MAC_DATA_LEN + 1];

		buf[0] = BQ27Z8XX_MACDATA;
		memcpy(&buf[1], data, data_len);
		ret = i2c_write_dt(&cfg->i2c, buf, data_len + 1);
		if (ret != 0) {
			return ret;
		}
	}

	/*
	 * Checksum = 0xFF - ((cmd_lo + cmd_hi + sum(data)) & 0xFF)
	 * Mirrors the verification in bq27z8xx_read_mac().
	 */
	uint8_t sum = (uint8_t)(cmd & 0xFF) + (uint8_t)(cmd >> 8);

	for (size_t i = 0; i < data_len; i++) {
		sum += data[i];
	}

	ret = bq27z8xx_write8(dev, BQ27Z8XX_MACDATASUM, 0xFF - sum);
	if (ret != 0) {
		return ret;
	}

	/* Length = 2 (cmd) + data_len + 1 (checksum) + 1 (length itself) = 4 + data_len */
	return bq27z8xx_write8(dev, BQ27Z8XX_MACDATALEN, (uint8_t)(4 + data_len));
}

/* ---------------------------------------------------------------------------
 * Data flash access
 * ---------------------------------------------------------------------------
 */

int bq27z8xx_df_read(const struct device *dev, uint16_t addr, uint8_t *buf, uint8_t len)
{
	if (addr < BQ27Z8XX_DF_ADDR_MIN || addr > BQ27Z8XX_DF_ADDR_MAX) {
		return -EINVAL;
	}

	int ret = -EIO;

	/*
	 * Treat this as a 32-byte MAC read command, this is done with the address
	 * of the data flash segment as the payload.
	 */
	ret = bq27z8xx_read_mac(dev, addr, buf, len, K_NO_WAIT);
	if (ret != 0) {
		return ret;
	}

	return 0;
}

int bq27z8xx_df_write(const struct device *dev, uint16_t addr, const uint8_t *data,
		      uint8_t data_len)
{
	if (addr < BQ27Z8XX_DF_ADDR_MIN || addr > BQ27Z8XX_DF_ADDR_MAX) {
		return -EINVAL;
	}

	if (data_len > BQ27Z8XX_MAC_DATA_LEN) {
		return -EINVAL;
	}

	int ret = -EIO;

	ret = bq27z8xx_write_mac(dev, addr, data, data_len);
	if (ret != 0) {
		return ret;
	}

	/* Sleep to allow dataflash write to take effect */
	k_sleep(K_MSEC(40));

	return ret;
}

int bq27z8xx_df_read_next(const struct device *dev, uint8_t *buf, uint8_t len)
{
	const struct bq27z8xx_config *cfg = dev->config;

	/*
	 * The device auto-increments its internal DF address by 32 on each call;
	 * no address write is needed. Can still read address from 0x3E, and Checksum/Length
	 * from 0x60 and 0x61.
	 */
	uint8_t raw[BQ27Z8XX_MAC_DATA_LEN + 4];
	int ret = i2c_burst_read_dt(&cfg->i2c, BQ27Z8XX_MACDATANEXT, raw, sizeof(raw));

	if (ret != 0) {
		return ret;
	}

	buf[0] = BQ27Z8XX_MAC_DATA_LEN;
	memcpy(&buf[1], raw, MIN((size_t)len, (size_t)BQ27Z8XX_MAC_DATA_LEN));

	return 0;
}

/* ---------------------------------------------------------------------------
 * fuel_gauge API — scalar properties
 * ---------------------------------------------------------------------------
 */

int bq27z8xx_get_prop(const struct device *dev, fuel_gauge_prop_t prop,
		      union fuel_gauge_prop_val *val)
{
	const struct bq27z8xx_config *cfg = dev->config;
	int rc = -ENOTSUP;
	uint16_t tmp_val = 0;

	/* Try the variant handler first — it may override common behaviour */
	if (cfg->variant_ops != NULL && cfg->variant_ops->get_prop != NULL) {
		rc = cfg->variant_ops->get_prop(dev, prop, val);
		if (rc != -ENOTSUP) {
			return rc;
		}
	}

	/*
	 * Possibly negative values must be cast from uint16 to int16 before
	 * widening to the types used in fuel_gauge_prop_val.
	 */
	switch (prop) {
	case FUEL_GAUGE_AVG_CURRENT:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_AVERAGECURRENT, &tmp_val);
		val->avg_current = (int16_t)tmp_val * 1000;
		break;
	case FUEL_GAUGE_CYCLE_COUNT:
		/*
		 * bq27z758 returns raw counts; bq27z855 returns hundredths of a
		 * cycle (×100). The bq27z855 variant overrides this case — the
		 * common handler provides the bq27z758-compatible raw behaviour.
		 */
		rc = bq27z8xx_read16(dev, BQ27Z8XX_CYCLECOUNT, &tmp_val);
		val->cycle_count = tmp_val;
		break;
	case FUEL_GAUGE_CURRENT:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_CURRENT, &tmp_val);
		val->current = (int16_t)tmp_val * 1000;
		break;
	case FUEL_GAUGE_FULL_CHARGE_CAPACITY:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_FULLCHARGECAPACITY, &tmp_val);
		val->full_charge_capacity = tmp_val * 1000;
		break;
	case FUEL_GAUGE_REMAINING_CAPACITY:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_REMAININGCAPACITY, &tmp_val);
		val->remaining_capacity = tmp_val * 1000;
		break;
	case FUEL_GAUGE_RUNTIME_TO_EMPTY:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_AVERAGETIMETOEMPTY, &tmp_val);
		val->runtime_to_empty = tmp_val;
		break;
	case FUEL_GAUGE_RUNTIME_TO_FULL:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_AVERAGETIMETOFULL, &tmp_val);
		val->runtime_to_full = tmp_val;
		break;
	case FUEL_GAUGE_SBS_MFR_ACCESS:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_MANUFACTURERACCESS, &tmp_val);
		val->sbs_mfr_access_word = tmp_val;
		break;
	case FUEL_GAUGE_RELATIVE_STATE_OF_CHARGE:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_RELATIVESTATEOFCHARGE, &tmp_val);
		val->relative_state_of_charge = tmp_val;
		break;
	case FUEL_GAUGE_TEMPERATURE:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_TEMPERATURE, &tmp_val);
		val->temperature = tmp_val;
		break;
	case FUEL_GAUGE_VOLTAGE:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_VOLTAGE, &tmp_val);
		val->voltage = tmp_val * 1000;
		break;
	case FUEL_GAUGE_SBS_ATRATE:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_ATRATE, &tmp_val);
		val->sbs_at_rate = (int16_t)tmp_val;
		break;
	case FUEL_GAUGE_SBS_ATRATE_TIME_TO_EMPTY:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_ATRATETIMETOEMPTY, &tmp_val);
		val->sbs_at_rate_time_to_empty = tmp_val;
		break;
	case FUEL_GAUGE_CHARGE_VOLTAGE:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_CHARGINGVOLTAGE, &tmp_val);
		val->chg_voltage = tmp_val * 1000;
		break;
	case FUEL_GAUGE_CHARGE_CURRENT:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_CHARGINGCURRENT, &tmp_val);
		val->chg_current = tmp_val * 1000;
		break;
	case FUEL_GAUGE_STATUS:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_BATTERYSTATUS, &tmp_val);
		val->fg_status = tmp_val;
		break;
	case FUEL_GAUGE_DESIGN_CAPACITY:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_DESIGNCAPACITY, &tmp_val);
		val->design_cap = tmp_val;
		break;
	case FUEL_GAUGE_HIGH_VOLTAGE_ALARM:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_VOLTHISETTHRESHOLD, &tmp_val);
		val->high_voltage_alarm = tmp_val * 1000;
		break;
	case FUEL_GAUGE_LOW_VOLTAGE_ALARM:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_VOLTLOSETTHRESHOLD, &tmp_val);
		val->low_voltage_alarm = tmp_val * 1000;
		break;
	case FUEL_GAUGE_HIGH_TEMPERATURE_ALARM: {
		uint8_t tmp_val8;

		rc = bq27z8xx_read8(dev, BQ27Z8XX_TEMPHISETTHRESHOLD, &tmp_val8);
		val->high_temperature_alarm = (int8_t)tmp_val8 * 10 + 2731;
		break;
	}
	case FUEL_GAUGE_LOW_TEMPERATURE_ALARM: {
		uint8_t tmp_val8;

		rc = bq27z8xx_read8(dev, BQ27Z8XX_TEMPLOSETTHRESHOLD, &tmp_val8);
		val->low_temperature_alarm = (int8_t)tmp_val8 * 10 + 2731;
		break;
	}
	case FUEL_GAUGE_STATE_OF_CHARGE_ALARM: {
		uint8_t tmp_val8;

		rc = bq27z8xx_read8(dev, BQ27Z8XX_SOCDELTASETTHRESHOLD, &tmp_val8);
		val->state_of_charge_alarm = tmp_val8;
		break;
	}
	case BQ27Z8XX_PROP_MAX_LOAD_CURRENT:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_MAXLOADCURRENT, &tmp_val);
		val->avg_current = (int16_t)tmp_val * 1000;
		break;
	case BQ27Z8XX_PROP_MAX_LOAD_TIME_TO_EMPTY:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_MAXLOADTIMETOEMPTY, &tmp_val);
		val->runtime_to_empty = tmp_val;
		break;
	case BQ27Z8XX_PROP_AVERAGE_POWER:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_AVERAGEPOWER, &tmp_val);
		val->current = (int16_t)tmp_val * 1000;
		break;
	case BQ27Z8XX_PROP_INTERNAL_TEMPERATURE:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_INTERNALTEMPERATURE, &tmp_val);
		val->temperature = tmp_val;
		break;
	case BQ27Z8XX_PROP_STATE_OF_HEALTH:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_STATEOFHEALTH, &tmp_val);
		val->relative_state_of_charge = (uint8_t)tmp_val;
		break;
	case BQ27Z8XX_PROP_TIMESTAMP_UPPER:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_TIMESTAMPUPPER, &tmp_val);
		val->sbs_mfr_access_word = tmp_val;
		break;
	case BQ27Z8XX_PROP_TIMESTAMP_LOWER:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_TIMESTAMPLOWER, &tmp_val);
		val->sbs_mfr_access_word = tmp_val;
		break;
	case BQ27Z8XX_PROP_QMAX_CYCLES:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_QMAXCYCLES, &tmp_val);
		val->cycle_count = tmp_val;
		break;
	case BQ27Z8XX_PROP_INTERRUPT_STATUS: {
		uint8_t tmp_val8;

		rc = bq27z8xx_read8(dev, BQ27Z8XX_INTERRUPTSTATUS, &tmp_val8);
		val->fg_status = tmp_val8;
		break;
	}
	case BQ27Z8XX_PROP_BTP_DISCHARGE_SET:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_BTPDISCHARGESET, &tmp_val);
		val->sbs_mfr_access_word = tmp_val;
		break;
	case BQ27Z8XX_PROP_BTP_CHARGE_SET:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_BTPCHARGESET, &tmp_val);
		val->sbs_mfr_access_word = tmp_val;
		break;
	case BQ27Z8XX_PROP_TERMINATE_VOLTAGE:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_TERMINATEVOLTAGE, &tmp_val);
		val->voltage = tmp_val * 1000;
		break;
	case BQ27Z8XX_PROP_VOLT_HI_CLEAR_THRESHOLD:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_VOLTHICLEARTHRESHOLD, &tmp_val);
		val->high_voltage_alarm = tmp_val * 1000;
		break;
	case BQ27Z8XX_PROP_VOLT_LO_CLEAR_THRESHOLD:
		rc = bq27z8xx_read16(dev, BQ27Z8XX_VOLTLOCLEARTHRESHOLD, &tmp_val);
		val->low_voltage_alarm = tmp_val * 1000;
		break;
	case BQ27Z8XX_PROP_TEMP_HI_CLEAR_THRESHOLD: {
		uint8_t tmp_val8;

		rc = bq27z8xx_read8(dev, BQ27Z8XX_TEMPHICLEARTHRESHOLD, &tmp_val8);
		val->high_temperature_alarm = (int8_t)tmp_val8 * 10 + 2731;
		break;
	}
	case BQ27Z8XX_PROP_TEMP_LO_CLEAR_THRESHOLD: {
		uint8_t tmp_val8;

		rc = bq27z8xx_read8(dev, BQ27Z8XX_TEMPLOCLEARTHRESHOLD, &tmp_val8);
		val->low_temperature_alarm = (int8_t)tmp_val8 * 10 + 2731;
		break;
	}
	default:
		rc = -ENOTSUP;
	}

	return rc;
}

int bq27z8xx_set_prop(const struct device *dev, fuel_gauge_prop_t prop,
		      union fuel_gauge_prop_val val)
{
	const struct bq27z8xx_config *cfg = dev->config;
	int rc = -ENOTSUP;

	/* Try the variant handler first */
	if (cfg->variant_ops != NULL && cfg->variant_ops->set_prop != NULL) {
		rc = cfg->variant_ops->set_prop(dev, prop, val);
		if (rc != -ENOTSUP) {
			return rc;
		}
	}

	switch (prop) {
	case FUEL_GAUGE_SBS_MFR_ACCESS:
		rc = bq27z8xx_write16(dev, BQ27Z8XX_MANUFACTURERACCESS, val.sbs_mfr_access_word);
		break;
	case FUEL_GAUGE_SBS_ATRATE:
		rc = bq27z8xx_write16(dev, BQ27Z8XX_ATRATE, val.sbs_at_rate);
		break;
	case FUEL_GAUGE_HIGH_VOLTAGE_ALARM:
		rc = bq27z8xx_write16(dev, BQ27Z8XX_VOLTHISETTHRESHOLD,
				      val.high_voltage_alarm / 1000);
		break;
	case FUEL_GAUGE_LOW_VOLTAGE_ALARM:
		rc = bq27z8xx_write16(dev, BQ27Z8XX_VOLTLOSETTHRESHOLD,
				      val.low_voltage_alarm / 1000);
		break;
	case FUEL_GAUGE_HIGH_TEMPERATURE_ALARM:
		rc = bq27z8xx_write8(dev, BQ27Z8XX_TEMPHISETTHRESHOLD,
				     (int8_t)((val.high_temperature_alarm - 2731) / 10));
		break;
	case FUEL_GAUGE_LOW_TEMPERATURE_ALARM:
		rc = bq27z8xx_write8(dev, BQ27Z8XX_TEMPLOSETTHRESHOLD,
				     (int8_t)((val.low_temperature_alarm - 2731) / 10));
		break;
	case FUEL_GAUGE_STATE_OF_CHARGE_ALARM:
		rc = bq27z8xx_write8(dev, BQ27Z8XX_SOCDELTASETTHRESHOLD, val.state_of_charge_alarm);
		break;
	case BQ27Z8XX_PROP_BTP_DISCHARGE_SET:
		rc = bq27z8xx_write16(dev, BQ27Z8XX_BTPDISCHARGESET, val.sbs_mfr_access_word);
		break;
	case BQ27Z8XX_PROP_BTP_CHARGE_SET:
		rc = bq27z8xx_write16(dev, BQ27Z8XX_BTPCHARGESET, val.sbs_mfr_access_word);
		break;
	case BQ27Z8XX_PROP_TERMINATE_VOLTAGE:
		rc = bq27z8xx_write16(dev, BQ27Z8XX_TERMINATEVOLTAGE, val.voltage / 1000);
		break;
	case BQ27Z8XX_PROP_VOLT_HI_CLEAR_THRESHOLD:
		rc = bq27z8xx_write16(dev, BQ27Z8XX_VOLTHICLEARTHRESHOLD,
				      val.high_voltage_alarm / 1000);
		break;
	case BQ27Z8XX_PROP_VOLT_LO_CLEAR_THRESHOLD:
		rc = bq27z8xx_write16(dev, BQ27Z8XX_VOLTLOCLEARTHRESHOLD,
				      val.low_voltage_alarm / 1000);
		break;
	case BQ27Z8XX_PROP_TEMP_HI_CLEAR_THRESHOLD:
		rc = bq27z8xx_write8(dev, BQ27Z8XX_TEMPHICLEARTHRESHOLD,
				     (int8_t)((val.high_temperature_alarm - 2731) / 10));
		break;
	case BQ27Z8XX_PROP_TEMP_LO_CLEAR_THRESHOLD:
		rc = bq27z8xx_write8(dev, BQ27Z8XX_TEMPLOCLEARTHRESHOLD,
				     (int8_t)((val.low_temperature_alarm - 2731) / 10));
		break;
	default:
		rc = -ENOTSUP;
	}

	return rc;
}

/* ---------------------------------------------------------------------------
 * fuel_gauge API — buffer properties
 * ---------------------------------------------------------------------------
 */

/*
 * Helper macros: read a MAC command and verify the dst_len matches the
 * expected struct size.
 *
 * READ_MAC_PROP_DELAY — full form; pass an explicit k_timeout_t delay that is
 *   inserted between the command write and the response read.  Use this for
 *   commands where the device needs time to compute its response.
 *
 * READ_MAC_PROP — shorthand for the common case where no inter-transaction
 *   delay is required (passes K_NO_WAIT).
 */
#define READ_MAC_PROP_DELAY(cmd, struct_type, delay)                                               \
	do {                                                                                       \
		if (dst_len == sizeof(struct_type)) {                                              \
			rc = bq27z8xx_read_mac(dev, (cmd), (uint8_t *)dst, dst_len - 1, (delay));  \
		} else {                                                                           \
			rc = -EINVAL;                                                              \
		}                                                                                  \
	} while (0)

#define READ_MAC_PROP(cmd, struct_type) READ_MAC_PROP_DELAY(cmd, struct_type, K_NO_WAIT)

int bq27z8xx_get_buffer_prop(const struct device *dev, fuel_gauge_prop_t prop, void *dst,
			     size_t dst_len)
{
	const struct bq27z8xx_config *cfg = dev->config;
	int rc = -ENOTSUP;

	/* Try the variant handler first */
	if (cfg->variant_ops != NULL && cfg->variant_ops->get_buffer_prop != NULL) {
		rc = cfg->variant_ops->get_buffer_prop(dev, prop, dst, dst_len);
		if (rc != -ENOTSUP) {
			return rc;
		}
	}

	switch (prop) {
	case BQ27Z8XX_BUFFER_PROP_DEVICETYPE:
		READ_MAC_PROP(BQ27Z8XX_MAC_CMD_DEVICETYPE, struct bq27z8xx_device_type);
		break;
	case BQ27Z8XX_BUFFER_PROP_FIRMWAREVERSION:
		READ_MAC_PROP(BQ27Z8XX_MAC_CMD_FIRMWAREVERSION, struct bq27z8xx_firmware_version);
		break;
	case BQ27Z8XX_BUFFER_PROP_HARDWAREVERSION:
		READ_MAC_PROP(BQ27Z8XX_MAC_CMD_HARDWAREVERSION, struct bq27z8xx_hardware_version);
		break;
	case BQ27Z8XX_BUFFER_PROP_CHEMID:
		READ_MAC_PROP(BQ27Z8XX_MAC_CMD_CHEMID, struct bq27z8xx_chem_id);
		break;
	case BQ27Z8XX_BUFFER_PROP_IFSIGNATURE:
		/* Device requires 250 ms to compute the signature after the command
		 * is written before the result can be read back.
		 */
		READ_MAC_PROP_DELAY(BQ27Z8XX_MAC_CMD_IFSIGNATURE, struct bq27z8xx_if_signature,
				    K_MSEC(250));
		break;
	case BQ27Z8XX_BUFFER_PROP_STATICDFSIGNATURE:
		READ_MAC_PROP_DELAY(BQ27Z8XX_MAC_CMD_STATICDFSIGNATURE,
				    struct bq27z8xx_static_df_signature, K_MSEC(250));
		break;
	case BQ27Z8XX_BUFFER_PROP_STATICCHEMDFSIGNATURE:
		READ_MAC_PROP(BQ27Z8XX_MAC_CMD_STATICCHEMDFSIGNATURE,
			      struct bq27z8xx_static_chem_df_signature);
		break;
	case BQ27Z8XX_BUFFER_PROP_ALLDFSIGNATURE:
		READ_MAC_PROP_DELAY(BQ27Z8XX_MAC_CMD_ALLDFSIGNATURE,
				    struct bq27z8xx_all_df_signature, K_MSEC(250));
		break;
	case BQ27Z8XX_BUFFER_PROP_SECURITYKEYS:
		READ_MAC_PROP(BQ27Z8XX_MAC_CMD_SECURITYKEYS, struct bq27z8xx_security_keys);
		break;
	default:
		rc = -ENOTSUP;
	}

	return rc;
}

/* ---------------------------------------------------------------------------
 * Device initialisation
 * ---------------------------------------------------------------------------
 */

int bq27z8xx_init(const struct device *dev)
{
	const struct bq27z8xx_config *cfg = dev->config;

	if (!device_is_ready(cfg->i2c.bus)) {
		LOG_ERR("I2C bus not ready");
		return -ENODEV;
	}

	return 0;
}

int bq27z8xx_battery_cutoff(const struct device *dev)
{
	/*
	 * Buffer layout from bq27z8xx_read_mac():
	 *   buf[0]    = payload length (4 for OperationStatus on both variants)
	 *   buf[1..4] = 4-byte payload
	 *
	 * Both BQ27Z758 and BQ27Z855 place the FET state bits at the same
	 * positions within the lowest payload byte (buf[1]):
	 *   BIT(2) = CHG FET on
	 *   BIT(1) = DSG FET on
	 */
	uint8_t buf[5]; /* 1 length byte + 4 payload bytes */
	int ret;

	ret = bq27z8xx_read_mac(dev, BQ27Z8XX_MAC_CMD_OPERATIONSTATUS, buf, sizeof(buf) - 1,
				K_NO_WAIT);
	if (ret < 0) {
		return ret;
	}

	/*
	 * Use bq27z8xx_write16() directly rather than bq27z8xx_write_mac() —
	 * write_mac() with data_len=0 incorrectly writes a checksum to 0x60
	 * without a prior 0x40 data write, causing a NACK on real hardware.
	 * A bare write16 to ALTMANUFACTURERACCESS is the correct protocol for
	 * zero-data control subcommands.
	 */
	if (buf[1] & BIT(2)) {
		ret = bq27z8xx_write16(dev, BQ27Z8XX_ALTMANUFACTURERACCESS,
				       BQ27Z8XX_MAC_CTRL_CMD_CHGFETTOGGLE);
		if (ret < 0) {
			return ret;
		}
	}

	if (buf[1] & BIT(1)) {
		ret = bq27z8xx_write16(dev, BQ27Z8XX_ALTMANUFACTURERACCESS,
				       BQ27Z8XX_MAC_CTRL_CMD_DSGFETTOGGLE);
		if (ret < 0) {
			return ret;
		}
	}

	return 0;
}
