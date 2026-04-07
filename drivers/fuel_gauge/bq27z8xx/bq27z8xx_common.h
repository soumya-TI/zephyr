/*
 * Copyright (c) 2023, ithinx GmbH
 * Copyright (c) 2023, Tonies GmbH
 * Copyright (c) 2026, Texas Instruments Incorporated
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_FUELGAUGE_BQ27Z8XX_COMMON_H_
#define ZEPHYR_DRIVERS_FUELGAUGE_BQ27Z8XX_COMMON_H_

#include <zephyr/drivers/fuel_gauge.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

/**
 * @brief Registers common to bq27z758 and bq27z855.
 *
 * All addresses and semantics are identical on both devices.
 */
enum bq27z8xx_regs {
	BQ27Z8XX_MANUFACTURERACCESS = 0x00,    /* R/W */
	BQ27Z8XX_ATRATE = 0x02,                /* R/W, Unit: mA, Range: -32768..32767 */
	BQ27Z8XX_ATRATETIMETOEMPTY = 0x04,     /* R/O, Unit: minutes, Range: 0..65535 */
	BQ27Z8XX_TEMPERATURE = 0x06,           /* R/O, Unit: 0.1 K, Range: 0..32767 */
	BQ27Z8XX_VOLTAGE = 0x08,               /* R/O, Unit: mV, Range: 0..32767 */
	BQ27Z8XX_BATTERYSTATUS = 0x0A,         /* R/O, Unit: status bits */
	BQ27Z8XX_CURRENT = 0x0C,               /* R/O, Unit: mA, Range: -32768..32767 */
	BQ27Z8XX_REMAININGCAPACITY = 0x10,     /* R/O, Unit: mAh, Range: 0..32767 */
	BQ27Z8XX_FULLCHARGECAPACITY = 0x12,    /* R/O, Unit: mAh, Range: 0..32767 */
	BQ27Z8XX_AVERAGECURRENT = 0x14,        /* R/O, Unit: mA, Range: -32768..32767 */
	BQ27Z8XX_AVERAGETIMETOEMPTY = 0x16,    /* R/O, Unit: minutes, Range: 0..65535 */
	BQ27Z8XX_AVERAGETIMETOFULL = 0x18,     /* R/O, Unit: minutes, Range: 0..65535 */
	BQ27Z8XX_MAXLOADCURRENT = 0x1E,        /* R/O, Unit: mA, Range: 0..65535 */
	BQ27Z8XX_MAXLOADTIMETOEMPTY = 0x20,    /* R/O, Unit: minutes, Range: 0..65535 */
	BQ27Z8XX_AVERAGEPOWER = 0x22,          /* R/O, Unit: mW, Range: -32768..32767 */
	BQ27Z8XX_BTPDISCHARGESET = 0x24,       /* R/W */
	BQ27Z8XX_BTPCHARGESET = 0x26,          /* R/W */
	BQ27Z8XX_INTERNALTEMPERATURE = 0x28,   /* R/O, Unit: 0.1 K, Range: 0..32767 */
	BQ27Z8XX_CYCLECOUNT = 0x2A,            /* R/O, Unit: none, Range: 0..65535 */
	BQ27Z8XX_RELATIVESTATEOFCHARGE = 0x2C, /* R/O, Unit: percent, Range: 0..100 */
	BQ27Z8XX_STATEOFHEALTH = 0x2E,         /* R/O, Unit: percent, Range: 0..100 */
	BQ27Z8XX_CHARGINGVOLTAGE = 0x30,       /* R/O, Unit: mV, Range: 0..32767 */
	BQ27Z8XX_CHARGINGCURRENT = 0x32,       /* R/O, Unit: mA, Range: 0..32767 */
	BQ27Z8XX_TERMINATEVOLTAGE = 0x34,      /* R/W, Unit: mV, Range: 0..32767 */
	BQ27Z8XX_TIMESTAMPUPPER = 0x36,        /* R/O, Unit: seconds, Range: 0..65535 */
	BQ27Z8XX_TIMESTAMPLOWER = 0x38,        /* R/O, Unit: seconds, Range: 0..65535 */
	BQ27Z8XX_QMAXCYCLES = 0x3A,            /* R/O, Unit: none, Range: 0..65535 */
	BQ27Z8XX_DESIGNCAPACITY = 0x3C,        /* R/O (sealed), R/W (unsealed or factory access),
						* Unit: mAh, Range: 0..32767
						*/
	BQ27Z8XX_ALTMANUFACTURERACCESS = 0x3E, /* R/W */
	BQ27Z8XX_MACDATANEXT = 0x3E,           /* R/O, Next 32-byte DF block (auto-increment) */
	BQ27Z8XX_MACDATA = 0x40,               /* R/O, MAC data */
	BQ27Z8XX_MACDATASUM = 0x60,            /* R/O, Checksum over MAC command and data */
	BQ27Z8XX_MACDATALEN = 0x61,            /* R/O, Length of the MAC data */
	BQ27Z8XX_VOLTHISETTHRESHOLD = 0x62,    /* R/W, Unit: mV, Range: 0..5000 */
	BQ27Z8XX_VOLTHICLEARTHRESHOLD = 0x64,  /* R/W, Unit: mV, Range: 0..5000 */
	BQ27Z8XX_VOLTLOSETTHRESHOLD = 0x66,    /* R/W, Unit: mV, Range: 0..5000 */
	BQ27Z8XX_VOLTLOCLEARTHRESHOLD = 0x68,  /* R/W, Unit: mV, Range: 0..5000 */
	BQ27Z8XX_TEMPHISETTHRESHOLD = 0x6A,    /* R/W, Unit: degree celsius, Range: -128..127 */
	BQ27Z8XX_TEMPHICLEARTHRESHOLD = 0x6B,  /* R/W, Unit: degree celsius, Range: -128..127 */
	BQ27Z8XX_TEMPLOSETTHRESHOLD = 0x6C,    /* R/W, Unit: degree celsius, Range: -128..127 */
	BQ27Z8XX_TEMPLOCLEARTHRESHOLD = 0x6D,  /* R/W, Unit: degree celsius, Range: -128..127 */
	BQ27Z8XX_INTERRUPTSTATUS = 0x6E,       /* R/O, Unit: status bits */
	BQ27Z8XX_SOCDELTASETTHRESHOLD = 0x6F,  /* R/W, Unit: percent, Range: 0..100 */
};

/**
 * @brief AltManufacturerAccess read commands common to bq27z758 and bq27z855.
 *
 * These commands are at the same address on both devices and return data in the
 * same format. Use with bq27z8xx_read_mac().
 */
enum bq27z8xx_mac_read_cmds {
	BQ27Z8XX_MAC_CMD_DEVICETYPE = 0x0001,
	BQ27Z8XX_MAC_CMD_FIRMWAREVERSION = 0x0002,
	BQ27Z8XX_MAC_CMD_HARDWAREVERSION = 0x0003,
	BQ27Z8XX_MAC_CMD_IFSIGNATURE = 0x0004, /* Instruction flash integrity signature */
	BQ27Z8XX_MAC_CMD_STATICDFSIGNATURE = 0x0005,
	BQ27Z8XX_MAC_CMD_CHEMID = 0x0006,
	BQ27Z8XX_MAC_CMD_STATICCHEMDFSIGNATURE = 0x0008,
	BQ27Z8XX_MAC_CMD_ALLDFSIGNATURE = 0x0009,
	BQ27Z8XX_MAC_CMD_SECURITYKEYS = 0x0035,
	BQ27Z8XX_MAC_CMD_OPERATIONSTATUS = 0x0054,
};

/*
 * AltManufacturerAccess control commands common to bq27z758 and bq27z855.
 *
 * NOTE: Departure from fuel_gauge API encapsulation.
 * The Zephyr fuel_gauge API has no dedicated mechanism for fire-and-forget MAC
 * control commands (a 2-byte command write with no data payload and no response).
 * Rather than expose yet another device-specific function, we reuse the generic
 * FUEL_GAUGE_SBS_MFR_ACCESS set property, which writes a 16-bit word to register
 * 0x00 (MANUFACTURERACCESS). This is sufficient because control commands are
 * exactly a 2-byte write with no response data.
 *
 * Send via: fuel_gauge_set_prop(dev, FUEL_GAUGE_SBS_MFR_ACCESS,
 *               (union fuel_gauge_prop_val){.sbs_mfr_access_word = BQ27Z8XX_MAC_CTRL_CMD_XXX})
 */
enum bq27z8xx_mac_ctrl_cmds {
	BQ27Z8XX_MAC_CTRL_CMD_SHUTDOWNMODE = 0x0010,
	BQ27Z8XX_MAC_CTRL_CMD_CHGFETTOGGLE = 0x001F,
	BQ27Z8XX_MAC_CTRL_CMD_DSGFETTOGGLE = 0x0020,
	BQ27Z8XX_MAC_CTRL_CMD_GAUGING = 0x0021,
	BQ27Z8XX_MAC_CTRL_CMD_FET_CONTROL = 0x0022,
	BQ27Z8XX_MAC_CTRL_CMD_LIFETIMEDATACOLLECTION = 0x0023,
	BQ27Z8XX_MAC_CTRL_CMD_LIFETIMEDATARESET = 0x0028,
	BQ27Z8XX_MAC_CTRL_CMD_CALIBRATIONMODE = 0x002D,
	BQ27Z8XX_MAC_CTRL_CMD_LIFETIMEDATAFLUSH = 0x002E,
	BQ27Z8XX_MAC_CTRL_CMD_LIFETIMEDATASPEEDUPMODE = 0x002F,
	BQ27Z8XX_MAC_CTRL_CMD_SEALDEVICE = 0x0030,
	BQ27Z8XX_MAC_CTRL_CMD_DEVICERESET = 0x0041,
};

/*
 * Custom fuel gauge properties common to both bq27z758 and bq27z855.
 * Use with fuel_gauge_get_prop() / fuel_gauge_set_prop() /
 * fuel_gauge_get_buffer_prop().
 *
 * NOTE: Partial departure from fuel_gauge API encapsulation.
 * FUEL_GAUGE_CUSTOM_BEGIN is the Zephyr-designed extension point for
 * device-specific properties, so using it is intentional and supported.
 * However, callers must include this header directly and use these property
 * IDs rather than relying solely on the generic fuel_gauge API, meaning
 * application code is coupled to this driver.
 *
 * BQ27Z8XX_PROP_END marks the boundary beyond which each device may define
 * its own additional properties. Those device-specific IDs are declared in
 * the respective bq27z758.h / bq27z855.h headers.
 *
 * NOTE: FUEL_GAUGE_CYCLE_COUNT scaling differs per device:
 *   bq27z758 → raw register value (unit: counts)
 *   bq27z855 → raw register value × 100 (unit: hundredths of a cycle)
 * The bq27z855 variant_ops overrides FUEL_GAUGE_CYCLE_COUNT to apply the ×100
 * factor; the common handler returns the raw value (bq27z758 behaviour).
 */
enum bq27z8xx_prop {

	/* Get only */

	/** Max load current. Union field: avg_current (int, uA) */
	BQ27Z8XX_PROP_MAX_LOAD_CURRENT = FUEL_GAUGE_CUSTOM_BEGIN,
	/** Time to empty at max load. Union field: runtime_to_empty (uint32_t, minutes) */
	BQ27Z8XX_PROP_MAX_LOAD_TIME_TO_EMPTY,
	/** Average power. Union field: current (int, uW) */
	BQ27Z8XX_PROP_AVERAGE_POWER,
	/** Internal die temperature. Union field: temperature (uint16_t, 0.1K) */
	BQ27Z8XX_PROP_INTERNAL_TEMPERATURE,
	/** State of health. Union field: relative_state_of_charge (uint8_t, %) */
	BQ27Z8XX_PROP_STATE_OF_HEALTH,
	/** Timestamp upper word. Union field: sbs_mfr_access_word (uint16_t, seconds upper) */
	BQ27Z8XX_PROP_TIMESTAMP_UPPER,
	/** Timestamp lower word. Union field: sbs_mfr_access_word (uint16_t, seconds lower) */
	BQ27Z8XX_PROP_TIMESTAMP_LOWER,
	/** Qmax cycles. Union field: cycle_count (uint32_t) */
	BQ27Z8XX_PROP_QMAX_CYCLES,
	/** Interrupt status flags. Union field: fg_status (uint16_t) */
	BQ27Z8XX_PROP_INTERRUPT_STATUS,

	/* Get & Set */

	/** Battery trip point discharge threshold. Union field: sbs_mfr_access_word (uint16_t) */
	BQ27Z8XX_PROP_BTP_DISCHARGE_SET,
	/** Battery trip point charge threshold. Union field: sbs_mfr_access_word (uint16_t) */
	BQ27Z8XX_PROP_BTP_CHARGE_SET,
	/** Terminate voltage. Union field: voltage (int, uV) */
	BQ27Z8XX_PROP_TERMINATE_VOLTAGE,
	/** Voltage high clear threshold. Union field: high_voltage_alarm (uint32_t, uV) */
	BQ27Z8XX_PROP_VOLT_HI_CLEAR_THRESHOLD,
	/** Voltage low clear threshold. Union field: low_voltage_alarm (uint32_t, uV) */
	BQ27Z8XX_PROP_VOLT_LO_CLEAR_THRESHOLD,
	/** Temperature high clear threshold. Union field: high_temperature_alarm (uint16_t, 0.1K)
	 */
	BQ27Z8XX_PROP_TEMP_HI_CLEAR_THRESHOLD,
	/** Temperature low clear threshold. Union field: low_temperature_alarm (uint16_t, 0.1K) */
	BQ27Z8XX_PROP_TEMP_LO_CLEAR_THRESHOLD,

	/* MAC read buffer properties (use with fuel_gauge_get_buffer_prop()) */

	/** Device type identifier. Struct: bq27z8xx_device_type */
	BQ27Z8XX_BUFFER_PROP_DEVICETYPE,
	/** Firmware version. Struct: bq27z8xx_firmware_version */
	BQ27Z8XX_BUFFER_PROP_FIRMWAREVERSION,
	/** Hardware version. Struct: bq27z8xx_hardware_version */
	BQ27Z8XX_BUFFER_PROP_HARDWAREVERSION,
	/** Chemistry ID used for gauging. Struct: bq27z8xx_chem_id */
	BQ27Z8XX_BUFFER_PROP_CHEMID,
	/** Instruction flash integrity signature. Struct: bq27z8xx_if_signature */
	BQ27Z8XX_BUFFER_PROP_IFSIGNATURE,
	/** Static data flash signature. Struct: bq27z8xx_static_df_signature */
	BQ27Z8XX_BUFFER_PROP_STATICDFSIGNATURE,
	/** Static chemistry data flash signature. Struct: bq27z8xx_static_chem_df_signature */
	BQ27Z8XX_BUFFER_PROP_STATICCHEMDFSIGNATURE,
	/** Signature over all data flash. Struct: bq27z8xx_all_df_signature */
	BQ27Z8XX_BUFFER_PROP_ALLDFSIGNATURE,
	/** Security key values (UNSEAL, FULL ACCESS, etc.). Struct: bq27z8xx_security_keys */
	BQ27Z8XX_BUFFER_PROP_SECURITYKEYS,

	/** Sentinel — device-specific property IDs begin here. */
	BQ27Z8XX_PROP_END,
};

/*
 * Structs for common MAC read buffer properties.
 * The first byte of every struct is the data length returned by the device.
 * Pass sizeof(struct) as dst_len to fuel_gauge_get_buffer_prop().
 */

struct bq27z8xx_device_type {
	uint8_t length;
	uint16_t device_type;
} __packed;

struct bq27z8xx_firmware_version {
	uint8_t length;
	uint16_t device_number;
	uint16_t version;
	uint16_t build_number;
	uint8_t firmware_type;
	uint16_t dynamic_z_track_version;
	uint16_t reserved;
} __packed;

struct bq27z8xx_hardware_version {
	uint8_t length;
	uint16_t hardware_version;
} __packed;

struct bq27z8xx_chem_id {
	uint8_t length;
	uint16_t chem_id;
} __packed;

struct bq27z8xx_if_signature {
	uint8_t length;
	uint16_t signature;
} __packed;

struct bq27z8xx_static_df_signature {
	uint8_t length;
	uint16_t signature;
} __packed;

struct bq27z8xx_static_chem_df_signature {
	uint8_t length;
	uint16_t signature;
} __packed;

struct bq27z8xx_all_df_signature {
	uint8_t length;
	uint16_t signature;
} __packed;

struct bq27z8xx_security_keys {
	uint8_t length;
	uint16_t unseal_key[2];
	uint16_t full_access_key[2];
	uint16_t df_read_only_key[2];
	uint16_t manual_pf_key[2];
	uint16_t lifetimes_reset_key[2];
	uint16_t override_key[2];
	uint16_t mfg_info_c_write_key[2];
} __packed;

/* BatteryStatus (0x0A) bits */
#define BQ27Z8XX_BATTERY_STATUS_TCA  BIT(14) /* Terminate Charge Alarm */
#define BQ27Z8XX_BATTERY_STATUS_TDA  BIT(11) /* Terminate Discharge Alarm */
#define BQ27Z8XX_BATTERY_STATUS_RCA  BIT(9)  /* Remaining Capacity Alarm */
#define BQ27Z8XX_BATTERY_STATUS_INIT BIT(7)  /* Initialization */
#define BQ27Z8XX_BATTERY_STATUS_DSG  BIT(6)  /* Discharging */
#define BQ27Z8XX_BATTERY_STATUS_FC   BIT(5)  /* Fully Charged */
#define BQ27Z8XX_BATTERY_STATUS_FD   BIT(4)  /* Fully Discharged */

/* InterruptStatus (0x6E) bits */
#define BQ27Z8XX_INTERRUPT_STATUS_SOC_DELTA BIT(4) /* State of Charge delta threshold crossed */
#define BQ27Z8XX_INTERRUPT_STATUS_TEMP_LO   BIT(3) /* Temperature low threshold crossed */
#define BQ27Z8XX_INTERRUPT_STATUS_TEMP_HI   BIT(2) /* Temperature high threshold crossed */
#define BQ27Z8XX_INTERRUPT_STATUS_VOLT_LO   BIT(1) /* Voltage low threshold crossed */
#define BQ27Z8XX_INTERRUPT_STATUS_VOLT_HI   BIT(0) /* Voltage high threshold crossed */

/* DeviceType MAC command response values */
/* TODO: Verify that these device types are correct */
#define BQ27Z8XX_DEVICE_TYPE_BQ27Z758 0x1758
#define BQ27Z8XX_DEVICE_TYPE_BQ27Z855 0x1855

/**
 * @brief Variant-specific operation dispatch.
 *
 * Each device variant (bq27z758, bq27z855) may register function pointers here
 * for properties that differ from the common behaviour. The common get/set/buffer
 * handlers call the variant first; a return of -ENOTSUP falls through to common
 * handling. NULL function pointers are permitted and treated as -ENOTSUP.
 */
struct bq27z8xx_variant_ops {
	/** Handle a device-specific or overridden scalar get property. May be NULL. */
	int (*get_prop)(const struct device *dev, fuel_gauge_prop_t prop,
			union fuel_gauge_prop_val *val);
	/** Handle a device-specific or overridden scalar set property. May be NULL. */
	int (*set_prop)(const struct device *dev, fuel_gauge_prop_t prop,
			union fuel_gauge_prop_val val);
	/** Handle a device-specific or overridden buffer get property. May be NULL. */
	int (*get_buffer_prop)(const struct device *dev, fuel_gauge_prop_t prop, void *dst,
			       size_t dst_len);
};

/**
 * @brief Common driver configuration (const, stored in flash).
 */
struct bq27z8xx_config {
	struct i2c_dt_spec i2c;
	const struct bq27z8xx_variant_ops *variant_ops; /* set at compile time */
};

/* Data flash MAC address range (both BQ27Z758 and BQ27Z855) */
#define BQ27Z8XX_DF_ADDR_MIN 0x4000u
#define BQ27Z8XX_DF_ADDR_MAX 0x5FFFu

/* Transport helpers — used by common code and optionally by variant layers and shell. */
const struct i2c_dt_spec *bq27z8xx_get_i2c_dt_spec(const struct device *dev);
int bq27z8xx_read8(const struct device *dev, uint8_t reg, uint8_t *value);
int bq27z8xx_write8(const struct device *dev, uint8_t reg, uint8_t value);
int bq27z8xx_read16(const struct device *dev, uint8_t reg, uint16_t *value);
int bq27z8xx_write16(const struct device *dev, uint8_t reg, uint16_t value);

/**
 * @brief Read an AltManufacturerAccess command response.
 *
 * Writes @p cmd to register 0x3E, reads back 36 bytes, verifies the command
 * echo and checksum, then copies the payload into @p data.
 *
 * @param dev  Device pointer.
 * @param cmd  MAC command code.
 * @param data Output buffer; data[0] receives the payload length, data[1..len]
 *             receives the payload bytes.
 * @param len  Maximum payload bytes to copy (max 32).
 * @param delay Time to wait between MAC Write and read-back. Necessary for Flash
 *              signature related commands.
 *
 * @return 0 on success, negative errno on failure.
 */
int bq27z8xx_read_mac(const struct device *dev, uint16_t cmd, uint8_t *data, int len,
		      k_timeout_t delay);

/**
 * @brief Write an AltManufacturerAccess block write command.
 *
 * Sends @p cmd to register 0x3E, writes @p data_len bytes of @p data to the
 * MAC data buffer (0x40), then commits the transaction by writing the checksum
 * to 0x60 and the total length to 0x61.
 *
 * @p cmd is passed as a raw uint16_t so that device-specific write command
 * enums (e.g. bq27z855_mac_write_cmds) can be forwarded without a cast at
 * every call site.
 *
 * @param dev      Device pointer.
 * @param cmd      MAC write command code.
 * @param data     Payload bytes (NULL if data_len == 0).
 * @param data_len Number of payload bytes (max 32).
 *
 * @return 0 on success, negative errno on failure.
 */
int bq27z8xx_write_mac(const struct device *dev, uint16_t cmd, const uint8_t *data,
		       size_t data_len);

/**
 * @brief Read up to 32 bytes from the device data flash.
 *
 * Sends @p addr as a MAC command to ALTMANUFACTURERACCESS (0x3E) and reads
 * back up to @p len bytes of payload.  @p addr must be the full 16-bit MAC
 * command word (e.g. 0x4010 to access data flash at offset 0x0010).
 * Returns -EINVAL if @p addr is outside BQ27Z8XX_DF_ADDR_MIN..BQ27Z8XX_DF_ADDR_MAX.
 *
 * @param dev  Device pointer.
 * @param addr Full 16-bit MAC address word (must be in 0x4000–0x7FFF).
 * @param buf  Output buffer; buf[0] receives the device-reported payload
 *             length, buf[1..len] receives the payload bytes.
 * @param len  Maximum payload bytes to copy (max 32).
 *
 * @return 0 on success, negative errno on failure.
 */
int bq27z8xx_df_read(const struct device *dev, uint16_t addr, uint8_t *buf, uint8_t len);

/**
 * @brief Write up to 32 bytes to the device data flash.
 *
 * Sends @p addr as a MAC command to ALTMANUFACTURERACCESS (0x3E), writes
 * @p data_len bytes to the MAC data buffer (0x40), then commits the
 * transaction with the checksum and length bytes.
 * Returns -EINVAL if @p addr is outside BQ27Z8XX_DF_ADDR_MIN..BQ27Z8XX_DF_ADDR_MAX.
 *
 * @param dev      Device pointer.
 * @param addr     Full 16-bit MAC address word (must be in 0x4000–0x7FFF).
 * @param data     Payload bytes (NULL if data_len == 0).
 * @param data_len Number of payload bytes (max 32).
 *
 * @return 0 on success, negative errno on failure.
 */
int bq27z8xx_df_write(const struct device *dev, uint16_t addr, const uint8_t *data,
		      uint8_t data_len);

/**
 * @brief Read the next consecutive 32-byte DF block using auto-increment.
 *
 * The device auto-increments its internal DF address by 32 after each DF
 * block read.  A block read to register 0x44 returns the next 32 bytes
 * without re-sending the starting address, greatly reducing bus traffic when
 * reading multiple consecutive DF blocks.
 *
 * Must be preceded by a successful bq27z8xx_df_read() call (or another
 * bq27z8xx_df_read_next() call).
 *
 * @param dev  Device pointer.
 * @param buf  Output buffer; buf[0] receives the payload length (always 32),
 *             buf[1..len] receives the next 32 data flash bytes.
 * @param len  Maximum payload bytes to copy (max 32).
 *
 * @return 0 on success, negative errno on failure.
 */
int bq27z8xx_df_read_next(const struct device *dev, uint8_t *buf, uint8_t len);

/* fuel_gauge API entry points — registered in each variant's DEVICE_DT_INST_DEFINE. */
int bq27z8xx_init(const struct device *dev);
int bq27z8xx_battery_cutoff(const struct device *dev);
int bq27z8xx_get_prop(const struct device *dev, fuel_gauge_prop_t prop,
		      union fuel_gauge_prop_val *val);
int bq27z8xx_set_prop(const struct device *dev, fuel_gauge_prop_t prop,
		      union fuel_gauge_prop_val val);
int bq27z8xx_get_buffer_prop(const struct device *dev, fuel_gauge_prop_t prop, void *dst,
			     size_t dst_len);

#endif /* ZEPHYR_DRIVERS_FUELGAUGE_BQ27Z8XX_COMMON_H_ */
