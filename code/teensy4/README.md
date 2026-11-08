# Teensy 4.0 firmware — VL53L8CX carrier

Host: **Teensy 4.0** (NXP i.MX RT1062, 600 MHz, 3.3 V I/O, not 5 V tolerant).

The carrier shifts 3.3 V ↔ 1.8 V (sensor IOVDD max 1.98 V) and the sensors run
from the **external 3.3 V rail**. The Teensy 3.3 V pin is limited to ~250 mA and
is **not** used by the carrier — J1 carries `3V3_EXT`, which powers the shifter
high side (VCCA/VCCB) and the LDO.

## Sketches

| Folder | Mode | Bus use |
|---|---|---|
| `single_sensor_bringup/` | — | One sensor on `SPI` (header pins). Start here. |
| `mode_a_single_bus/` | **A** | All 5 on `SPI`, one CS each, polled sequentially. |
| `mode_b_multi_bus/` | **B** | 5 sensors across 3 SPI + 2 I2C, one per bus, concurrent. |

All three compile for `teensy:avr:teensy40` (arduino-cli).

## Pin maps (Teensy 4.0)

Only **pins 0–23 are breadboardable**; **24–39 are bottom SMT pads**.

### SPI

| Bus | Object | SCK | MOSI | MISO | CS | Location |
|---|---|---|---|---|---|---|
| SPI (LPSPI4) | `SPI` | 13 | 11 | 12 | 10 | header |
| SPI1 (LPSPI3) | `SPI1` | 27 | 26 | 1 | 0 | SCK/MOSI **bottom pads**; MISO/CS header |
| SPI2 (LPSPI1) | `SPI2` | 37 | 35 | 34 | 36 | **bottom pads** |

- Pin 13 also drives the on-board LED. In SPI mode 3 SCK idles high, so the LED
  is lit and toggles during transfers. Harmless, but it loads SCK.
  **There is no alternate SCK on the Teensy 4.0:** the installed core (1.62.0)
  defines `CNT_SCK_PINS = 1` for every SPI object on T4.0, so `setSCK()` cannot
  remap it (`SPI.cpp`, `SPIClass` Teensy-4 section).
- Pins 0/1 are Serial1 RX/TX. No conflict unless Serial1 is used.

### I2C

| Bus | Object | SDA | SCL | Alt SDA/SCL |
|---|---|---|---|---|
| Wire (LPI2C1) | `Wire` | 18 | 19 | — |
| Wire1 (LPI2C3) | `Wire1` | 17 | 16 | 36 / 37 |
| Wire2 (LPI2C4) | `Wire2` | 25 | 24 | — |

- **Do not use Wire1's alt pins 36/37** — they collide with SPI2.
- Pins 16/17 are Serial4 RX/TX. No conflict unless Serial4 is used.

## Mode A vs Mode B

**Mode A** — one SPI bus (`SPI`, header pins), one CS per sensor (10,9,8,7,6),
sequential reads. Header-only, so easiest to bring up.

**Mode B — do not fabricate the I2C carrier variant for this build.**

One sensor per bus: S0 `SPI`, S1 `SPI1`, S2 `SPI2`, S3 `Wire`, S4 `Wire1`. S1 and
S2 need soldered bottom pads; one sensor per I2C bus keeps the default address
`0x52`. The five range concurrently and the MCU drains each bus in turn.

Numbers (round 7): a trimmed frame read costs ~0.97 ms on SPI at 3 MHz, so a
worst-case pass where all five are simultaneously ready is **~4.9 ms = 7.3 %**
of a 66.7 ms frame. 8×8 tops out at 15 Hz (api.h), so Mode A's sequential
polling was never the bottleneck and the trim removed Mode B's only real
advantage. Mode B still costs a TCA9406 plus pull-up engineering per I2C
carrier, two strap changes, bottom-pad soldering for SPI1/SPI2 anyway, and an
extra failure class (a stuck-SDA slave needs a rail power cycle). Keep the
sketch and the hardware deltas as documented options; build Mode A.

If bus isolation is ever needed, or the array grows past five sensors, the next
step is **`SPI` + `SPI1`** (3+2, one CS each, identical carriers), not I2C.

**Mode C (five SPI masters via FlexIO) — not built.** `FlexIOSPI` is mode 0
only (it never programs `_dataMode`), so it cannot talk to the VL53L8CX without
patching an unmaintained library. It also buys no throughput: sampling is
already concurrent in Mode A/B, and the transfer is CPU-polled in every case.
With the trimmed 320-byte frame, Mode A reads all five sensors in ~5 ms of a
66 ms period. Keep Mode A as the production topology. See
`review/FLEXIO_ANALYSIS.md`.

## I2C specifics (verified)

- **Wire buffer:** Teensy `WireIMXRT.h` sets `BUFFER_LENGTH 136`; the tx buffer
  is 137 bytes. The library writes 2 address bytes + `DEFAULT_I2C_BUFFER_LEN` per
  transmission, so the default 136 overflows. The cap is now set **in the
  patched library** (`vl53l8cx_platform_config_custom.h`, `#if defined(__IMXRT1062__)
  → 128`), not in the sketch: `IO_Read`/`IO_Write` are inline members, so a
  sketch-level `#define` only changed the sketch's own translation unit.
  `mode_b` fails to compile unless the cap is present and ≤ 128.
- **I2C clock: 400 kHz.** With the trimmed ~320 B frame a full read is
  (2 addr + 320) × 9 bits ≈ **7.3 ms**, so two I2C sensors use ~22 % of a 66.7 ms
  frame. 1 MHz (the DS14161 limit) is not needed and makes the rise-time budget
  fragile (Fm+ allows t_r ≤ 120 ns). The old "1 MHz is required" claim applied to
  the untrimmed 1444 B frame and no longer holds.
- **Pull-ups:** the TCA9406 has internal 10 kΩ on all four pins and they cannot
  be disabled, so external pull-ups are still needed on both sides. Sensor-side
  limit from DS14161 Table 15: V_OL ≤ 0.36 V at 4 mA → R_p(min) = 360 Ω.
  Start with **2.2 kΩ to 1V8** (effective 1.8 kΩ with the internal 10 kΩ: t_r ≈
  153 ns at 100 pF, 306 ns at 200 pF) and the same to 3V3 on the host side;
  drop to 1.5 kΩ if the harness measures > ~150 pF. Verify in Stage 3.
- **Level translator:** the PCA9306 is specified to 400 kHz (TI product page:
  "max frequency 0.4 MHz"). Use the **TCA9406** instead: TI lists it as a 2-bit
  bidirectional *Fm+ (1 MHz)* I2C/SMBus translator, VCCA 1.65–3.6 V, VCCB
  2.3–5.5 V (VCCA ≤ VCCB), internal 10 kΩ pull-ups on all four lines, high-Z
  when OE is low, VCC isolation if either rail is at GND.
- **The AXC4T774 cannot translate SDA** (unidirectional per channel).

## Hardware deltas for the I2C carrier variant (not drawn)

1. Bidirectional Fm+ I2C translator (**TCA9406**) for SDA/SCL.
2. `SPI_I2C_N` pulled **low** for I2C (R5 currently pulls it high = SPI). A fixed
   pull-down removes the bus-reset capability UM3109 mentions; a GPIO is better.
3. `SPI_I2C_N` (C1) pulled **low** for I²C (R5 currently pulls it high = SPI). A
   fixed pull-down removes the bus-reset capability UM3109 mentions; a GPIO is
   better. Note that this is the same pin the datasheet calls the I²C interface
   reset — there is no separate `I2C_RST` pin on the VL53L8CX.
4. Header exposes SDA/SCL.
5. External I2C pull-ups on both rails (1.8 V sensor side, 3.3 V host side).
6. `SYNC` (B1) is not a "start-of-frame align" line: UM3109 §4.15 describes it as
   a per-acquisition trigger used to avoid interference. Do not tie all five
   together expecting simultaneity; use separate/staggered lines if needed.

## Monitoring and recovery behaviour

- Each sketch polls only its healthy sensors. A sensor that returns an error
  **or stops delivering frames for `max(2 s, 5 frame periods)`** (SPI reads
  cannot report errors) is demoted; its CSV column becomes `-1` immediately.
  `stop_ranging()` is never called at runtime: a demoted sensor may be dead and
  `stop_ranging()` would poll its MCU-stop bit for ~5 s. `init()` begins with a
  software reboot, so `bring_up()` does not need the device stopped first.
- **Rail enable.** The external 3.3 V rail is behind a load switch whose ON pin
  has a 100 kΩ pull-down, so it is **off at every reset**. All three sketches
  define `RAIL_EN_PIN = 2` and assert it in the first lines of `setup()`, before
  `park_all_ncs_high()` — driving a CS while the rail is off feeds ~0.7 mA per
  board through R7 into the dead node. Do not move that call order.
- A demoted sensor is retried at most **once per 30 s**, one sensor per loop
  pass. Both `bring_up()` and the boot-time `bring_up_all()` probe with
  `is_alive()` first (3 tries, 50 ms apart) and then make a **single**
  `configure()` + `start()` attempt — the 30 s loop is the retry mechanism. The
  probe matters at boot: a dead SPI sensor never errors, so `init()` alone burns
  ~4.8 s per attempt (~15 s per dead board, ~75 s for five missing boards).
- **Worst-case blocking.** `init()` now returns as soon as a poll fails instead
  of running every remaining poll, so an ACKing-but-wedged sensor costs at most
  ~4.8 s (SPI) / ~6.8 s (I²C at 400 kHz) per attempt, not the unguarded ~12–15 s.
  Boot and retry both make one attempt, so that bound applies once per pass.
- **Silent-sensor back-off.** After 3 consecutive *silent* demotions (socket A
  answers `is_alive()` and starts, but never produces frames) the retry period
  goes 30 s → 300 s until a frame arrives again. That turns a permanent 1.3 %
  (SPI) or 7.2 % (I²C at 400 kHz) duty loss into 0.13 % / 0.72 %.
- The retry runs **before** the no-frame check, and when it ran the sketch
  re-baselines `last_update_ms[]` for every healthy sensor. Without that, one
  wedged sensor's ~2 s `init()` pushes all the others past their no-frame
  timeout and demotes the whole array.
- A hung sensor with no reset line still needs a power cycle. The 3V3_EXT load
  switch gives the host that power cycle.

## Carrier items still open (from earlier rounds)

- **Power-up slew and the sensor reset are both solved by one 3V3_EXT load
  switch**, so per-board EN networks are not fitted at all.
  - Switch: **TPS22918-class**, ≥ 1.5 A, **CT open** ⇒ t_R ≈ 95 µs at 3.3 V, which
    is inside the 275 µs CORE_1V8-tracking budget, so EN = IN per board stays.
    DS14161 Table 5 wants 0.9 → 1.6 V in ≤ **58.3 µs** and AVDD full level in
    ≤ 3.3 ms.
  - Keep the ≥ 47 µF bulk **upstream**. Downstream capacitance is 5 × 7.1 µF
    plus 1 µF at VOUT ≈ 36.5 µF on the 3.3 V side; the 1.8 V-side capacitors
    (≈1.4 µF per board) charge through the LDO too, so inrush is ≈1.5 A —
    inside the 2 A switch; adding bulk
    downstream pushes it to ≈ 2.9 A.
  - **100 kΩ pull-down on the switch's ON pin** — the datasheet does not state an
    internal one, and the rail must default off at host reset.
  - A per-board EN RC only sequences power **up**: pulling EN low auto-discharges
    the 1.8 V rail (25 Ω) but AVDD stays at 3.3 V, which DS14161 forbids
    (all three supplies must come down together). So the switch is the only
    valid sensor reset and the only stuck-I2C recovery path.
- **R7 back-feed.** Moving the five NCS pull-ups to the host side does **not**
  fix it: with the host off and the rail on, NCS_S is pulled low, U3 is enabled
  and its MISO push-pull drivers fight into the dead Teensy pad. With the load
  switch, host off ⇒ rail dead ⇒ every carrier unpowered (Ioff on U3/U5). Keep
  R7 4.7 kΩ board-side.
- **R6 = 6.8 kΩ, not 10 kΩ.** U3 has 71 kΩ typical internal pull-downs
  (SCES898C §7.3.9); with 10 kΩ, SCLK_S idles at 1.58 V (above V_IH = 1.26 V but
  thin). 6.8 kΩ gives 1.64 V. TI also warns that any external **pull-up** on a
  data I/O must be ≤ 7 kΩ; never fit a pull-**down** here.
- **MISO when all boards are deselected: probably needs no part.** The same
  71 kΩ internal pull-downs bias MISO_H through ≈ 14.2 kΩ to GND across five
  boards. Scope it first; if it floats or sits mid-rail, fit a **47 kΩ
  pull-down** at the host. Never a pull-up: 7 kΩ to 3.3 V against the ≈14.2 kΩ
  combined pull-down would hold MISO_H at ≈2.2 V, defeating the Hi-Z release.

## Library requirements (unchanged)

`../drivers_libraries/STM32duino_VL53L8CX-2.1.0/` is locally patched:

- `vl53l8cx.h`: `VL53L8CX_CS_SETTLE_US` delay after CS-low; `IO_Read` uses a
  2-byte buffer; the I2C address-phase retry is bounded (`VL53L8CX_I2C_NACK_RETRIES`,
  default 5) instead of looping forever on a NACK; the chunked I2C read retries
  a **zero-length** `requestFrom` (the address phase NACKed, so the sensor's
  pointer did not advance) and aborts with status 1 on a **short** chunk (the
  pointer did advance, so a resync inside the driver is impossible).
- `vl53l8cx_platform_config_custom.h`: chunk size 1024, `DEFAULT_I2C_BUFFER_LEN
  128` on i.MX RT, and the unused output blocks are disabled
  (`VL53L8CX_DISABLE_{AMBIENT_PER_SPAD,NB_SPADS_ENABLED,SIGNAL_PER_SPAD,
  RANGE_SIGMA_MM,REFLECTANCE_PERCENT,MOTION_INDICATOR}`), both after including
  the default config. A frame drops from ~1444 to ~320 bytes: ~1 ms per SPI
  read at 3 MHz, ~7.3 ms per I2C read at 400 kHz (~2.9 ms at 1 MHz). Only
  `nb_target_detected`, `distance_mm` and `target_status` remain in
  `VL53L8CX_ResultsData`; every sketch `static_assert`s the trimmed size.
- `vl53l8cx_platform.h`: `#ifndef` guard so the overrides apply.

Every sketch fails to compile against an unpatched copy.

## Build

```
arduino-cli core install teensy:avr \
  --additional-urls https://www.pjrc.com/teensy/package_teensy_index.json
arduino-cli compile --fqbn teensy:avr:teensy40 \
  --libraries "code/drivers_libraries" code/teensy4/<sketch>
```

Use `--libraries "code/drivers_libraries"` so the patched copy wins over any
stock copy installed in `Documents/Arduino/libraries`.

**RAM:** ~108 KB static (Mode A), of which ~86 KB is `VL53L8CX_FIRMWARE`. Those
arrays are already `const`, but the Teensy linker script puts `.rodata` in
`.data`, so they are copied into DTCM — the symbol resolves to `0x20000aa8`,
size `0x15000`. 350 KB of RAM1 stays free; not worth a linker-script hack.

## Bring-up order

1. `single_sensor_bringup` with one board on the header SPI. Its serial `r`
   command forces a re-init while the sensor is streaming — that is the R4
   test (no `stop_ranging()`).
2. `mode_a_single_bus` with all five on `SPI`.
3. `mode_b_multi_bus` after the I2C carriers and bottom-pad harness exist.