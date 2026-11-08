# FlexIO SPIs on Teensy 4.0 — feasibility note (round 6)

Question: can the Teensy 4.0 expose **two more SPI masters** beyond the three
LPSPI controllers (`SPI`, `SPI1`, `SPI2`) using FlexIO, so that all five
VL53L8CX carriers can be SPI-only?

Sources: installed `teensy:avr 1.62.0`, `libraries/FlexIO_t4` (v1.0.0) —
`FlexIO_t4.h/.cpp`, `FlexIOSPI.h/.cpp`, `README.md`, `examples/SPI/Simple`; and
KurtE/FlexIO_t4 `master` (`src/FlexIOSPI.cpp`, same content).

## Answer

Yes. Two extra masters are practical, a third is possible.

- `FlexIOHandler::CNT_FLEX_IO_OBJECT = 3`; `flexIO1`, `flexIO2`, `flexIO3` are
  all instantiated (`FlexIO_t4.cpp`).
- Each `FlexIOSPI` claims **2 shifters + 1 timer** (`begin()`:
  `requestTimers(_csPin != -1 ? 2 : 1)`, `requestShifter()` twice). Each
  controller has 4 shifters and 4 timers, so at most **2 FlexIOSPI per
  controller**.
- T4.0 FlexIO pins (README tables):
  - FlexIO1: Teensy 2, 3, 4, 5, 33
  - FlexIO2: Teensy 6, 7, 8, 9, 10, 11, 12, 13, 32
  - FlexIO3: Teensy 7, 8, 14–23, 26, 27
- Clock: default FlexIO clock is 480 MHz / 16 = 30 MHz; `beginTransaction`
  computes `div = (clock/2)/baud`, so 3 MHz is exact (`div = 4` →
  15 MHz / 5 = 3 MHz). `TIMCMP` packs `div` in the low byte, so much slower
  clocks are also reachable.

## Proposed Mode C pin map (header pins only)

| Sensor | Master | MOSI | SCK | MISO | CS |
|---|---|---|---|---|---|
| S0 | `SPI` (LPSPI4) | 11 | 13 | 12 | 10 |
| S1 | `SPI1` (LPSPI3) | 26 | 27 | 1 | 0 |
| S2 | `SPI2` (LPSPI1) | 35 | 37 | 34 | 36 |
| S3 | `FlexIOSPI` on FlexIO2 | 6 | 8 | 9 | 20 |
| S4 | `FlexIOSPI` on FlexIO3 | 7 | 14 | 15 | 21 |

CS is a plain GPIO (`csPin = -1`, `digitalWrite`), which keeps the SPI object
at one timer. Pin 7 exists on both FlexIO2 and FlexIO3; it is assigned to
FlexIO3 only. All eight added pins are breadboardable (0–23).

## Blocking issue: FlexIOSPI does not implement SPI mode 3

`FlexIOSPI::beginTransaction(FlexIOSPISettings)` stores `_dataMode` and never
programs it. `grep dataMode FlexIOSPI.cpp` in both the installed copy and
master returns exactly three hits: the assignment, the comparison that decides
whether to recompute `TIMCMP`, and the `SPI_MODE_TRANSMIT_ONLY` test.

`begin()` hard-codes one timing:

- `TIMCFG[_timer] = DEC_ON_CLOCK_SHIFT_ON_OUTPUT | OUTPUT_LOW_WHEN_ENABLED |
  DISABLE_ON_8BIT_MATCH | ENABLE_WHEN_TRIGGER_HIGH` → SCK idles low (CPOL = 0)
- TX: `SHIFTCTL ... SHIFT_ON_FALLING_EDGE`
- RX: `SHIFTCTL ... SHIFT_ON_RISING_EDGE`

That is SPI mode 0, which is what the library's own example uses. The
VL53L8CX requires **mode 3** (CPOL = 1, CPHA = 1). Mode 3 therefore requires a
patch to `FlexIOSPI` (polarity plus the shift/sample edge relationship) and a
scope check of a real frame; the exact patch is not proven here.

Not acceptable workaround: externally inverting SCK maps mode 0 to mode 2, not
mode 3.

## Other verified constraints

- `transferBufferNBits()` is a per-byte register poll (`SHIFTSTAT`), no FIFO,
  no DMA; `transferNBits()` polls with a 0xffff timeout and then returns 0xff
  silently.
- FlexIO3 has **no DMA** (README). FlexIO2 does, but only through the async
  `transfer(buf, retbuf, count, EventResponderRef)` API.
- `begin()` writes `IOMUXC_PAD_PUE | PUS(3)` on the MISO pin — an internal
  pull-up. This helps the floating-MISO finding for the two FlexIO buses.
- `begin()` returns false unless MOSI, SCK and MISO all map to the same
  controller.
- The VL53L8CX class only has `SPIClass*` and `TwoWire*` constructors, and the
  driver calls `beginTransaction(SPISettings(...))`. `FlexIOSPI` takes
  `FlexIOSPISettings`, so a `FlexIOSPI*` constructor plus a small adapter (or a
  template) is required in the vendored library.

## Advantages

1. No I2C anywhere: no TCA9406 Fm+ risk, no bidirectional translator, no
   `SPI_I2C_N` / `I2C_RST` deltas, no address handling, no NACK hangs. All five
   carriers are the same board in the same mode.
2. 3 MHz on every lane: about 3.2 ms to read a 1.2 KB frame per sensor versus
   about 12 ms at 1 MHz I2C. Five sensors ≈ 16 ms of a 66 ms period (24 %),
   against ≈ 34 ms (51 %) for Mode B.
3. Uniform firmware: one driver path, CI-driven CS, same retry/demotion and
   stale handling for all five.
4. The two extra buses use header pins only.

## Disadvantages

1. Depends on a mode-3 patch to an unmaintained WIP library (v1.0.0, single
   author, no tests).
2. Requires a new constructor/adapter in the vendored VL53L8CX library.
3. CPU-polled byte loop, no FIFO; FlexIO3 has no DMA.
4. FlexIO pins overlap the LPSPI pin sets; FlexIO1 has only 5 pins; shifters
   and timers are shared, limited resources.
5. Silent failure modes: poll timeout returns 0xff with no error code.

## Recommendation

Add **Mode C** as an additional sketch (five sensors on `SPI`, `SPI1`, `SPI2`,
FlexIO2, FlexIO3) and keep Mode A and Mode B. The carrier is identical in all
SPI modes, so this costs nothing in hardware; the FlexIO risk stays confined to
one sketch and Mode B remains the fallback. Gate Mode C on a scope-verified
mode-3 frame during single-sensor bring-up.

## Verdict after the round-6 review — do not build Mode C

The reviewer refuted the performance argument, and the numbers above support it:

- Sampling is already concurrent in every mode. The MCU drains the buses in
  turn regardless; FlexIO does not make the reads parallel, it is CPU-polled
  byte-by-byte with no FIFO, so each byte costs roughly the same as an LPSPI
  byte plus loop overhead.
- Mode A on one 3 MHz bus costs 5 x ~1 ms = ~5 ms CPU per 66 ms frame with the
  trimmed 320-byte payload. Mode B costs less than that. Mode C cannot go
  below the sum of the five transfers either.
- So Mode C buys **bus isolation**, not speed, and it does so by patching an
  unmaintained library whose mode-3 support does not exist (`begin()` hard-codes
  SCK-idle-low, TX-on-falling, RX-on-rising; an external inverter turns that
  into mode 2, not mode 3).

Decision: **Mode C is not built.** Production topology is Mode A. If a future
requirement needs one sensor per physical bus without I2C, implement it as
**bit-banged SPI mode 3** on GPIO (600 MHz gives ~100 cycles per half period at
3 MHz) rather than patching `FlexIOSPI`.
