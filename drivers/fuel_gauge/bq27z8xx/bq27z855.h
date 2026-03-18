/*
 * Copyright (c) 2023, ithinx GmbH
 * Copyright (c) 2023, Tonies GmbH
 * Copyright (c) 2026, Texas Instruments Incorporated
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_FUELGAUGE_BQ27Z8XX_BQ27Z855_H_
#define ZEPHYR_DRIVERS_FUELGAUGE_BQ27Z8XX_BQ27Z855_H_

#include "bq27z8xx_common.h"

/**
 * @brief AltManufacturerAccess read commands specific to the bq27z855.
 *
 * Commands common to both bq27z758 and bq27z855 are in bq27z8xx_mac_read_cmds.
 * Use these with bq27z8xx_read_mac().
 */
enum bq27z855_mac_read_cmds {
	BQ27Z855_MAC_CMD_AUTHENTICATIONKEY = 0x0037, /* TODO: Check support */
	BQ27Z855_MAC_CMD_SAFETYALERT = 0x0050,
	BQ27Z855_MAC_CMD_SAFETYSTATUS = 0x0051,
	BQ27Z855_MAC_CMD_PFALERT = 0x0052,  /* TODO: Check support */
	BQ27Z855_MAC_CMD_PFSTATUS = 0x0053, /* TODO: Check support */
	BQ27Z855_MAC_CMD_OPERATIONSTATUS = 0x0054,
	BQ27Z855_MAC_CMD_CHARGINGSTATUS = 0x0055,
	BQ27Z855_MAC_CMD_GAUGINGSTATUS = 0x0056,
	BQ27Z855_MAC_CMD_MANUFACTURINGSTATUS = 0x0057,
	BQ27Z855_MAC_CMD_AFEREGISTER = 0x0058,       /* TODO: Check support */
	BQ27Z855_MAC_CMD_NOLOADREMCAP = 0x005A,      /* TODO: Check support */
	BQ27Z855_MAC_CMD_CHARGINGSTATUSEXT = 0x005E, /* TODO: Check support */
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK1 = 0x0060,
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK2 = 0x0061,
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK3 = 0x0062,
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK4 = 0x0063,
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK5 = 0x0064,
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK6 = 0x0065,
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK7 = 0x0066,
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK8 = 0x0067,
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK9 = 0x0068,
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK10 = 0x0069,
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK11 = 0x006A,
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK12 = 0x006B,
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK13 = 0x006C, /* TODO: Check support */
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK14 = 0x006D, /* TODO: Check support */
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK15 = 0x006E, /* TODO: Check support */
	BQ27Z855_MAC_CMD_POWEREVENTS = 0x006F,         /* TODO: Check support */
	BQ27Z855_MAC_CMD_MANUFACTURERINFO = 0x0070,
	BQ27Z855_MAC_CMD_DASTATUS1 = 0x0071,
	BQ27Z855_MAC_CMD_DASTATUS2 = 0x0072,
	BQ27Z855_MAC_CMD_GAUGESTATUS1 = 0x0073,
	BQ27Z855_MAC_CMD_GAUGESTATUS2 = 0x0074,
	BQ27Z855_MAC_CMD_GAUGESTATUS3 = 0x0075,
	BQ27Z855_MAC_CMD_CBSTATUS = 0x0076, /* TODO: Check support */
	BQ27Z855_MAC_CMD_FCC_SOH = 0x0077,
	BQ27Z855_MAC_CMD_FILTERCAPACITY = 0x0078,
	BQ27Z855_MAC_CMD_MANUFACTURERINFOB = 0x007A,
	BQ27Z855_MAC_CMD_MANUFACTURERINFOC = 0x007B,
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK16 = 0x007E,
	BQ27Z855_MAC_CMD_ACCUMULATIONCHARGETHRESHOLD = 0x009D,    /* TODO: Check support */
	BQ27Z855_MAC_CMD_ACCUMULATIONDISCHARGETHRESHOLD = 0x009E, /* TODO: Check support */
	BQ27Z855_MAC_CMD_ACCUMULATEDTIMECHARGE = 0x009F,          /* TODO: Check support */
	BQ27Z855_MAC_CMD_EXITCALIBRATIONOUTPUT = 0xF080,
	BQ27Z855_MAC_CMD_OUTPUTCCADCAL = 0xF081,
	BQ27Z855_MAC_CMD_OUTPUTSHORTEDCCADCAL = 0xF082,
};

/*
 * AltManufacturerAccess control commands specific to the bq27z855.
 *
 * NOTE: Departure from fuel_gauge API encapsulation — see the note in
 * bq27z8xx_common.h above bq27z8xx_mac_ctrl_cmds for the full explanation.
 * The same FUEL_GAUGE_SBS_MFR_ACCESS mechanism applies here.
 *
 * Send via: fuel_gauge_set_prop(dev, FUEL_GAUGE_SBS_MFR_ACCESS,
 *               (union fuel_gauge_prop_val){.sbs_mfr_access_word = BQ27Z855_MAC_CTRL_CMD_XXX})
 */
enum bq27z855_mac_ctrl_cmds {
	BQ27Z855_MAC_CTRL_CMD_STORAGEMODE = 0x000A, /* TODO: Check support */
	BQ27Z855_MAC_CTRL_CMD_SLEEPMODE = 0x0011,
	BQ27Z855_MAC_CTRL_CMD_FUSETOGGLE = 0x001D,                  /* TODO: Check support */
	BQ27Z855_MAC_CTRL_CMD_PERMANENTFAILURE = 0x0024,            /* TODO: Check support */
	BQ27Z855_MAC_CTRL_CMD_BLACKBOXRECORDER = 0x0025,            /* TODO: Check support */
	BQ27Z855_MAC_CTRL_CMD_PERMANENTFAILDATARESET = 0x0029,      /* TODO: Check support */
	BQ27Z855_MAC_CTRL_CMD_BLACKBOXRECORDERRESET = 0x002A,       /* TODO: Check support */
	BQ27Z855_MAC_CTRL_CMD_RSOCWRITE = 0x0079,                   /* TODO: Check support */
	BQ27Z855_MAC_CTRL_CMD_ACCUMULATIONCHARGEENABLE = 0x0098,    /* TODO: Check support */
	BQ27Z855_MAC_CTRL_CMD_ACCUMULATIONDISCHARGEENABLE = 0x0099, /* TODO: Check support */
	BQ27Z855_MAC_CTRL_CMD_ACCUMULATIONRESET = 0x009A,           /* TODO: Check support */
	BQ27Z855_MAC_CTRL_CMD_ACCUMULATIONSTOP = 0x009B,            /* TODO: Check support */
	BQ27Z855_MAC_CTRL_CMD_ACCUMULATIONSTART = 0x009C,           /* TODO: Check support */
	BQ27Z855_MAC_CTRL_CMD_IATASHUTDOWN = 0x00F0,                /* TODO: Check support */
	BQ27Z855_MAC_CTRL_CMD_IATARM = 0x00F1,                      /* TODO: Check support */
	BQ27Z855_MAC_CTRL_CMD_IATAFCC = 0x00F2,                     /* TODO: Check support */
	BQ27Z855_MAC_CTRL_CMD_ROMMODE = 0x0F00,
};

/*
 * MAC block write commands — write command + data payload via AltManufacturerAccess.
 *
 * NOTE: Departure from fuel_gauge API encapsulation.
 * The Zephyr fuel_gauge API provides fuel_gauge_get_buffer_prop() for MAC block
 * reads, but has no fuel_gauge_set_buffer_prop() counterpart. MAC block write
 * commands (which carry a multi-byte data payload) cannot be expressed through
 * the standard API. A device-specific public function, bq27z855_write_mac(), is
 * exposed instead. If a fuel_gauge_set_buffer_prop() API is added to Zephyr in
 * the future, this function should be replaced with a set_buffer_property
 * implementation.
 *
 * Use with bq27z855_write_mac(dev, BQ27Z855_MAC_WRITE_CMD_XXX, data, data_len).
 */
enum bq27z855_mac_write_cmds {
	BQ27Z855_MAC_WRITE_CMD_CHARGINVOLTAGEOVERRIDE = 0x00B0,
	BQ27Z855_MAC_WRITE_CMD_CHARGINCURRENTOVERRIDE = 0x00B2,
	BQ27Z855_MAC_WRITE_CMD_WRITETEMP = 0x3008,
};

/**
 * @brief Write data to the device via an AltManufacturerAccess block write command.
 *
 * This function exists because the Zephyr fuel_gauge API has no
 * fuel_gauge_set_buffer_prop() — see the note above bq27z855_mac_write_cmds.
 *
 * Sends the command word to register 0x3E, writes @p data_len bytes of @p data
 * to the MAC data buffer (0x40), then commits the transaction by writing the
 * checksum to 0x60 and the total length to 0x61.
 *
 * @param dev      Pointer to the bq27z855 device.
 * @param cmd      MAC write command (from bq27z855_mac_write_cmds).
 * @param data     Pointer to the data payload to write.
 * @param data_len Number of bytes in @p data (max 32).
 *
 * @return 0 on success, negative errno on failure.
 */
int bq27z855_write_mac(const struct device *dev, enum bq27z855_mac_write_cmds cmd,
		       const uint8_t *data, size_t data_len);

/*
 * Custom fuel gauge properties specific to the bq27z855.
 * These extend the common bq27z8xx_prop enum from BQ27Z8XX_PROP_END.
 *
 * For common properties (max load current, average power, alarm thresholds,
 * etc.) use the BQ27Z8XX_PROP_* and BQ27Z8XX_BUFFER_PROP_* identifiers
 * defined in bq27z8xx_common.h.
 */
enum bq27z855_prop {
	/* Active safety alert flags. Struct: bq27z855_safety_alert */
	BQ27Z855_BUFFER_PROP_SAFETYALERT = BQ27Z8XX_PROP_END,
	/* Latched safety status flags. Struct: bq27z855_safety_status */
	BQ27Z855_BUFFER_PROP_SAFETYSTATUS,
	/* Active permanent failure alert flags. Struct: bq27z855_pf_alert */
	BQ27Z855_BUFFER_PROP_PFALERT,
	/* Latched permanent failure status flags. Struct: bq27z855_pf_status */
	BQ27Z855_BUFFER_PROP_PFSTATUS,
	/* Operation status flags. Struct: bq27z855_operation_status */
	BQ27Z855_BUFFER_PROP_OPERATIONSTATUS,
	/* Charging status flags (40-bit). Struct: bq27z855_charging_status */
	BQ27Z855_BUFFER_PROP_CHARGINGSTATUS,
	/* Gauging status flags (40-bit). Struct: bq27z855_gauging_status */
	BQ27Z855_BUFFER_PROP_GAUGINGSTATUS,
	/* Manufacturing status flags. Struct: bq27z855_manufacturing_status */
	BQ27Z855_BUFFER_PROP_MANUFACTURINGSTATUS,
	/* Remaining capacity at no load (mAh). Struct: bq27z855_no_load_rem_cap */
	BQ27Z855_BUFFER_PROP_NOLOADREMCAP,
	/* Extended charging status flags. Struct: bq27z855_charging_status_ext */
	BQ27Z855_BUFFER_PROP_CHARGINGSTATUSEXT,
	/* Accumulation charge trip point threshold. Struct: bq27z855_accumulation_charge_threshold
	 */
	BQ27Z855_BUFFER_PROP_ACCUMULATIONCHARGETHRESHOLD,
	/* Accumulation discharge trip point threshold. Struct:
	 * bq27z855_accumulation_discharge_threshold
	 */
	BQ27Z855_BUFFER_PROP_ACCUMULATIONDISCHARGETHRESHOLD,
};

/*
 * Structs for bq27z855-specific MAC read buffer properties.
 * The first byte of every struct is the data length returned by the device.
 * Pass sizeof(struct) as dst_len to fuel_gauge_get_buffer_prop().
 */

struct bq27z855_safety_alert {
	uint8_t length;
	uint32_t flags;
} __packed;

struct bq27z855_safety_status {
	uint8_t length;
	uint32_t flags;
} __packed;

struct bq27z855_pf_alert {
	uint8_t length;
	uint32_t flags;
} __packed;

struct bq27z855_pf_status {
	uint8_t length;
	uint32_t flags;
} __packed;

struct bq27z855_operation_status {
	uint8_t length;
	uint32_t flags;
} __packed;

struct bq27z855_charging_status {
	uint8_t length;
	uint8_t flags[5]; /* 40-bit status field, bits 39:0 */
} __packed;

struct bq27z855_gauging_status {
	uint8_t length;
	/* TODO: hardware returns a 6-byte payload; investigate whether the extra
	 * byte is a reserved field, an extended status word, or a device-revision
	 * difference.  The TRM documents only 40 bits (5 bytes).
	 */
	uint8_t flags[6];
} __packed;

struct bq27z855_manufacturing_status {
	uint8_t length;
	uint16_t flags;
} __packed;

struct bq27z855_no_load_rem_cap {
	uint8_t length;
	uint16_t capacity_mah;
} __packed;

struct bq27z855_charging_status_ext {
	uint8_t length;
	uint32_t flags;
} __packed;

struct bq27z855_accumulation_charge_threshold {
	uint8_t length;
	int16_t threshold_mah;
} __packed;

struct bq27z855_accumulation_discharge_threshold {
	uint8_t length;
	int16_t threshold_mah;
} __packed;

/* SafetyAlert (MAC 0x0050) bits — 32-bit bq27z855_safety_alert.flags */
#define BQ27Z855_SAFETY_ALERT_ACOV  BIT(31) /* Cell overvoltage */
#define BQ27Z855_SAFETY_ALERT_ACUV  BIT(30) /* Cell undervoltage */
#define BQ27Z855_SAFETY_ALERT_OCDL  BIT(29) /* Overcurrent in discharge latch */
#define BQ27Z855_SAFETY_ALERT_COVL  BIT(28) /* Cell overvoltage latch */
#define BQ27Z855_SAFETY_ALERT_UTD   BIT(27) /* Undertemperature during discharge */
#define BQ27Z855_SAFETY_ALERT_UTC   BIT(26) /* Undertemperature during charge */
#define BQ27Z855_SAFETY_ALERT_PCHGC BIT(25) /* Over-precharge current */
#define BQ27Z855_SAFETY_ALERT_CHGV  BIT(24) /* Overcharging voltage */
#define BQ27Z855_SAFETY_ALERT_CHGC  BIT(23) /* Overcharging current */
#define BQ27Z855_SAFETY_ALERT_OC    BIT(22) /* Overcharge */
#define BQ27Z855_SAFETY_ALERT_CTOS  BIT(21) /* Charge timeout suspend */
#define BQ27Z855_SAFETY_ALERT_CTO   BIT(20) /* Charge timeout */
#define BQ27Z855_SAFETY_ALERT_PTOS  BIT(19) /* Precharge timeout suspend */
#define BQ27Z855_SAFETY_ALERT_PTO   BIT(18) /* Precharge timeout */
/* bit 17 reserved */
#define BQ27Z855_SAFETY_ALERT_OTF   BIT(16) /* Overtemperature FET */
#define BQ27Z855_SAFETY_ALERT_DCOT  BIT(15) /* Delta cell overtemperature */
#define BQ27Z855_SAFETY_ALERT_CUVC  BIT(14) /* Cell undervoltage compensated */
#define BQ27Z855_SAFETY_ALERT_OTD   BIT(13) /* Overtemperature during discharge */
#define BQ27Z855_SAFETY_ALERT_OTC   BIT(12) /* Overtemperature during charge */
#define BQ27Z855_SAFETY_ALERT_ASCDL BIT(11) /* Short-circuit during discharge latch */
/* bit 10 reserved */
#define BQ27Z855_SAFETY_ALERT_AOCCL BIT(9) /* Short-circuit during charge latch */
/* bit 8 reserved */
#define BQ27Z855_SAFETY_ALERT_AOCDL BIT(7) /* Overload during discharge latch */
/* bit 6 reserved */
#define BQ27Z855_SAFETY_ALERT_OCD2  BIT(5) /* Overcurrent during discharge 2 */
#define BQ27Z855_SAFETY_ALERT_OCD1  BIT(4) /* Overcurrent during discharge 1 */
#define BQ27Z855_SAFETY_ALERT_OCC2  BIT(3) /* Overcurrent during charge 2 */
#define BQ27Z855_SAFETY_ALERT_OCC1  BIT(2) /* Overcurrent during charge 1 */
#define BQ27Z855_SAFETY_ALERT_COV   BIT(1) /* Cell overvoltage */
#define BQ27Z855_SAFETY_ALERT_CUV   BIT(0) /* Cell undervoltage */

/*
 * SafetyStatus (MAC 0x0051) bits — 32-bit bq27z855_safety_status.flags.
 * Differs from SafetyAlert: bits 21 and 19 (CTOS, PTOS) are absent; bit 17
 * is HWDF instead of reserved; bits 10, 8, 6 (ASCD, AOCC, AOCD) are present.
 */
#define BQ27Z855_SAFETY_STATUS_ACOV  BIT(31) /* Cell overvoltage */
#define BQ27Z855_SAFETY_STATUS_ACUV  BIT(30) /* Cell undervoltage */
#define BQ27Z855_SAFETY_STATUS_OCDL  BIT(29) /* Overcurrent in discharge latch */
#define BQ27Z855_SAFETY_STATUS_COVL  BIT(28) /* Cell overvoltage latch */
#define BQ27Z855_SAFETY_STATUS_UTD   BIT(27) /* Undertemperature during discharge */
#define BQ27Z855_SAFETY_STATUS_UTC   BIT(26) /* Undertemperature during charge */
#define BQ27Z855_SAFETY_STATUS_PCHGC BIT(25) /* Over-precharge current */
#define BQ27Z855_SAFETY_STATUS_CHGV  BIT(24) /* Overcharging voltage */
#define BQ27Z855_SAFETY_STATUS_CHGC  BIT(23) /* Overcharging current */
#define BQ27Z855_SAFETY_STATUS_OC    BIT(22) /* Overcharge */
/* bit 21 reserved */
#define BQ27Z855_SAFETY_STATUS_CTO   BIT(20) /* Charge timeout */
/* bit 19 reserved */
#define BQ27Z855_SAFETY_STATUS_PTO   BIT(18) /* Precharge timeout */
#define BQ27Z855_SAFETY_STATUS_HWDF  BIT(17) /* SBS Host watchdog timeout */
#define BQ27Z855_SAFETY_STATUS_OTF   BIT(16) /* Overtemperature FET */
#define BQ27Z855_SAFETY_STATUS_DCOT  BIT(15) /* Delta cell overtemperature */
#define BQ27Z855_SAFETY_STATUS_CUVC  BIT(14) /* Cell undervoltage compensated */
#define BQ27Z855_SAFETY_STATUS_OTD   BIT(13) /* Overtemperature during discharge */
#define BQ27Z855_SAFETY_STATUS_OTC   BIT(12) /* Overtemperature during charge */
#define BQ27Z855_SAFETY_STATUS_ASCDL BIT(11) /* Short-circuit during discharge latch */
#define BQ27Z855_SAFETY_STATUS_ASCD  BIT(10) /* Short-circuit during discharge */
#define BQ27Z855_SAFETY_STATUS_AOCCL BIT(9)  /* Short-circuit during charge latch */
#define BQ27Z855_SAFETY_STATUS_AOCC  BIT(8)  /* Short-circuit during charge */
#define BQ27Z855_SAFETY_STATUS_AOCDL BIT(7)  /* Overload during discharge latch */
#define BQ27Z855_SAFETY_STATUS_AOCD  BIT(6)  /* Overload during discharge */
#define BQ27Z855_SAFETY_STATUS_OCD2  BIT(5)  /* Overcurrent during discharge 2 */
#define BQ27Z855_SAFETY_STATUS_OCD1  BIT(4)  /* Overcurrent during discharge 1 */
#define BQ27Z855_SAFETY_STATUS_OCC2  BIT(3)  /* Overcurrent during charge 2 */
#define BQ27Z855_SAFETY_STATUS_OCC1  BIT(2)  /* Overcurrent during charge 1 */
#define BQ27Z855_SAFETY_STATUS_COV   BIT(1)  /* Cell overvoltage */
#define BQ27Z855_SAFETY_STATUS_CUV   BIT(0)  /* Cell undervoltage */

/* PFAlert (MAC 0x0052) bits — 32-bit bq27z855_pf_alert.flags */
#define BQ27Z855_PF_ALERT_TS4   BIT(31) /* Open thermistor TS4 */
#define BQ27Z855_PF_ALERT_TS3   BIT(30) /* Open thermistor TS3 */
#define BQ27Z855_PF_ALERT_TS2   BIT(29) /* Open thermistor TS2 */
#define BQ27Z855_PF_ALERT_TS1   BIT(28) /* Open thermistor TS1 */
#define BQ27Z855_PF_ALERT_TMPC  BIT(27) /* TMP468 communication failure */
/* bits 26-23 reserved */
#define BQ27Z855_PF_ALERT_2LVL  BIT(22) /* Second level protector failure */
#define BQ27Z855_PF_ALERT_AFEC  BIT(21) /* AFE communication failure */
#define BQ27Z855_PF_ALERT_AFER  BIT(20) /* AFE register failure */
#define BQ27Z855_PF_ALERT_FUSE  BIT(19) /* Chemical fuse failure */
#define BQ27Z855_PF_ALERT_OCDL  BIT(18) /* Overcurrent in discharge */
#define BQ27Z855_PF_ALERT_DFETF BIT(17) /* Discharge FET failure */
#define BQ27Z855_PF_ALERT_CFETF BIT(16) /* Charge FET failure */
#define BQ27Z855_PF_ALERT_ASCDL BIT(15) /* Short circuit in discharge */
#define BQ27Z855_PF_ALERT_AOCCL BIT(14) /* Short circuit in charge */
#define BQ27Z855_PF_ALERT_AOCDL BIT(13) /* Overload in discharge */
#define BQ27Z855_PF_ALERT_VIMA  BIT(12) /* Voltage imbalance while pack active */
#define BQ27Z855_PF_ALERT_VIMR  BIT(11) /* Voltage imbalance while pack at rest */
#define BQ27Z855_PF_ALERT_CD    BIT(10) /* Capacity degradation failure */
#define BQ27Z855_PF_ALERT_IMP   BIT(9)  /* Impedance failure */
#define BQ27Z855_PF_ALERT_CB    BIT(8)  /* Cell balancing failure */
#define BQ27Z855_PF_ALERT_QIM   BIT(7)  /* QMax imbalance failure */
#define BQ27Z855_PF_ALERT_SOTF  BIT(6)  /* Safety overtemperature FET failure */
#define BQ27Z855_PF_ALERT_COVL  BIT(5)  /* Cell overvoltage latch */
#define BQ27Z855_PF_ALERT_SOT   BIT(4)  /* Safety overtemperature cell failure */
#define BQ27Z855_PF_ALERT_SOCD  BIT(3)  /* Safety overcurrent in discharge */
#define BQ27Z855_PF_ALERT_SOCC  BIT(2)  /* Safety overcurrent in charge */
#define BQ27Z855_PF_ALERT_SOV   BIT(1)  /* Safety cell overvoltage failure */
#define BQ27Z855_PF_ALERT_SUV   BIT(0)  /* Safety cell undervoltage failure */

/*
 * PFStatus (MAC 0x0053) bits — 32-bit bq27z855_pf_status.flags.
 * Differs from PFAlert at bits 26-23: DFW(26), FORCE(25), reserved(24), NTC(23).
 */
#define BQ27Z855_PF_STATUS_TS4   BIT(31) /* Open thermistor TS4 */
#define BQ27Z855_PF_STATUS_TS3   BIT(30) /* Open thermistor TS3 */
#define BQ27Z855_PF_STATUS_TS2   BIT(29) /* Open thermistor TS2 */
#define BQ27Z855_PF_STATUS_TS1   BIT(28) /* Open thermistor TS1 */
#define BQ27Z855_PF_STATUS_TMPC  BIT(27) /* TMP468 communication failure */
#define BQ27Z855_PF_STATUS_DFW   BIT(26) /* Data flash wearout failure */
#define BQ27Z855_PF_STATUS_FORCE BIT(25) /* Manual permanent failure */
/* bit 24 reserved */
#define BQ27Z855_PF_STATUS_NTC   BIT(23) /* NTC failure */
#define BQ27Z855_PF_STATUS_2LVL  BIT(22) /* Second level protector failure */
#define BQ27Z855_PF_STATUS_AFEC  BIT(21) /* AFE communication failure */
#define BQ27Z855_PF_STATUS_AFER  BIT(20) /* AFE register failure */
#define BQ27Z855_PF_STATUS_FUSE  BIT(19) /* Chemical fuse failure */
#define BQ27Z855_PF_STATUS_OCDL  BIT(18) /* Overcurrent in discharge */
#define BQ27Z855_PF_STATUS_DFETF BIT(17) /* Discharge FET failure */
#define BQ27Z855_PF_STATUS_CFETF BIT(16) /* Charge FET failure */
#define BQ27Z855_PF_STATUS_ASCDL BIT(15) /* Short circuit in discharge */
#define BQ27Z855_PF_STATUS_AOCCL BIT(14) /* Short circuit in charge */
#define BQ27Z855_PF_STATUS_AOCDL BIT(13) /* Overload in discharge */
#define BQ27Z855_PF_STATUS_VIMA  BIT(12) /* Voltage imbalance while pack active */
#define BQ27Z855_PF_STATUS_VIMR  BIT(11) /* Voltage imbalance while pack at rest */
#define BQ27Z855_PF_STATUS_CD    BIT(10) /* Capacity degradation failure */
#define BQ27Z855_PF_STATUS_IMP   BIT(9)  /* Impedance failure */
#define BQ27Z855_PF_STATUS_CB    BIT(8)  /* Cell balancing failure */
#define BQ27Z855_PF_STATUS_QIM   BIT(7)  /* QMax imbalance failure */
#define BQ27Z855_PF_STATUS_SOTF  BIT(6)  /* Safety overtemperature FET failure */
#define BQ27Z855_PF_STATUS_COVL  BIT(5)  /* Cell overvoltage latch */
#define BQ27Z855_PF_STATUS_SOT   BIT(4)  /* Safety overtemperature cell failure */
#define BQ27Z855_PF_STATUS_SOCD  BIT(3)  /* Safety overcurrent in discharge */
#define BQ27Z855_PF_STATUS_SOCC  BIT(2)  /* Safety overcurrent in charge */
#define BQ27Z855_PF_STATUS_SOV   BIT(1)  /* Safety cell overvoltage failure */
#define BQ27Z855_PF_STATUS_SUV   BIT(0)  /* Safety cell undervoltage failure */

/* OperationStatus (MAC 0x0054) bits — 32-bit bq27z855_operation_status.flags */
#define BQ27Z855_OP_STATUS_IOSHUT          BIT(31) /* IO-based shutdown */
#define BQ27Z855_OP_STATUS_PSSHUT          BIT(30) /* Power saving shutdown */
#define BQ27Z855_OP_STATUS_DISCONN         BIT(29) /* System disconnect */
/* bits 28-27 reserved */
#define BQ27Z855_OP_STATUS_STORAGEM        BIT(26) /* Storage mode */
#define BQ27Z855_OP_STATUS_SMBLCAL         BIT(25) /* Auto CC calibration */
#define BQ27Z855_OP_STATUS_INIT            BIT(24) /* Initialization after full reset */
#define BQ27Z855_OP_STATUS_SLEEPM          BIT(23) /* SLEEP mode triggered via command */
#define BQ27Z855_OP_STATUS_XL              BIT(22) /* 400-kHz SMBus mode */
#define BQ27Z855_OP_STATUS_CAL_OFFSET      BIT(21) /* Calibration output (raw CC offset data) */
#define BQ27Z855_OP_STATUS_CAL             BIT(20) /* Calibration output (raw ADC and CC data) */
#define BQ27Z855_OP_STATUS_AUTOCALM        BIT(19) /* CC auto offset calibration */
#define BQ27Z855_OP_STATUS_AUTH            BIT(18) /* Authentication in progress */
/* bit 17 reserved */
#define BQ27Z855_OP_STATUS_SDM             BIT(16) /* Shutdown triggered via command */
#define BQ27Z855_OP_STATUS_SLEEP           BIT(15) /* SLEEP mode conditions met */
#define BQ27Z855_OP_STATUS_XCHG            BIT(14) /* Charging disabled */
#define BQ27Z855_OP_STATUS_XDSG            BIT(13) /* Discharging disabled */
#define BQ27Z855_OP_STATUS_PF              BIT(12) /* Permanent failure mode */
#define BQ27Z855_OP_STATUS_SS              BIT(11) /* Safety status (OR of all SafetyStatus bits) */
#define BQ27Z855_OP_STATUS_SDV             BIT(10) /* Shutdown via low battery stack voltage */
#define BQ27Z855_OP_STATUS_SEC_SHIFT       8
#define BQ27Z855_OP_STATUS_SEC_MASK        GENMASK(9, 8) /* Security mode (2-bit field) */
#define BQ27Z855_OP_STATUS_SEC_FULL_ACCESS (0x01U << BQ27Z855_OP_STATUS_SEC_SHIFT)
#define BQ27Z855_OP_STATUS_SEC_UNSEALED    (0x02U << BQ27Z855_OP_STATUS_SEC_SHIFT)
#define BQ27Z855_OP_STATUS_SEC_SEALED      (0x03U << BQ27Z855_OP_STATUS_SEC_SHIFT)
#define BQ27Z855_OP_STATUS_BTP_INT         BIT(7) /* Battery trip point interrupt */
#define BQ27Z855_OP_STATUS_EMSHUT          BIT(6) /* Emergency FET shutdown */
#define BQ27Z855_OP_STATUS_MINSYS          BIT(5) /* MINSYS operation status */
#define BQ27Z855_OP_STATUS_ACTHR           BIT(4) /* Accumulated charge threshold reached */
/* bit 3 reserved */
#define BQ27Z855_OP_STATUS_CHG             BIT(2) /* CHG FET on */
#define BQ27Z855_OP_STATUS_DSG             BIT(1) /* DSG FET on */
#define BQ27Z855_OP_STATUS_PRES            BIT(0) /* System present */

/*
 * ChargingStatus (MAC 0x0055) bits — 40-bit field in bq27z855_charging_status.flags[5].
 * flags[0]=bits 7:0, flags[1]=bits 15:8, flags[2]=bits 23:16,
 * flags[3]=bits 31:24, flags[4]=bits 39:32.
 * Apply each mask against the appropriate flags[] byte index shown in the comment.
 */
/* Temperature range (flags[0]) */
#define BQ27Z855_CHG_STATUS_OT       BIT(6) /* flags[0]: Overtemperature region */
#define BQ27Z855_CHG_STATUS_HT       BIT(5) /* flags[0]: High temperature region */
#define BQ27Z855_CHG_STATUS_STH      BIT(4) /* flags[0]: Standard temperature high region */
#define BQ27Z855_CHG_STATUS_RT       BIT(3) /* flags[0]: Recommended temperature region */
#define BQ27Z855_CHG_STATUS_STL      BIT(2) /* flags[0]: Standard temperature low region */
#define BQ27Z855_CHG_STATUS_LT       BIT(1) /* flags[0]: Low temperature region */
#define BQ27Z855_CHG_STATUS_UT       BIT(0) /* flags[0]: Undertemperature region */
/* Voltage region and mode (flags[1]) */
#define BQ27Z855_CHG_STATUS_VCT      BIT(7) /* flags[1]: Charge termination */
#define BQ27Z855_CHG_STATUS_MCHG     BIT(6) /* flags[1]: Maintenance charge */
#define BQ27Z855_CHG_STATUS_SU       BIT(5) /* flags[1]: Suspend charge */
#define BQ27Z855_CHG_STATUS_IN       BIT(4) /* flags[1]: Charge inhibit */
#define BQ27Z855_CHG_STATUS_HV       BIT(3) /* flags[1]: High voltage region */
#define BQ27Z855_CHG_STATUS_MV       BIT(2) /* flags[1]: Mid voltage region */
#define BQ27Z855_CHG_STATUS_LV       BIT(1) /* flags[1]: Low voltage region */
#define BQ27Z855_CHG_STATUS_PV       BIT(0) /* flags[1]: Precharge voltage region */
/* Rate-of-change and mode (flags[2]) */
#define BQ27Z855_CHG_STATUS_DEG1     BIT(7) /* flags[2]: Degradation level 1 */
#define BQ27Z855_CHG_STATUS_DEG0     BIT(6) /* flags[2]: Degradation level 0 */
#define BQ27Z855_CHG_STATUS_ERETM    BIT(5) /* flags[2]: Elevated RSOC and temperature */
#define BQ27Z855_CHG_STATUS_ERM      BIT(4) /* flags[2]: Elevated RSOC mode */
#define BQ27Z855_CHG_STATUS_NCT      BIT(3) /* flags[2]: Near charge termination */
#define BQ27Z855_CHG_STATUS_CCC      BIT(2) /* flags[2]: Charging loss compensation */
#define BQ27Z855_CHG_STATUS_CVR      BIT(1) /* flags[2]: Charging voltage rate of change */
#define BQ27Z855_CHG_STATUS_CCR      BIT(0) /* flags[2]: Charging current rate of change */
/* Voltage-based status mirror (flags[3]) */
#define BQ27Z855_CHG_STATUS_V_VCT    BIT(7) /* flags[3]: Charge termination (voltage-based) */
#define BQ27Z855_CHG_STATUS_V_MCHG   BIT(6) /* flags[3]: Maintenance charge (voltage-based) */
#define BQ27Z855_CHG_STATUS_V_SU     BIT(5) /* flags[3]: Suspend charge (voltage-based) */
#define BQ27Z855_CHG_STATUS_V_IN     BIT(4) /* flags[3]: Charge inhibit (voltage-based) */
#define BQ27Z855_CHG_STATUS_V_HV     BIT(3) /* flags[3]: High voltage region (voltage-based) */
#define BQ27Z855_CHG_STATUS_V_MV     BIT(2) /* flags[3]: Mid voltage region (voltage-based) */
#define BQ27Z855_CHG_STATUS_V_LV     BIT(1) /* flags[3]: Low voltage region (voltage-based) */
#define BQ27Z855_CHG_STATUS_V_PV     BIT(0) /* flags[3]: Precharge region (voltage-based) */
/* SOC-based status mirror (flags[4]) */
#define BQ27Z855_CHG_STATUS_SOC_VCT  BIT(7) /* flags[4]: Charge termination (SOC-based) */
#define BQ27Z855_CHG_STATUS_SOC_MCHG BIT(6) /* flags[4]: Maintenance charge (SOC-based) */
#define BQ27Z855_CHG_STATUS_SOC_SU   BIT(5) /* flags[4]: Suspend charge (SOC-based) */
#define BQ27Z855_CHG_STATUS_SOC_IN   BIT(4) /* flags[4]: Charge inhibit (SOC-based) */
#define BQ27Z855_CHG_STATUS_SOC_HV   BIT(3) /* flags[4]: High voltage region (SOC-based) */
#define BQ27Z855_CHG_STATUS_SOC_MV   BIT(2) /* flags[4]: Mid voltage region (SOC-based) */
#define BQ27Z855_CHG_STATUS_SOC_LV   BIT(1) /* flags[4]: Low voltage region (SOC-based) */
#define BQ27Z855_CHG_STATUS_SOC_PV   BIT(0) /* flags[4]: Precharge region (SOC-based) */

/*
 * GaugingStatus (MAC 0x0056) bits — 40-bit field in bq27z855_gauging_status.flags[5].
 * flags[0]=bits 7:0, flags[1]=bits 15:8, flags[2]=bits 23:16,
 * flags[3]=bits 31:24, flags[4]=bits 39:32 (all reserved).
 * Apply each mask against the appropriate flags[] byte index shown in the comment.
 */
/* flags[0] */
#define BQ27Z855_GAUGING_STATUS_CF        BIT(7) /* flags[0]: Condition flag */
#define BQ27Z855_GAUGING_STATUS_DSG       BIT(6) /* flags[0]: Discharge/relax */
#define BQ27Z855_GAUGING_STATUS_EDV       BIT(5) /* flags[0]: End-of-discharge voltage */
#define BQ27Z855_GAUGING_STATUS_BAL_EN    BIT(4) /* flags[0]: Cell balancing */
#define BQ27Z855_GAUGING_STATUS_TC        BIT(3) /* flags[0]: Terminate charge */
#define BQ27Z855_GAUGING_STATUS_TD        BIT(2) /* flags[0]: Terminate discharge */
#define BQ27Z855_GAUGING_STATUS_FC        BIT(1) /* flags[0]: Fully charged */
#define BQ27Z855_GAUGING_STATUS_FD        BIT(0) /* flags[0]: Fully discharged */
/* flags[1] */
#define BQ27Z855_GAUGING_STATUS_NSFM      BIT(7) /* flags[1]: Negative scale factor mode */
#define BQ27Z855_GAUGING_STATUS_OCVPRED   BIT(6) /* flags[1]: OCV predicted */
#define BQ27Z855_GAUGING_STATUS_SLPQMax   BIT(5) /* flags[1]: OCV update in SLEEP mode */
#define BQ27Z855_GAUGING_STATUS_QEN       BIT(4) /* flags[1]: Impedance Track gauging enabled */
#define BQ27Z855_GAUGING_STATUS_VOK       BIT(3) /* flags[1]: Voltages OK for QMax update */
#define BQ27Z855_GAUGING_STATUS_R_DIS     BIT(2) /* flags[1]: Resistance updates disabled */
/* bit 1 of flags[1] reserved */
#define BQ27Z855_GAUGING_STATUS_REST      BIT(0) /* flags[1]: Rest */
/* flags[2] */
/* bits 7:6 of flags[2] reserved */
#define BQ27Z855_GAUGING_STATUS_VLB       BIT(5) /* flags[2]: Very low battery warning */
#define BQ27Z855_GAUGING_STATUS_OCVFR     BIT(4) /* flags[2]: OCV in flat region (RELAX) */
#define BQ27Z855_GAUGING_STATUS_LDMD      BIT(3) /* flags[2]: LOAD mode */
#define BQ27Z855_GAUGING_STATUS_RX        BIT(2) /* flags[2]: Resistance update */
#define BQ27Z855_GAUGING_STATUS_QMax      BIT(1) /* flags[2]: QMax update */
#define BQ27Z855_GAUGING_STATUS_VDQ       BIT(0) /* flags[2]: Discharge qualified for learning */
/* flags[3] */
/* bits 7:1 of flags[3] reserved */
#define BQ27Z855_GAUGING_STATUS_PERF_MODE BIT(0) /* flags[3]: Performance mode */
/* flags[4]: all reserved */

/* ManufacturingStatus (MAC 0x0057) bits — 16-bit bq27z855_manufacturing_status.flags */
#define BQ27Z855_MFG_STATUS_CAL_EN   BIT(15) /* Calibration mode enabled */
#define BQ27Z855_MFG_STATUS_LT_TEST  BIT(14) /* Lifetime data speed-up mode */
/* bits 13-9 reserved */
#define BQ27Z855_MFG_STATUS_FUSE_EN  BIT(8) /* Fuse action enabled */
#define BQ27Z855_MFG_STATUS_BBR_EN   BIT(7) /* Black box recorder enabled */
#define BQ27Z855_MFG_STATUS_PF_EN    BIT(6) /* Permanent failure enabled */
#define BQ27Z855_MFG_STATUS_LF_EN    BIT(5) /* Lifetime data collection enabled */
#define BQ27Z855_MFG_STATUS_FET_EN   BIT(4) /* All FET action enabled */
#define BQ27Z855_MFG_STATUS_GAUGE_EN BIT(3) /* Gas gauging enabled */
#define BQ27Z855_MFG_STATUS_DSG_EN   BIT(2) /* Discharge FET test */
#define BQ27Z855_MFG_STATUS_CHG_EN   BIT(1) /* Charge FET test */
#define BQ27Z855_MFG_STATUS_PCHG_EN  BIT(0) /* Precharge FET test */

/* Variant ops table — defined in bq27z855.c */
extern const struct bq27z8xx_variant_ops bq27z855_variant_ops;

#endif /* ZEPHYR_DRIVERS_FUELGAUGE_BQ27Z8XX_BQ27Z855_H_ */
