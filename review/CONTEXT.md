# CONTEXT — VL53L8CX × 5 carrier (one page)

Read this first, then `Design_proposal.txt` (Rev 2) for detail.

## What is being built

Five identical carrier boards, each holding one ST **VL53L8CX** 8×8 multizone
ToF sensor, to a **Teensy 4.0** (i.MX RT1062, 600 MHz, 3.3 V I/O). The host is
USB-powered; the sensors are powered from a **separate external 3.3 V rail
(≥1.5 A)** because the Teensy 3.3 V pin is limited to ~250 mA. The two rails are
never joined.

The host was migrated from a Waveshare RP2350-Zero Mini; the carrier is
unchanged (3.3 V ↔ 1.8 V shifters).

## Per-board architecture

```
J1 (1x6): 3V3_EXT, GND, SCLK, MOSI, MISO, NCS
  |
  +-- 3V3 --> MIC5504-1.8 LDO --> 1V8 (CORE_1V8 + IOVDD of sensor and shifters)
  |
  +-- SCLK/MOSI/MISO --> SN74AXC4T774 (U3), 3.3V <-> 1.8V, OE = NCS_S
  +-- NCS --> SN74AXC1T45 (U5), 3.3V -> 1.8V, DIR = GND
  |
  +-- VL53L8CX (SPI mode, LPn and I2C_RST pulled up on board)
```

**Key scheme:** U3's OE is driven by that board's NCS_S. NCS_S is produced by
U5, *not* by U3. Therefore:

- NCS low → board selected → U3 enabled → its MISO drives the shared bus.
- NCS high → U3 Hi-Z → MISO released.

Routing NCS through U3 deadlocks (the board can never be selected). That was the
Rev 1 defect, fixed in Rev 2 by adding U5.

## Bus parameters

- Sensor SPI: mode 3, **3 MHz**; sensor I2C: **400 kHz** (DS14161 allows 1 MHz;
  400 kHz is enough at ~320 B frames and has far more rise-time margin).
- **Mode A** (single bus, sequential): all five on `SPI` (SCK 13, MOSI 11,
  MISO 12), CS 10, 9, 8, 7, 6 — all header pins.
- **Mode B** (concurrent): S0 SPI (13/11/12, CS 10), S1 SPI1 (27/26/1, CS 0),
  S2 SPI2 (37/35/34, CS 36), S3 Wire (19/18), S4 Wire1 (16/17). One sensor per
  bus, default I2C address `0x52` kept.

## Firmware

- `code/teensy4/single_sensor_bringup/` — first light, one sensor on `SPI`
  (13/11/12, CS 10).
- `code/teensy4/mode_a_single_bus/` — five on one SPI bus, sequential.
- `code/teensy4/mode_b_multi_bus/` — 3 SPI + 2 I2C, concurrent.

All three sketches compile for `teensy:avr:teensy40` with arduino-cli.

**Mode A is the production topology.** Round 7: a trimmed frame read costs
~0.97 ms on SPI at 3 MHz, so a pass with all five ready is ~4.35 ms (6.5 % of a
66.7 ms frame). The I2C carrier variant should not be fabricated.

The vendored library `code/drivers_libraries/STM32duino_VL53L8CX-2.1.0/` is
**locally patched**: `vl53l8cx.h` defines `VL53L8CX_CS_SETTLE_US` and inserts
that delay after CS-low in `IO_Read`/`IO_Write`, because NCS_S also gates U3's
OE. `IO_Read` uses a 2-byte buffer; the chunk size is overridden to 1024 via
`vl53l8cx_platform_config_custom.h`.

## Confidence tags in the design

`[V]` verified against a document, `[S]` secondary source, `[U]` unverified /
inferred. The `[U]` items are the main reason for this review.

## Scope of this review

- **Hardware design only** (the PCB/schematic is not drawn yet; this is a
  pre-layout design review).
- Firmware correctness against the real library headers.
- Resolve `[U]` items using the datasheets listed in `FILES_TO_SEND.md`.

## What round 1 already found (do not re-report unless you can refute it)

1. 1.8 V LDO ramp with EN=IN may violate the 0.9→1.6 V ≤63 µs slew rule.
2. R7 (NCS_H pull-up to 3V3_EXT) back-feeds the dead rail / host when one side
   is unpowered, so "any power-up order" is false.
3. Thermal-pad paste pattern (one aperture vs AN5897 Fig. 3's eight rectangles)
   and the "no vias below footprint" rule conflict with the 8 thermal vias.
4. The CS-settle patch is justified but was fragile (now hardened with a macro
   and a compile-time guard).
5. Ranging mode was never set (ULD default is AUTONOMOUS, not CONTINUOUS).
6. Firmware robustness: unchecked statuses, `new`, stale first frames,
   unfiltered `target_status`, unused counter.
7. ~4 KB stack buffer per SPI call; `ResultsData` on the stack in HelloWorld.
8. Grid orientation was mirrored.
9. HelloWorld spun forever after a failed init.
10. No sensor reset / power-cycle path.
11. MISO may float when all boards are deselected.
12. Optical/assembly citation errors (AN5939 air gap, TN1532 wet-clean, reflow
    peak, multi-emitter FOV crosstalk, SYNC left unrouted).

Items 4–9 are **fixed in the current firmware**. Items 1–3 and 10–12 are
**hardware/documentation** and remain open.

## Round 2 found (and what was done)

| # | Finding | Status |
|---|---|---|
| 1 | U3 (SN74AXC4T774 RSV) pin numbers conflict with SCES898C Table 4-1 | **Open** — check the RSV\|16 pinout figure before layout |
| 2 | Static-array copy-init of `VL53L8CX` (self-pointers) | **Fixed** — in-place construction + pointer array |
| 3 | Retry claimed but not implemented; `r` re-inits a live sensor | **Fixed** — retry loop, periodic retry, demotion, `stop_ranging()` before re-init |
| 4 | 1.8 V slew: limit is 58 µs, MIC5504 max 125 µs, EN=IN | **Open** — decouple EN, supply soft-start ≤ ~3 ms |
| 5 | R7 back-feeds the dead rail / host | **Open** — host-side pull-up or load switch |
| 6 | 4 KB stack frame in `IO_Read`/`IO_Write` | **Fixed** — chunk size 1024 via custom platform config |
| 7 | NCS parked after the Serial wait | **Fixed** — park first in `setup()` |
| 8 | `five_sensor_array` stale CSV / flooding / wrong-sensor `g` | **Fixed** — per-sensor buffers, stale timeout, print on update |
| 9 | RP2350-E9 floating MISO (GP12) | **Open** — read E9, add pull-down, check stepping |
| 10 | `#error` guard weaknesses | **Fixed** — `#ifndef` + `#if < 1 #error` |
| 11 | §9.3 power numbers were sensor-only | **Fixed** — rail-side numbers added |
| 12 | Citation / assembly corrections | **Fixed** — reflow, no-wet-clean source, AN5939, xtalk |

Still needs documents: SCES898C §5.11/§7.3.9, SCES882E, `vl53l8cx_api.c`,
RP2350 datasheet, Arduino-Pico stack size.

## Round 3 found (and what was done)

| # | Finding | Status |
|---|---|---|
| 13 | Retry loop blocks healthy sensors | **Fixed** — 30 s period, one dead sensor per pass, after the poll |
| 14 | `stop_ranging()` on a never-started sensor | **Fixed** — `started[]` flag |
| 15 | Custom config replaced the default wholesale | **Fixed** — includes the default config first |
| 16 | No chunk-size override guard | **Fixed** — `#if > 1024 #error` in every sketch |
| 17 | `IO_Read` reserved a full chunk of stack | **Fixed** — `data_write[2]` |
| 18 | `five_sensor_poll` spammed errors | **Fixed** — counts and demotes after 50 |
| 19 | README overstated the chunk cost | **Fixed** |
| 1 | U3 pin numbers | **Resolved** — match a secondary SCES898C mirror; confirm on TI's figure |
| 4 | 1.8 V slew / EN=IN | **Open** |
| 5 | R7 back-feed | **Open** |
| 9 | RP2350-E9 | **Moot** — host is now a Teensy 4.0 |

## Round 4 — host migration (this round)

The host moved to a **Teensy 4.0** and the project gained a second topology.
- **Mode A**: one SPI bus, CS-per-sensor, sequential (port of the old design).
- **Mode B**: one sensor per bus — 3 SPI + 2 I2C — so all five range
  concurrently. I2C at 1 MHz, one sensor per bus (address `0x52`).
- **Hardware deltas for Mode B:** bidirectional I2C translator (PCA9306),
  `SPI_I2C_N` jumper, header with SDA/SCL, SPI2 on bottom pads, SYNC unrouted.
- **Still open (carrier):** 1.8 V slew / EN=IN, R7 back-feed.
- **Now moot:** RP2350-E9, RP2350 edge pins, Arduino-Pico stack size.

### Round-4 review fixes

| # | Finding | Status |
|---|---|---|
| 20 | SPI1 26/27 are bottom pads | **Fixed** — Mode A/bring-up on `SPI` header pins |
| 21 | Mode A/B lost retry/demotion/stale | **Fixed** — restored |
| 22 | Mode A printed -1 every poll | **Fixed** — print only on update |
| 23 | I2C_RST for the I2C variant | **Documented** |
| 24 | Wire buffer overflow | **Fixed** — `DEFAULT_I2C_BUFFER_LEN 128` |
| 25 | PCA9306 at 1 MHz | **Fixed (part change)** — TCA9406, TI lists 1 MHz Fm+ |
| 26 | MISO float | **Open** — external 10–47 kΩ host-side pull |

## Round 5 found (and what was done)

| # | Finding | Status |
|---|---|---|
| 27 | Sketch-level `DEFAULT_I2C_BUFFER_LEN` cannot change the library's inline `IO_Read`/`IO_Write` | **Fixed** — moved into `vl53l8cx_platform_config_custom.h`; `mode_b` `#error` guard |
| 28 | PCA9306 is a 400 kHz part | **Fixed** — TCA9406 (1 MHz Fm+) |
| 29 | I2C `IO_Read` retries a NACK forever | **Fixed** — 5 attempts, returns 1 |
| 30 | SPI sensors can never be demoted | **Fixed** — 2 s no-frame timeout + retry |
| 31 | Retry blocks healthy sensors | **Documented** — cost stated in README |
| 32 | LED load on SCK 13 | **Accepted** — core has `CNT_SCK_PINS = 1`, no remap |
| 33 | `INPUT_PULLUP` would break SPI MISO | **Documented** — external resistor only |
| 34 | `I2C_RST = -1` keeps the SPI pull-up | **Documented** — I2C variant delta |

Also this round: the ST reference ULD package was removed (unreferenced; the
vendored library already contains `vl53l8cx_api.c`, which is now in the bundle).

## Round 6 found (and what was done)

| # | Finding | Status |
|---|---|---|
| 35 | Retrying a dead sensor blocks ~12 s (`stop_ranging` polls MCU-stop 5 s, then 3 × 2.1 s `init`), because `started[]` was never cleared on demotion | **Fixed** — `demote()` clears `started[]`; `bring_up()` calls `is_alive()` first |
| 36 | Second unbounded loop: the I2C chunked read spins forever on a short `requestFrom` | **Fixed** — returns 1 on a short chunk |
| 37 | Frame is ~1444 B, and unused outputs could be dropped to ~320 B | **Fixed** — six `VL53L8CX_DISABLE_*` blocks in the custom config; sketches `static_assert` the trimmed struct |
| 38 | No-frame timeout fixed at 2000 ms regardless of rate | **Fixed** — `max(2000, 5 × frame period)` |
| 39 | Round-5 "SPI sensors can never be demoted" was too strong | **Corrected** — sensor GO2 errors do count |
| 25 | TCA9406 at 1 MHz | **Open (bench)** — and with the 320-byte frame, 400 kHz would also fit |
| 4, 5, 26 | Slew/EN, R7 back-feed, MISO bias | **Open (bench)** — see Design §13.8 for the proposed fixes |

**Mode C (FlexIO, five SPI masters) is dropped.** The reviewer refuted the
performance case: sampling is already concurrent, every transfer is CPU-polled,
and `FlexIOSPI` is mode 0 only. Mode A is the production topology. If bus
isolation is ever needed, bit-bang SPI mode 3 instead.

All three sketches compile for `teensy:avr:teensy40` with the trimmed frame.

## Round 7 found (and what was done)

| # | Finding | Status |
|---|---|---|
| 35–39 | Round-6 fixes (retry cost, short-chunk abort, trim, timeout scaling, GO2 errors) | **Verified** against `vl53l8cx_api.c` |
| 40 | Host-side NCS pull-ups do **not** fix R7 back-feed: host off / rail on leaves NCS_S low, U3 enabled, five MISO drivers fighting into a dead pad | **Decided** — load switch on 3V3_EXT, enable host-driven with a pull-down; keep R7 board-side |
| 41 | No-frame timeout cascade: a wedged sensor's 2–6 s `init()` ages every healthy sensor past 2000 ms, so one fault blanks the array | **Fixed** — retry runs before the no-frame check, re-baselines `last_update_ms[]`, and `bring_up()` makes a single attempt |
| 42 | "10–47 kΩ host-side pull" was direction-ambiguous and the pull-**up** reading is wrong (TI: external pull-up ≤ 7 kΩ) | **Fixed** — SCES898C §7.3.9 gives 71 kΩ internal pull-downs; likely no part, else 47 kΩ pull-down |
| 43 | R6 = 10 kΩ gives SCLK_S idle = 1.58 V against a 1.26 V V_IH | **Fixed** — R6 → 6.8 kΩ (idle 1.64 V) |
| 44 | A stuck I2C slave needs a power cycle | **Fixed** — covered by the 3V3_EXT load switch |
| 45 | `static_assert` message said ~272 B; the trimmed struct is 258 B | **Fixed** — message updated |
| 46 | `stop_ranging()` branches became dead code after round 6 | **Fixed** — `started[]` and all `stop_ranging()` calls removed; a comment records why |
| 25 | TCA9406 at 1 MHz | **Closed** — I2C moved to 400 kHz; ~7.3 ms per frame read, ~22 % duty for two sensors |
| 4 | 1.8 V slew with EN = IN | **Specified** — per-board 100 kΩ + 1 µF EN RC with a 2N7002 host pull-down (needs a 1×7 J1), or a 3V3_EXT load switch with t_RAMP ≤ 275 µs. Bench-scope CORE_1V8 |
| 26 | MISO bias | **Specified** — likely no part (14.2 kΩ effective); else 47 kΩ pull-down at the host |

Address-phase NACK handling also improved (round-7 Q3): a **zero-length**
`requestFrom` is retried, because the sensor's pointer provably did not advance;
a **short** chunk still aborts, because it did.

All three sketches compile for `teensy:avr:teensy40` after these changes.

## Round 8 found (and what was done)

Round 8 reported that the uploaded bundle was still round 7. That was an upload
problem, not a code problem: `review/send10/` was already Rev 4 (CONTEXT 12 821 B,
Design 63 130 B, `mode_a` 9 296 B, `mode_b` 10 857 B, README 11 216 B,
`vl53l8cx.h` 14 791 B). **Check these sizes before uploading again.**

| # | Finding | Status |
|---|---|---|
| r8-1 | The per-board EN pull-down is **not** a sensor reset: it auto-discharges 1.8 V but AVDD stays at 3.3 V, and DS14161 requires all three supplies down together | **Fixed** — reset / stuck-I2C recovery assigned to the 3V3_EXT load switch only |
| r8-2 | The load switch and the per-board EN RC/2N7002 are redundant — the switch's 95 µs ramp already satisfies the slew rule | **Fixed** — load switch **only**; EN RC + 2N7002 + J1 1×7 withdrawn, J1 stays 1×6, EN = IN per board |
| r8-3 | TPS22918 sizing | **Adopted** — CT open (t_R ≈ 95 µs), inrush ≈ 1.27 A into 36.5 µF, bulk ≥ 47 µF upstream, 100 kΩ pull-down on ON, ≤ 59 mV drop at 750 mA |
| r8-4 | `bring_up_all()` had no `is_alive()` gate: ~4.8 s × 3 per dead SPI board, ~75 s to first CSV with five missing | **Fixed** — shared `probe()` gates both `bring_up()` and `bring_up_all()` |
| r8-5 | ~108 KB static RAM — reviewer inferred non-`const` buffers | **Measured, accepted** — they are `const`; the Teensy linker puts `.rodata` in `.data`. `VL53L8CX_FIRMWARE` at `0x20000aa8`, 86 KB, 350 KB free |
| r8-6 | Stale "1 MHz is required" text, RP2350 remnants in §8/§10/§13.4 | **Fixed** — §13.3 → 400 kHz with the derivation, harness on Teensy pins, checklist migrated, §13.4 names the TCA9406 only |
| r8-7 | I²C retry cost understated (~1 s) | **Fixed** — ~2.2 s healthy, ~4.6 s wedged, at 400 kHz |

Worst array dark time after r8-4: ≈ 2.9 s (wedged SPI) / ≈ 4.6 s (wedged I²C at
400 kHz) per 30 s, no cascade. All three sketches recompile.

## Round 9 found (and what was done)

| # | Finding | Status |
|---|---|---|
| r9-1 | The adopted load switch had **no firmware**: its ON pin has a 100 kΩ pull-down, so the rail is off at reset and every probe fails | **Fixed** — `RAIL_EN_PIN = 2` in all three sketches, asserted before `park_all_ncs_high()` |
| r9-2 | No system BOM for the switch | **Fixed** — §7 system BOM: switch, 100 kΩ, 22 µF at VIN, 1 µF at VOUT, bulk upstream |
| r9-3 | "With the switch off no current flows" was wrong for host-on/switch-off | **Fixed** — §13.9 states the 0.7 mA/board path and the ordering rule |
| r9-4 | Silent-but-healthy sensors cost array duty forever | **Fixed** — ×10 back-off after 3 silent demotions (30 s → 300 s), reset by a real frame |
| r9-5 | Is a soft-start cap needed on the switch? | **Closed** — CT open: rail 0.0278 V/µs (2.3× the minimum), CORE_1V8 window ≈ 25 µs tracking / 48.6 µs worst case vs 58.3 µs |
| r9-6 | `probe()` timing | **Confirmed** — rails valid ~0.3–0.5 ms after ON; first probe ≥ 50 ms in |
| r9-7 | Doc residuals (§8 EN-RC fallback, §7/§9.3 RP2350, §9.5 and the risk list, §13.7/§13.8 rows 4/5/26, §13.4 item 12) | **Fixed** — see §13.11 |
| r9-8 | `vl53l8cx.h` uploaded at 14 291 B (round-7 content) | **Upload-only** — workspace copy is 14 791 B; round 10 embeds the patched source inline |

All three sketches recompile (Mode A 46 672 flash / 108 544 RAM1).

## Round 10 — review converged

No BLOCKER / MAJOR / MINOR. Round-9's two MAJORs verified closed from source;
DIR = LOW ⇒ B→A confirmed against SCES882E's function table. Three NITs fixed:

| # | Finding | Status |
|---|---|---|
| r10-1 | Small (single-chunk) I²C read had no zero-length retry | **Fixed** — same bounded loop as the chunked path |
| r10-2 | `IO_Write` swallowed mid-download NACKs | **Fixed** — first failed chunk is returned |
| r10-3 | §8 wrote U5 as "A→B" (datasheet says B→A for DIR = GND) | **Fixed** — direction now [V]; only pin numbers stay [U] |
| r10-4 | Inrush numbers ignored the 1 µF at VOUT | **Fixed** — 36.5 µF / 1.27 A |

**Next step is bench bring-up, not another review round.** Scope the ON→rail edge
under 36.5 µF and CORE_1V8 0.9→1.6 V on every board first.

## Round 11 — verification phase (no new findings)

Round 11 was not a review. It embedded the frozen Rev-7 source and asked for a
57-row PASS / FAIL / NV matrix across carrier electrical (E1–E19), firmware
(F1–F20), library patches (L1–L9) and system/assembly (S1–S9).

**Result: 47 PASS, 10 NV, 0 FAIL.** Nothing was manufactured or changed; no
finding was raised against the code or the schematic. The 10 NV rows are the
bench/document closures: switch t_D/ON V_IH/current limit/QOD (R1), MIC5504
turn-on endpoints (R2), the sensor's real 320 B block set (R3), re-init of a
streaming sensor (R4), SCES882E pin numbers for U5 (R5), MISO_H idle level (R6),
I²C rise time (R7), LPSPI SCK duty (R8), far-end AVDD under the harness (R9) and
a Rev-7 compile/RAM record (R10).

Bench procedure supplied with numeric thresholds (B0–B9); only **R4** can force
a design change if it fails, and the load switch already provides the fallback
(rail cycle in the retry path). One cosmetic evidence typo was noted: E8's
1.51 V figure corresponds to a 35.5 kΩ (half-value) internal pull-down, not
"2× typical"; the PASS verdict is unaffected (1.51 V > 1.26 V V_IH).

**Status: design frozen at Rev 7. Begin Stage 1 bring-up.**
