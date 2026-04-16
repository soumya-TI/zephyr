/*
 * Copyright (c) 2023, ithinx GmbH
 * Copyright (c) 2023, Tonies GmbH
 * Copyright (c) 2026, Texas Instruments Incorporated
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_FUELGAUGE_BQ27Z8XX_BQ27Z758_H_
#define ZEPHYR_DRIVERS_FUELGAUGE_BQ27Z8XX_BQ27Z758_H_

#include "bq27z8xx_common.h"

/**
 * @brief AltManufacturerAccess read commands specific to the bq27z758.
 *
 * Commands common to both bq27z758 and bq27z855 are in bq27z8xx_mac_read_cmds.
 * Use these with bq27z8xx_read_mac().
 */
enum bq27z758_mac_read_cmds {
	BQ27Z758_MAC_CMD_PREV_MACWRITE = 0x0007,
	BQ27Z758_MAC_CMD_DEVICE_NAME = 0x004A,
	BQ27Z758_MAC_CMD_DEVICE_CHEM = 0x004B,
	BQ27Z758_MAC_CMD_MANUFACTURER_NAME = 0x004C,
	BQ27Z758_MAC_CMD_MANUFACTURE_DATE = 0x004D,
	BQ27Z758_MAC_CMD_SERIAL_NUMBER = 0x004E,
	BQ27Z758_MAC_CMD_SAFETYALERT = 0x0050,
	BQ27Z758_MAC_CMD_SAFETYSTATUS = 0x0051,
	BQ27Z758_MAC_CMD_PFALERT = 0x0052,
	BQ27Z758_MAC_CMD_PFSTATUS = 0x0053,
	BQ27Z758_MAC_CMD_OPERATIONSTATUS = 0x0054,
	BQ27Z758_MAC_CMD_CHARGINGSTATUS = 0x0055,
	BQ27Z758_MAC_CMD_GAUGINGSTATUS = 0x0056,
	BQ27Z758_MAC_CMD_MANUFACTURINGSTATUS = 0x0057,
	BQ27Z758_MAC_CMD_LIFETIMEDATABLOCK1 = 0x0060,
	BQ27Z758_MAC_CMD_LIFETIMEDATABLOCK2 = 0x0061,
	BQ27Z758_MAC_CMD_LIFETIMEDATABLOCK3 = 0x0062,
	BQ27Z758_MAC_CMD_LIFETIMEDATABLOCK4 = 0x0063,
	BQ27Z758_MAC_CMD_LIFETIMEDATABLOCK5 = 0x0064,
	BQ27Z758_MAC_CMD_LIFETIMEDATABLOCK6 = 0x0065,
	BQ27Z758_MAC_CMD_LIFETIMEDATABLOCK7 = 0x0066,
	BQ27Z758_MAC_CMD_LIFETIMEDATABLOCK8 = 0x0067,
	BQ27Z758_MAC_CMD_LIFETIMEDATABLOCK9 = 0x0068,
	BQ27Z758_MAC_CMD_LIFETIMEDATABLOCK10 = 0x0069,
	BQ27Z758_MAC_CMD_LIFETIMEDATABLOCK11 = 0x006A,
	BQ27Z758_MAC_CMD_LIFETIMEDATABLOCK12 = 0x006B,
	BQ27Z758_MAC_CMD_LIFETIMEDATABLOCK13 = 0x006C,
	BQ27Z758_MAC_CMD_MANUFACTURERINFO = 0x0070,
	BQ27Z758_MAC_CMD_DASTATUS1 = 0x0071,
	BQ27Z758_MAC_CMD_DASTATUS2 = 0x0072,
	BQ27Z758_MAC_CMD_ITSTATUS1 = 0x0073,
	BQ27Z758_MAC_CMD_ITSTATUS2 = 0x0074,
	BQ27Z758_MAC_CMD_ITSTATUS3 = 0x0075,
	BQ27Z758_MAC_CMD_FCC_SOH = 0x0077,
	BQ27Z758_MAC_CMD_FILTERED_CAPACITY = 0x0078,
	BQ27Z758_MAC_CMD_MANUFACTURERINFOB = 0x007A,
	BQ27Z758_MAC_CMD_MANUFACTURERINFOC = 0x007B,
	BQ27Z758_MAC_CMD_FET_CONTROL_OVERRIDE = 0x0097,
	BQ27Z758_MAC_CMD_SYSTEM_RESET_ENABLE = 0x00A3,
	BQ27Z758_MAC_CMD_SYSTEM_RESET = 0x00A4,
	BQ27Z758_MAC_CMD_BATTSENSEOUTPUT = 0x00B1,
	BQ27Z758_MAC_CMD_RATABLECELL0 = 0x00E0,
	BQ27Z758_MAC_CMD_DATAFLASHACCESS = 0x4000,
	BQ27Z758_MAC_CMD_SWITCHTOHDQ = 0x7C40,
	BQ27Z758_MAC_CMD_EXITCALIBRATIONOUTPUT = 0xF080,
	BQ27Z758_MAC_CMD_OUTPUTCCANDADCFORCALIBRATIO = 0xF081,
	BQ27Z758_MAC_CMD_OUTPUTTEMPERATURECAL = 0xF083,
	BQ27Z758_MAC_CMD_PROTECTORCALIBRATION = 0xF0A0,
	BQ27Z758_MAC_CMD_PROTECTORIMAGE1 = 0xF0A1,
	BQ27Z758_MAC_CMD_PROTECTORIMAGE2 = 0xF0A2,
	BQ27Z758_MAC_CMD_PROTECTORIMAGESAVE = 0xF0A3,
	BQ27Z758_MAC_CMD_PROTECTORIMAGELOCK = 0xF0A4,
	BQ27Z758_MAC_CMD_PROTECTORFACTORYCONFIG = 0xF0A5,
};

/*
 * AltManufacturerAccess control commands specific to the bq27z758.
 *
 * NOTE: Departure from fuel_gauge API encapsulation — see the note in
 * bq27z8xx_common.h above bq27z8xx_mac_ctrl_cmds for the full explanation.
 * The same FUEL_GAUGE_SBS_MFR_ACCESS mechanism applies here.
 *
 * Send via: fuel_gauge_set_prop(dev, FUEL_GAUGE_SBS_MFR_ACCESS,
 *               (union fuel_gauge_prop_val){.sbs_mfr_access_word = BQ27Z758_MAC_CTRL_CMD_XXX})
 */
enum bq27z758_mac_ctrl_cmds {
	BQ27Z758_MAC_CTRL_CMD_SHELFENABLE = 0x000B,
	BQ27Z758_MAC_CTRL_CMD_SHELFDISABLE = 0x000C,
	BQ27Z758_MAC_CTRL_CMD_RESET1 = 0x0012,
	BQ27Z758_MAC_CTRL_CMD_SHIPMODEENABLE = 0x0015,
	BQ27Z758_MAC_CTRL_CMD_SHIPMODEDISABLE = 0x0016,
	BQ27Z758_MAC_CTRL_CMD_QMAX_DAY = 0x0017,
	BQ27Z758_MAC_CTRL_CMD_TAMBIENTSYNC = 0x0047,
	BQ27Z758_MAC_CTRL_CMD_ROMMODE = 0x0F00,
};

/*
 * Custom fuel gauge properties specific to the bq27z758.
 * These extend the common bq27z8xx_prop enum from BQ27Z8XX_PROP_END.
 *
 * For common properties (max load current, average power, alarm thresholds,
 * etc.) use the BQ27Z8XX_PROP_* and BQ27Z8XX_BUFFER_PROP_* identifiers
 * defined in bq27z8xx_common.h.
 *
 * Note: FUEL_GAUGE_MANUFACTURER_NAME, FUEL_GAUGE_DEVICE_NAME, and
 * FUEL_GAUGE_DEVICE_CHEMISTRY are standard fuel_gauge buffer properties
 * supported by this driver via MAC commands. They do not need custom IDs.
 */
enum bq27z758_prop {
	/** Active safety alert flags. Struct: bq27z758_safety_alert */
	BQ27Z758_BUFFER_PROP_SAFETYALERT = BQ27Z8XX_PROP_END,
	/** Latched safety status flags. Struct: bq27z758_safety_status */
	BQ27Z758_BUFFER_PROP_SAFETYSTATUS,
	/** Active permanent failure alert flags. Struct: bq27z758_pf_alert */
	BQ27Z758_BUFFER_PROP_PFALERT,
	/** Latched permanent failure status flags. Struct: bq27z758_pf_status */
	BQ27Z758_BUFFER_PROP_PFSTATUS,
	/** Operation status (A and B words). Struct: bq27z758_operation_status */
	BQ27Z758_BUFFER_PROP_OPERATIONSTATUS,
	/** Charging status flags. Struct: bq27z758_charging_status */
	BQ27Z758_BUFFER_PROP_CHARGINGSTATUS,
	/** Gauging status flags (24-bit). Struct: bq27z758_gauging_status */
	BQ27Z758_BUFFER_PROP_GAUGINGSTATUS,
	/** Manufacturing status flags. Struct: bq27z758_manufacturing_status */
	BQ27Z758_BUFFER_PROP_MANUFACTURINGSTATUS,
};

/*
 * Structs for bq27z758-specific MAC read buffer properties.
 * The first byte of every struct is the data length returned by the device.
 * Pass sizeof(struct) as dst_len to fuel_gauge_get_buffer_prop().
 *
 * Note: bq27z758 status register layouts differ from bq27z855 — in particular
 * ChargingStatus is 24-bit, GaugingStatus is 24-bit, and OperationStatus is
 * split into two 16-bit words (A and B).
 */

struct bq27z758_safety_alert {
	uint8_t length;
	uint32_t status;
} __packed;

struct bq27z758_safety_status {
	uint8_t length;
	uint32_t status;
} __packed;

struct bq27z758_pf_alert {
	uint8_t length;
	uint32_t status;
} __packed;

struct bq27z758_pf_status {
	uint8_t length;
	uint32_t status;
} __packed;

struct bq27z758_operation_status {
	uint8_t length;
	uint16_t status_a; /* OperationStatus A, bits 15:0 */
	uint16_t status_b; /* OperationStatus B, bits 31:16 */
} __packed;

struct bq27z758_charging_status {
	uint8_t length;
	uint8_t temp_range; /* Temperature range flags, bits 6:0 */
	uint16_t status;    /* Charging status flags, bits 11:0 */
} __packed;

struct bq27z758_gauging_status {
	uint8_t length;
	uint8_t status[3]; /* 24-bit status field, bits 23:0 */
} __packed;

struct bq27z758_manufacturing_status {
	uint8_t length;
	uint16_t status;
} __packed;

/* SafetyAlert (MAC 0x0050) bits — 32-bit bq27z758_safety_alert.status */
#define BQ27Z758_SAFETY_ALERT_UTD  BIT(27) /* Undertemperature during discharge */
#define BQ27Z758_SAFETY_ALERT_UTC  BIT(26) /* Undertemperature during charge */
#define BQ27Z758_SAFETY_ALERT_CTOS BIT(21) /* Charge timeout suspend */
#define BQ27Z758_SAFETY_ALERT_PTOS BIT(19) /* Precharge timeout suspend */
#define BQ27Z758_SAFETY_ALERT_OTF  BIT(16) /* Overtemperature FET */
#define BQ27Z758_SAFETY_ALERT_OTD  BIT(13) /* Overtemperature during discharge */
#define BQ27Z758_SAFETY_ALERT_OTC  BIT(12) /* Overtemperature during charge */
#define BQ27Z758_SAFETY_ALERT_OCD  BIT(4)  /* Overcurrent during discharge */
#define BQ27Z758_SAFETY_ALERT_OCC  BIT(2)  /* Overcurrent during charge */
#define BQ27Z758_SAFETY_ALERT_COV  BIT(1)  /* Cell overvoltage */
#define BQ27Z758_SAFETY_ALERT_CUV  BIT(0)  /* Cell undervoltage */

/* SafetyStatus (MAC 0x0051) bits — 32-bit bq27z758_safety_status.status */
#define BQ27Z758_SAFETY_STATUS_HCUV BIT(31) /* Heartbeat cell undervoltage */
#define BQ27Z758_SAFETY_STATUS_HCOV BIT(30) /* Heartbeat cell overvoltage */
#define BQ27Z758_SAFETY_STATUS_UTD  BIT(27) /* Undertemperature during discharge */
#define BQ27Z758_SAFETY_STATUS_UTC  BIT(26) /* Undertemperature during charge */
#define BQ27Z758_SAFETY_STATUS_CTO  BIT(20) /* Charge timeout */
#define BQ27Z758_SAFETY_STATUS_PTO  BIT(18) /* Precharge timeout */
#define BQ27Z758_SAFETY_STATUS_OTF  BIT(16) /* Overtemperature FET */
#define BQ27Z758_SAFETY_STATUS_OTD  BIT(13) /* Overtemperature during discharge */
#define BQ27Z758_SAFETY_STATUS_OTC  BIT(12) /* Overtemperature during charge */
#define BQ27Z758_SAFETY_STATUS_HSCD BIT(10) /* Short-circuit during discharge */
#define BQ27Z758_SAFETY_STATUS_HOCC BIT(8)  /* Short-circuit during charge */
#define BQ27Z758_SAFETY_STATUS_HOCD BIT(6)  /* Overload during discharge */
#define BQ27Z758_SAFETY_STATUS_OCD  BIT(4)  /* Overcurrent during discharge */
#define BQ27Z758_SAFETY_STATUS_OCC  BIT(2)  /* Overcurrent during charge */
#define BQ27Z758_SAFETY_STATUS_COV  BIT(1)  /* Cell overvoltage */
#define BQ27Z758_SAFETY_STATUS_CUV  BIT(0)  /* Cell undervoltage */

/*
 * PFAlert/PFStatus (MAC 0x0052 / 0x0053) bits — 32-bit status.
 * The register is organized as two 16-bit word pairs (A+B and C+D);
 * the defines below cover the bits relevant to each pair.
 */
#define BQ27Z758_PF_ALERT_SOV   BIT(1) /* Safety cell overvoltage (A+B word) */
#define BQ27Z758_PF_ALERT_SUV   BIT(0) /* Safety cell undervoltage (A+B word) */
#define BQ27Z758_PF_ALERT_DFETF BIT(1) /* Discharge FET failure (C+D word) */
#define BQ27Z758_PF_ALERT_CFETF BIT(0) /* Charge FET failure (C+D word) */

/* OperationStatus A (MAC 0x0054) bits — 16-bit bq27z758_operation_status.status_a */
#define BQ27Z758_OP_STATUS_A_SLEEP     BIT(15) /* SLEEP mode conditions met */
#define BQ27Z758_OP_STATUS_A_XCHG      BIT(14) /* Charging disabled */
#define BQ27Z758_OP_STATUS_A_XDSG      BIT(13) /* Discharging disabled */
#define BQ27Z758_OP_STATUS_A_PF        BIT(12) /* Permanent failure mode */
#define BQ27Z758_OP_STATUS_A_SS        BIT(11) /* Safety status (OR of all SafetyStatus bits) */
#define BQ27Z758_OP_STATUS_A_SDV       BIT(10) /* Shutdown via low battery stack voltage */
#define BQ27Z758_OP_STATUS_A_SEC_SHIFT 8
#define BQ27Z758_OP_STATUS_A_SEC_MASK  GENMASK(9, 8) /* Security mode (2-bit field) */
#define BQ27Z758_OP_STATUS_A_BTP_INT   BIT(7)        /* Battery trip point interrupt */
#define BQ27Z758_OP_STATUS_A_SHELF     BIT(6)        /* Shelf mode conditions met */
#define BQ27Z758_OP_STATUS_A_SHIP      BIT(4)        /* SHIP mode conditions met */
#define BQ27Z758_OP_STATUS_A_ZVCHG     BIT(3)        /* Zero-volt charge active */
#define BQ27Z758_OP_STATUS_A_CHG       BIT(2)        /* CHG FET on */
#define BQ27Z758_OP_STATUS_A_DSG       BIT(1)        /* DSG FET on */
#define BQ27Z758_OP_STATUS_A_SHIPV     BIT(0)        /* SHIPV mode active */

/* OperationStatus B (MAC 0x0054) bits — 16-bit bq27z758_operation_status.status_b */
#define BQ27Z758_OP_STATUS_B_SHELFV BIT(15) /* Shelf mode voltage conditions met */
#define BQ27Z758_OP_STATUS_B_SHIPM  BIT(14) /* Ship mode triggered via command */
#define BQ27Z758_OP_STATUS_B_SLPAD  BIT(10) /* Sleep and pad active */
#define BQ27Z758_OP_STATUS_B_INIT   BIT(8)  /* Initialization after full reset */
#define BQ27Z758_OP_STATUS_B_SHELFM BIT(7)  /* Shelf mode triggered via command */
#define BQ27Z758_OP_STATUS_B_XL     BIT(6)  /* 400-kHz SMBus mode */
#define BQ27Z758_OP_STATUS_B_CAL    BIT(4)  /* Calibration output active */
#define BQ27Z758_OP_STATUS_B_AUTH   BIT(2)  /* Authentication in progress */
#define BQ27Z758_OP_STATUS_B_SDM    BIT(0)  /* Shutdown triggered via command */

/*
 * ChargingStatus (MAC 0x0055) bits — 24-bit field.
 * temp_range (bq27z758_charging_status.temp_range): bits 6:0
 * status     (bq27z758_charging_status.status):     bits 11:0
 */
/* Temperature range (temp_range byte) */
#define BQ27Z758_CHG_TEMP_RANGE_OT        BIT(6) /* Overtemperature region */
#define BQ27Z758_CHG_TEMP_RANGE_HT        BIT(5) /* High temperature region */
#define BQ27Z758_CHG_TEMP_RANGE_STH       BIT(4) /* Standard temperature high region */
#define BQ27Z758_CHG_TEMP_RANGE_RT        BIT(3) /* Recommended temperature region */
#define BQ27Z758_CHG_TEMP_RANGE_STL       BIT(2) /* Standard temperature low region */
#define BQ27Z758_CHG_TEMP_RANGE_LT        BIT(1) /* Low temperature region */
#define BQ27Z758_CHG_TEMP_RANGE_UT        BIT(0) /* Undertemperature region */
/* Charging status (status word) */
#define BQ27Z758_CHG_STATUS_NCT           BIT(11) /* Near charge termination */
#define BQ27Z758_CHG_STATUS_CV_DGRD_SHIFT 8
#define BQ27Z758_CHG_STATUS_CV_DGRD_MASK  GENMASK(9, 8) /* CV degradation level (2-bit) */
#define BQ27Z758_CHG_STATUS_VCT           BIT(7)        /* Charge termination */
#define BQ27Z758_CHG_STATUS_SU            BIT(5)        /* Suspend charge */
#define BQ27Z758_CHG_STATUS_IN            BIT(4)        /* Charge inhibit */
#define BQ27Z758_CHG_STATUS_HV            BIT(3)        /* High voltage region */
#define BQ27Z758_CHG_STATUS_MV            BIT(2)        /* Mid voltage region */
#define BQ27Z758_CHG_STATUS_LV            BIT(1)        /* Low voltage region */
#define BQ27Z758_CHG_STATUS_PV            BIT(0)        /* Precharge voltage region */

/*
 * GaugingStatus (MAC 0x0056) bits — 24-bit field in bq27z758_gauging_status.status[3].
 * status[0]=bits 7:0, status[1]=bits 15:8, status[2]=bits 23:16.
 * Apply each mask against the appropriate flags[] byte index shown in the comment.
 */
#define BQ27Z758_GAUGING_STATUS_QMAXDODOK BIT(21) /* flags[2]: QMax DOD OK */
#define BQ27Z758_GAUGING_STATUS_OCVFR     BIT(20) /* flags[2]: OCV in flat region */
#define BQ27Z758_GAUGING_STATUS_LDMD      BIT(19) /* flags[2]: LOAD mode */
#define BQ27Z758_GAUGING_STATUS_RX        BIT(18) /* flags[2]: Resistance update */
#define BQ27Z758_GAUGING_STATUS_QMAX      BIT(17) /* flags[2]: QMax update */
#define BQ27Z758_GAUGING_STATUS_VDQ       BIT(16) /* flags[2]: Discharge qualified for learning */
#define BQ27Z758_GAUGING_STATUS_NSFM      BIT(15) /* flags[1]: Negative scale factor mode */
#define BQ27Z758_GAUGING_STATUS_OCVPRED   BIT(14) /* flags[1]: OCV predicted */
#define BQ27Z758_GAUGING_STATUS_SLPQMAX   BIT(13) /* flags[1]: OCV update in SLEEP mode */
#define BQ27Z758_GAUGING_STATUS_QEN       BIT(12) /* flags[1]: Impedance Track gauging enabled */
#define BQ27Z758_GAUGING_STATUS_VOK       BIT(11) /* flags[1]: Voltages OK for QMax update */
#define BQ27Z758_GAUGING_STATUS_RDIS      BIT(10) /* flags[1]: Resistance updates disabled */
#define BQ27Z758_GAUGING_STATUS_REST      BIT(8)  /* flags[1]: Rest */
#define BQ27Z758_GAUGING_STATUS_DSG       BIT(6)  /* flags[0]: Discharge/relax */
#define BQ27Z758_GAUGING_STATUS_EDV       BIT(5)  /* flags[0]: End-of-discharge voltage */
#define BQ27Z758_GAUGING_STATUS_TC        BIT(3)  /* flags[0]: Terminate charge */
#define BQ27Z758_GAUGING_STATUS_TD        BIT(2)  /* flags[0]: Terminate discharge */
#define BQ27Z758_GAUGING_STATUS_FC        BIT(1)  /* flags[0]: Fully charged */
#define BQ27Z758_GAUGING_STATUS_FD        BIT(0)  /* flags[0]: Fully discharged */

/* ManufacturingStatus (MAC 0x0057) bits — 16-bit bq27z758_manufacturing_status.status */
#define BQ27Z758_MFG_STATUS_CAL_EN   BIT(15) /* Calibration mode enabled */
#define BQ27Z758_MFG_STATUS_LT_TEST  BIT(14) /* Lifetime data speed-up mode */
#define BQ27Z758_MFG_STATUS_FET_OVRD BIT(8)  /* FET override active */
#define BQ27Z758_MFG_STATUS_PF_EN    BIT(6)  /* Permanent failure enabled */
#define BQ27Z758_MFG_STATUS_LF_EN    BIT(5)  /* Lifetime data collection enabled */
#define BQ27Z758_MFG_STATUS_FET_EN   BIT(4)  /* All FET action enabled */
#define BQ27Z758_MFG_STATUS_GAUGE_EN BIT(3)  /* Gas gauging enabled */
#define BQ27Z758_MFG_STATUS_DSG_TEST BIT(2)  /* Discharge FET test */
#define BQ27Z758_MFG_STATUS_CHG_TEST BIT(1)  /* Charge FET test */

/* Variant ops table — defined in bq27z758.c */
extern const struct bq27z8xx_variant_ops bq27z758_variant_ops;

/* ── Backward-compatibility aliases ──
 *
 * Code written against the legacy standalone bq27z758 driver compiles
 * unchanged after replacing:
 *   #include <drivers/fuel_gauge_bq27z758.h>
 * with:
 *   #include "bq27z758.h"   (with the bq27z8xx driver dir on the include path)
 *
 * All aliases are compile-time #defines; no binary overhead is introduced.
 * The two remaining Kconfig renames (CONFIG_EMUL_BQ27Z758 →
 * CONFIG_EMUL_BQ27Z8XX, CONFIG_BQ27Z758_SHELL → CONFIG_BQ27Z8XX_SHELL) must
 * be updated manually in prj.conf.
 */

/* Register constants (bq27z758_regs → bq27z8xx_regs) */
#define BQ27Z758_MANUFACTURERACCESS    BQ27Z8XX_MANUFACTURERACCESS
#define BQ27Z758_ATRATE                BQ27Z8XX_ATRATE
#define BQ27Z758_ATRATETIMETOEMPTY     BQ27Z8XX_ATRATETIMETOEMPTY
#define BQ27Z758_TEMPERATURE           BQ27Z8XX_TEMPERATURE
#define BQ27Z758_VOLTAGE               BQ27Z8XX_VOLTAGE
#define BQ27Z758_BATTERYSTATUS         BQ27Z8XX_BATTERYSTATUS
#define BQ27Z758_CURRENT               BQ27Z8XX_CURRENT
#define BQ27Z758_REMAININGCAPACITY     BQ27Z8XX_REMAININGCAPACITY
#define BQ27Z758_FULLCHARGECAPACITY    BQ27Z8XX_FULLCHARGECAPACITY
#define BQ27Z758_AVERAGECURRENT        BQ27Z8XX_AVERAGECURRENT
#define BQ27Z758_AVERAGETIMETOEMPTY    BQ27Z8XX_AVERAGETIMETOEMPTY
#define BQ27Z758_AVERAGETIMETOFULL     BQ27Z8XX_AVERAGETIMETOFULL
#define BQ27Z758_MAXLOADCURRENT        BQ27Z8XX_MAXLOADCURRENT
#define BQ27Z758_MAXLOADTIMETOEMPTY    BQ27Z8XX_MAXLOADTIMETOEMPTY
#define BQ27Z758_AVERAGEPOWER          BQ27Z8XX_AVERAGEPOWER
#define BQ27Z758_BTPDISCHARGESET       BQ27Z8XX_BTPDISCHARGESET
#define BQ27Z758_BTPCHARGESET          BQ27Z8XX_BTPCHARGESET
#define BQ27Z758_INTERNALTEMPERATURE   BQ27Z8XX_INTERNALTEMPERATURE
#define BQ27Z758_CYCLECOUNT            BQ27Z8XX_CYCLECOUNT
#define BQ27Z758_RELATIVESTATEOFCHARGE BQ27Z8XX_RELATIVESTATEOFCHARGE
#define BQ27Z758_STATEOFHEALTH         BQ27Z8XX_STATEOFHEALTH
#define BQ27Z758_CHARGINGVOLTAGE       BQ27Z8XX_CHARGINGVOLTAGE
#define BQ27Z758_CHARGINGCURRENT       BQ27Z8XX_CHARGINGCURRENT
#define BQ27Z758_TERMINATEVOLTAGE      BQ27Z8XX_TERMINATEVOLTAGE
#define BQ27Z758_TIMESTAMPUPPER        BQ27Z8XX_TIMESTAMPUPPER
#define BQ27Z758_TIMESTAMPLOWER        BQ27Z8XX_TIMESTAMPLOWER
#define BQ27Z758_QMAXCYCLES            BQ27Z8XX_QMAXCYCLES
#define BQ27Z758_DESIGNCAPACITY        BQ27Z8XX_DESIGNCAPACITY
#define BQ27Z758_ALTMANUFACTURERACCESS BQ27Z8XX_ALTMANUFACTURERACCESS
#define BQ27Z758_MACDATA               BQ27Z8XX_MACDATA
#define BQ27Z758_MACDATASUM            BQ27Z8XX_MACDATASUM
#define BQ27Z758_MACDATALEN            BQ27Z8XX_MACDATALEN
#define BQ27Z758_VOLTHISETTHRESHOLD    BQ27Z8XX_VOLTHISETTHRESHOLD
#define BQ27Z758_VOLTHICLEARTHRESHOLD  BQ27Z8XX_VOLTHICLEARTHRESHOLD
#define BQ27Z758_VOLTLOSETTHRESHOLD    BQ27Z8XX_VOLTLOSETTHRESHOLD
#define BQ27Z758_VOLTLOCLEARTHRESHOLD  BQ27Z8XX_VOLTLOCLEARTHRESHOLD
#define BQ27Z758_TEMPHISETTHRESHOLD    BQ27Z8XX_TEMPHISETTHRESHOLD
#define BQ27Z758_TEMPHICLEARTHRESHOLD  BQ27Z8XX_TEMPHICLEARTHRESHOLD
#define BQ27Z758_TEMPLOSETTHRESHOLD    BQ27Z8XX_TEMPLOSETTHRESHOLD
#define BQ27Z758_TEMPLOCLEARTHRESHOLD  BQ27Z8XX_TEMPLOCLEARTHRESHOLD
#define BQ27Z758_INTERRUPTSTATUS       BQ27Z8XX_INTERRUPTSTATUS
#define BQ27Z758_SOCDELTASETTHRESHOLD  BQ27Z8XX_SOCDELTASETTHRESHOLD

/* BatteryStatus / InterruptStatus bits (moved to common header) */
#define BQ27Z758_BATTERY_STATUS_TCA         BQ27Z8XX_BATTERY_STATUS_TCA
#define BQ27Z758_BATTERY_STATUS_TDA         BQ27Z8XX_BATTERY_STATUS_TDA
#define BQ27Z758_BATTERY_STATUS_RCA         BQ27Z8XX_BATTERY_STATUS_RCA
#define BQ27Z758_BATTERY_STATUS_INIT        BQ27Z8XX_BATTERY_STATUS_INIT
#define BQ27Z758_BATTERY_STATUS_DSG         BQ27Z8XX_BATTERY_STATUS_DSG
#define BQ27Z758_BATTERY_STATUS_FC          BQ27Z8XX_BATTERY_STATUS_FC
#define BQ27Z758_BATTERY_STATUS_FD          BQ27Z8XX_BATTERY_STATUS_FD
#define BQ27Z758_INTERRUPT_STATUS_SOC_DELTA BQ27Z8XX_INTERRUPT_STATUS_SOC_DELTA
#define BQ27Z758_INTERRUPT_STATUS_TEMP_LO   BQ27Z8XX_INTERRUPT_STATUS_TEMP_LO
#define BQ27Z758_INTERRUPT_STATUS_TEMP_HI   BQ27Z8XX_INTERRUPT_STATUS_TEMP_HI
#define BQ27Z758_INTERRUPT_STATUS_VOLT_LO   BQ27Z8XX_INTERRUPT_STATUS_VOLT_LO
#define BQ27Z758_INTERRUPT_STATUS_VOLT_HI   BQ27Z8XX_INTERRUPT_STATUS_VOLT_HI

/* MAC commands: common read (prefix BQ27Z758_ → BQ27Z8XX_) */
#define BQ27Z758_MAC_CMD_DEVICETYPE            BQ27Z8XX_MAC_CMD_DEVICETYPE
#define BQ27Z758_MAC_CMD_FIRMWAREVERSION       BQ27Z8XX_MAC_CMD_FIRMWAREVERSION
#define BQ27Z758_MAC_CMD_HARDWAREVERSION       BQ27Z8XX_MAC_CMD_HARDWAREVERSION
#define BQ27Z758_MAC_CMD_IFCHECKSUM            BQ27Z8XX_MAC_CMD_IFSIGNATURE /* renamed */
#define BQ27Z758_MAC_CMD_STATICDFSIGNATURE     BQ27Z8XX_MAC_CMD_STATICDFSIGNATURE
#define BQ27Z758_MAC_CMD_CHEMID                BQ27Z8XX_MAC_CMD_CHEMID
#define BQ27Z758_MAC_CMD_STATICCHEMDFSIGNATURE BQ27Z8XX_MAC_CMD_STATICCHEMDFSIGNATURE
#define BQ27Z758_MAC_CMD_ALLDFSIGNATURE        BQ27Z8XX_MAC_CMD_ALLDFSIGNATURE
#define BQ27Z758_MAC_CMD_SECURITYKEYS          BQ27Z8XX_MAC_CMD_SECURITYKEYS

/* MAC commands: common control (moved to bq27z8xx_mac_ctrl_cmds, some renamed) */
#define BQ27Z758_MAC_CMD_SHUTDOWNMODE            BQ27Z8XX_MAC_CTRL_CMD_SHUTDOWNMODE
#define BQ27Z758_MAC_CMD_CHARGEFETTOGGLE         BQ27Z8XX_MAC_CTRL_CMD_CHGFETTOGGLE /* renamed */
#define BQ27Z758_MAC_CMD_DISCHARGEFETTOGGLE      BQ27Z8XX_MAC_CTRL_CMD_DSGFETTOGGLE /* renamed */
#define BQ27Z758_MAC_CMD_GAUGING_IT_ENABLE       BQ27Z8XX_MAC_CTRL_CMD_GAUGING      /* renamed */
#define BQ27Z758_MAC_CMD_FET_ENABLE              BQ27Z8XX_MAC_CTRL_CMD_FET_CONTROL  /* renamed */
#define BQ27Z758_MAC_CMD_LIFETIMEDATACOLLECTION  BQ27Z8XX_MAC_CTRL_CMD_LIFETIMEDATACOLLECTION
#define BQ27Z758_MAC_CMD_LIFETIMEDATARESET       BQ27Z8XX_MAC_CTRL_CMD_LIFETIMEDATARESET
#define BQ27Z758_MAC_CMD_CALIBRATIONMODE         BQ27Z8XX_MAC_CTRL_CMD_CALIBRATIONMODE
#define BQ27Z758_MAC_CMD_LIFETIMEDATAFLUSH       BQ27Z8XX_MAC_CTRL_CMD_LIFETIMEDATAFLUSH
#define BQ27Z758_MAC_CMD_LIFETIMEDATASPEEDUPMODE BQ27Z8XX_MAC_CTRL_CMD_LIFETIMEDATASPEEDUPMODE
#define BQ27Z758_MAC_CMD_SEALDEVICE              BQ27Z8XX_MAC_CTRL_CMD_SEALDEVICE
#define BQ27Z758_MAC_CMD_RESET2                  BQ27Z8XX_MAC_CTRL_CMD_DEVICERESET /* renamed */

/* MAC commands: 758-specific control (moved to bq27z758_mac_ctrl_cmds) */
#define BQ27Z758_MAC_CMD_SHELFENABLE     BQ27Z758_MAC_CTRL_CMD_SHELFENABLE
#define BQ27Z758_MAC_CMD_SHELFDISABLE    BQ27Z758_MAC_CTRL_CMD_SHELFDISABLE
#define BQ27Z758_MAC_CMD_RESET1          BQ27Z758_MAC_CTRL_CMD_RESET1
#define BQ27Z758_MAC_CMD_SHIPMODEENABLE  BQ27Z758_MAC_CTRL_CMD_SHIPMODEENABLE
#define BQ27Z758_MAC_CMD_SHIPMODEDISABLE BQ27Z758_MAC_CTRL_CMD_SHIPMODEDISABLE
#define BQ27Z758_MAC_CMD_QMAX_DAY        BQ27Z758_MAC_CTRL_CMD_QMAX_DAY
#define BQ27Z758_MAC_CMD_TAMBIENTSYNC    BQ27Z758_MAC_CTRL_CMD_TAMBIENTSYNC
#define BQ27Z758_MAC_CMD_ROMMODE         BQ27Z758_MAC_CTRL_CMD_ROMMODE

/* Buffer property ID */
#define FUEL_GAUGE_PROP_BQ27Z758_SAFETYSTATUS BQ27Z758_BUFFER_PROP_SAFETYSTATUS

/* Transport helper names */
#define bq27z758_get_i2c_dt_spec(dev)          bq27z8xx_get_i2c_dt_spec(dev)
#define bq27z758_read16(dev, reg, val)         bq27z8xx_read16(dev, reg, val)
#define bq27z758_write16(dev, reg, val)        bq27z8xx_write16(dev, reg, val)
#define bq27z758_read_mac(dev, cmd, data, len) bq27z8xx_read_mac(dev, cmd, data, len)

#endif /* ZEPHYR_DRIVERS_FUELGAUGE_BQ27Z8XX_BQ27Z758_H_ */
