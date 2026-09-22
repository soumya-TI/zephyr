# TI LP-AM13E230 — Motor Control HMI

A four-screen BLDC motor-control panel built in LVGL Pro XML (`lv_xml` DSL, LVGL 9.5.0). The
operator starts/stops the drive, sets a speed reference, watches measured speed and telemetry,
and clears latched faults. `project.xml` declares a single 320x480 portrait display; touch is
the assumed input, but no panel or touch driver is in this repo — the only runnable target here
is the desktop simulator (SDL2 on Linux/macOS, Win32 on Windows), whose input devices are mouse,
mousewheel and keyboard. This is UI plus a stand-in drive model; there is no firmware.

## Screens

| Screen | Purpose |
|---|---|
| `run` | Operating screen: RPM readout, -/+ speed-reference stepper, current/temp/load stats, Start/Stop button, on-screen E-STOP tag. |
| `trend` | Speed and load over the last 30 s in an `lv_chart` (static values), plus peak-RPM and mean-load tiles. |
| `faults` | Latched-fault `lv_table`, gated empty state, drive-health card, and a Clear button. |
| `config` | Rotation direction segmented control and three interlock checkboxes. |

The bottom `tab_bar` holds two destinations (Operate, Faults); Trend and Setup are icon actions in
the shared `header`. Each screen stamps `subject_tab` on `screen_loaded`; tab items light
themselves by comparing against it.

## Subjects

Declared in `globals.xml`. Application code drives the UI by writing these.

| Subject | Type | Default | Meaning |
|---|---|---|---|
| `subject_state` | int | 0 | Drive state: 0 stopped, 1 running, 2 fault (`ref_state_*`). Written by the Start/Stop button, the E-STOP tag, the fault Clear button, and `motor_sim_trip()`. |
| `subject_rpm_ref` | int | 1200 | Commanded speed, RPM, range 0-3600. Sole writer is the run-screen stepper (step 50). |
| `subject_rpm_act` | int | 0 | Measured shaft speed, RPM, range 0-3600. Derived; sole writer is `motor_sim.c`. |
| `subject_current_str` | string | `0.0` | Phase current in amps, pre-formatted to one decimal. |
| `subject_temp` | int | 31 | Power-stage temperature, whole degrees C. |
| `subject_load` | int | 0 | Torque demand, 0-100 percent. |
| `subject_bus_str` | string | `24.0` | DC bus voltage, pre-formatted to one decimal. |
| `subject_peak_rpm` | int | 0 | Session peak of `subject_rpm_act`. Never reset. |
| `subject_avg_load` | int | 0 | Session mean of `subject_load`, percent. |
| `subject_runtime_str` | string | `00:00` | Time since last start, `mm:ss`. |
| `subject_fault_count` | int | 0 | Latched faults, 0-99. 0 shows the empty state and hides the table and Clear button. |
| `subject_fault_active` | int | 0 | 0/1 — a fault is latched. Also turns the Faults tab red from any screen. |
| `subject_dir` | int | 0 | Commanded rotation: 0 CW, 1 CCW (`ref_dir_*`). Written from the config screen. |
| `subject_ramp` | int | 5 | Acceleration ramp in units of 100 RPM/s, 1-20. Read by `motor_sim.c`. No UI writer. |
| `subject_cfg_softstart` | int | 1 | 0/1 interlock — ramp instead of stepping to reference. Two-way via `bind_checked`. |
| `subject_cfg_reverse` | int | 0 | 0/1 interlock — permit direction reversal while running. Two-way via `bind_checked`. Not read by any C. |
| `subject_cfg_derate` | int | 1 | 0/1 interlock — fold back current above 80 C. Two-way via `bind_checked`. |
| `subject_alert_never` | int | 0 | Permanently 0. Default for `tab_item`'s `alert` prop so non-alarming tabs need no subject. |
| `subject_tab` | int | 0 | Active screen: 0 run, 1 faults, 2 trend, 3 config. Set by each screen on load. |

19 subjects. No subjects are declared outside `globals.xml`.

## Custom C

| File | Responsibility |
|---|---|
| `custom/motor_sim.c` / `.h` | The drive model. A 100 ms `lv_timer` ramps `subject_rpm_act` toward `subject_rpm_ref` and derives load, current, temperature, bus voltage, runtime, peak and mean load. Reads `subject_state`, `subject_rpm_ref`, `subject_ramp`, `subject_cfg_softstart`, `subject_cfg_derate`. Exposes `motor_sim_publish()` (the hardware seam) and `motor_sim_trip()`. |

Everything the operator touches — start/stop, speed reference, direction, interlocks, fault clear,
E-stop — is written straight from XML via `subject_set_int_event`, `subject_increment_event` and
`bind_checked`; no C is involved and `motor_sim.c` never touches a widget. C is only for values
that must be computed: fixed-point formatting for the decimal strings (`bind_text-fmt` forwards to
`lv_label_set_text_fmt`, which has no float support here).

## Wiring it to application code

- Include `motor_control.h` (which pulls in the generated `motor_control_gen.h` where every
  `lv_subject_t` is declared `extern`).
- Entry point: `void motor_control_init(const char * asset_path)` in `motor_control.c`. It calls
  the generated `motor_control_init_gen(asset_path)` — which creates and registers every subject,
  so nothing may touch a subject before it — then `motor_sim_init()`, then loads the run screen.
  The simulator calls it as `motor_control_init("A:")`.
- Push measured values in with `motor_sim_publish(rpm, milliamps, temp_c, load_pct)` from the real
  control loop, and drop the `lv_timer` in `motor_sim_init()`. To latch a fault, call
  `motor_sim_trip()` — nothing calls it today, so the fault path is unreachable at runtime.
- Direct subject access, as used in `custom/`: `lv_subject_set_int(&subject_state, 1)`,
  `lv_subject_get_int(&subject_rpm_ref)`, `lv_subject_copy_string(&subject_current_str, buf)`.
- A physical E-stop must write `subject_state = 0`, the same subject the on-screen tag writes, so
  both paths converge on one state.
- Edit by hand: `*.xml`, `custom/*.c`, `user_config.cmake` (lists the `custom/` sources),
  `sim/main.c`, `tools/iconset.js`. Regenerated, never edited: `*_gen.c`, `*_gen.h`,
  `*_gen.cmake`, `CMakeLists.txt`, `images/*_data.c`, `fonts/*_data.c`.

## Layout

- `screens/` — the four screens.
- `components/` — reusable XML components; `components/base/` holds the layout primitives
  (`row`, `column`, `card`, `panel`, `container`, `label`) everything else builds on.
- `widgets/` — empty (README only); no custom widgets in this project.
- `custom/` — the C the application team owns.
- `globals.xml` — consts, fonts, subjects, images, styles. The integration contract.
- `images/` / `fonts/` — PNG and TTF sources; the `*_data.c` beside them are generated.
- `sim/` — desktop simulator (own CMake project, fetches LVGL, SDL2/Win32 HAL).
- `tests/` — click-and-assert interaction tests.
- `tools/iconset.js` — icon manifest consumed by the shared `../tools/icons.js` rasteriser.
