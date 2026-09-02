# Collaborator Addition — soumya-TI (Soumya Tripathy)

## Why add soumya-TI?

Contributed TI K3 platform support including SCMI SMC transport for
AM62L, AM62L wakeup RTC DTS and counter support, multi-core support
for SK-AM62, and TF-A cmake refactoring. Also contributed initial TI
MSPM0 LCD auxdisplay driver and SPI test enablement. Plan to continue
contributing to TI K3 and MSPM platform driver improvements and
peripheral enablement.

---

## Authored PRs

### zephyrproject-rtos/zephyr

| PR | Title | Status |
|----|-------|--------|
| [#117237](https://github.com/zephyrproject-rtos/zephyr/pull/117237) | drivers: auxdisplay: TI MSPM0 LCD driver | open |
| [#115390](https://github.com/zephyrproject-rtos/zephyr/pull/115390) | tests: drivers: spi: loopback: add lp_mspm0g3519 support | 2026-08-10 |
| [#113113](https://github.com/zephyrproject-rtos/zephyr/pull/113113) | soc: am62x: Kconfig: set default RTC init priority | 2026-07-27 |
| [#112818](https://github.com/zephyrproject-rtos/zephyr/pull/112818) | dts: ti: am62l: Add wakeup RTC peripheral and counter support | 2026-07-14 |
| [#108583](https://github.com/zephyrproject-rtos/zephyr/pull/108583) | Update power domain unit address format | 2026-05-08 |
| [#108238](https://github.com/zephyrproject-rtos/zephyr/pull/108238) | Increase MAX_XLAT_TABLES default for SMP with 36-bit VA | 2026-05-04 |
| [#108103](https://github.com/zephyrproject-rtos/zephyr/pull/108103) | Refactor Trusted Firmware-A module cmake | 2026-08-04 |
| [#106763](https://github.com/zephyrproject-rtos/zephyr/pull/106763) | Add TI K3 build infrastructure to generate boot images | open |
| [#105643](https://github.com/zephyrproject-rtos/zephyr/pull/105643) | Enable TI K3 UART config for A53 cores | 2026-03-19 |
| [#103588](https://github.com/zephyrproject-rtos/zephyr/pull/103588) | Update documentation references to AM62L-specific resources | 2026-02-11 |
| [#103584](https://github.com/zephyrproject-rtos/zephyr/pull/103584) | SCMI: Add SMC transport support with TI AM62L SoC | 2026-03-18 |
| [#102948](https://github.com/zephyrproject-rtos/zephyr/pull/102948) | Correct SoC variant ID and enable multi-core for TI SK-AM62 | 2026-02-11 |

### zephyrproject-rtos/hal_ti

| PR | Title | |
|----|-------|-|
| [#88](https://github.com/zephyrproject-rtos/hal_ti/pull/88) | Enable "west blobs" fetching for TI foundational security firmware | open |

---

## Reviewed PRs

### TI MSPM

| PR | Title |
|----|-------|
| [#116778](https://github.com/zephyrproject-rtos/zephyr/pull/116778) | dts: ti: add pinctrl DTSIs for MSPM0GX51X and MSPM33C321A |
| [#112498](https://github.com/zephyrproject-rtos/zephyr/pull/112498) | drivers: pinctrl: ti: mspm0: convert to native register writes |
| [#114782](https://github.com/zephyrproject-rtos/zephyr/pull/114782) | drivers: serial: uart_mspm0: migrate from Driverlib to native |
| [#91500](https://github.com/zephyrproject-rtos/zephyr/pull/91500) | drivers: flash: mspm0: flash driver support |
| [#111577](https://github.com/zephyrproject-rtos/zephyr/pull/111577) | add MSPM33C Cortex-M33 initial support |
| [#111377](https://github.com/zephyrproject-rtos/zephyr/pull/111377) | soc: ti: rename MSP SoC family from ti_mspm0 to ti_msp |
| [hal_ti#91](https://github.com/zephyrproject-rtos/hal_ti/pull/91) | hal: dts: mspm0: restructure pinctrl dtsi |

### TI K3

| PR | Title |
|----|-------|
| [#106541](https://github.com/zephyrproject-rtos/zephyr/pull/106541) | boards: am62l_evm: add I/O expander support |
| [#102513](https://github.com/zephyrproject-rtos/zephyr/pull/102513) | Add Cadence MSPI controller and Infineon S28HX512T driver |
| [#103390](https://github.com/zephyrproject-rtos/zephyr/pull/103390) | drivers: i2c: i2c_omap: fix FIFO byte count extraction |
| [#103060](https://github.com/zephyrproject-rtos/zephyr/pull/103060) | firmware: scmi: move polling logic to transport layer |
| [#101338](https://github.com/zephyrproject-rtos/zephyr/pull/101338) | drivers: firmware: tisci: Add secure path support |
| [#101283](https://github.com/zephyrproject-rtos/zephyr/pull/101283) | am243x_evm/am2434/r50_0: configure timer clock source |
| [#97172](https://github.com/zephyrproject-rtos/zephyr/pull/97172) | add SDHC support for TI MMCSD subsystem |
| [#108156](https://github.com/zephyrproject-rtos/zephyr/pull/108156) | arch: arm: add support for TI AM13 Family and AM13E230x Launchpad |

---

## MAINTAINERS.yml Change

```yaml
TI K3 Platforms:
  collaborators:
    - ...
    - Kronosblaster
    + soumya-TI

TI MSPM Platforms:
  collaborators:
    - ...
    - santhosh-c-c
    + soumya-TI
```
