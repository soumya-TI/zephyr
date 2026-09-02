# Collaborator Addition — Aman-Lachhiramka-ti (Aman Lachhiramka)

## Why add Aman-Lachhiramka-ti?

Aman contributed the initial MSPM33C Cortex-M33 SoC, DTS, and board
support to upstream Zephyr, alongside existing MSPM0 driver work
covering pinctrl validation, counter support, and HAL integration.
Plans to continue contributing to MSPM0 and MSPM33 peripheral
enablement and platform driver improvements.

---

## Authored PRs

### zephyrproject-rtos/zephyr

| PR | Title | Merged |
|----|-------|--------|
| [#111577](https://github.com/zephyrproject-rtos/zephyr/pull/111577) | add MSPM33C Cortex-M33 initial support | 2026-07-17 |
| [#113223](https://github.com/zephyrproject-rtos/zephyr/pull/113223) | boards: ti: lp_mspm0l2228: Add GPTIMER counter support | 2026-07-20 |
| [#102007](https://github.com/zephyrproject-rtos/zephyr/pull/102007) | drivers: pinctrl: mspm0: Add validation for pin configurations | 2026-06-07 |
| [#102773](https://github.com/zephyrproject-rtos/zephyr/pull/102773) | manifest: update hal_ti revision | 2026-01-28 |
| [#113204](https://github.com/zephyrproject-rtos/zephyr/pull/113204) | drivers: counter: ti: mspm0: Convert to native register access | open |
| [#112498](https://github.com/zephyrproject-rtos/zephyr/pull/112498) | drivers: pinctrl: ti: mspm0: convert to native register writes | open |

### zephyrproject-rtos/hal_ti

| PR | Title | |
|----|-------|-|
| [#81](https://github.com/zephyrproject-rtos/hal_ti/pull/81) | Fix: Conditional MSPM0 Directory Inclusion | merged |
| [#91](https://github.com/zephyrproject-rtos/hal_ti/pull/91) | hal: dts: mspm0: restructure pinctrl dtsi to reference existing node | open |

---

## MAINTAINERS.yml Change

```yaml
TI MSPM Platforms:
  collaborators:
    - d-philpot
    + Aman-Lachhiramka-ti
    - Alex9360
    ...
```
