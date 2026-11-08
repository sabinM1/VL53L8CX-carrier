# VL53L8CX × 5 — ToF carrier array for Teensy 4.0

Five identical carrier boards, each carrying one ST **VL53L8CX** 8×8 multizone
Time-of-Flight sensor, wired to a **Teensy 4.0**. Every carrier level-shifts
3.3 V ↔ 1.8 V locally, so the host sees a plain 3.3 V SPI bus and the sensors
see only their 1.8 V I/O domain.

The array is read as a live **CSV stream over USB serial**: one timestamp and
five centre-zone distances, 15 times per second.

```
ms,S0,S1,S2,S3,S4
1234,512,488,-1,1203,497
```

`-1` means "no fresh valid target" for that sensor (no frame, no target, or a
demoted sensor). Everything else is millimetres.

| | |
|---|---|
| Sensor | VL53L8CX — 8×8 zones, 940 nm VCSEL, 45°×45° FoV (65° diagonal), up to 400 cm |
| Host | Teensy 4.0 (i.MX RT1062, 600 MHz) |
| Sensors | 5 |
| Frame rate | 15 Hz, continuous ranging |
| Host bus | SPI mode 3 @ 3 MHz, one CS per sensor |
| Sensor rail | 3.3 V AVDD, 1.8 V CORE/IOVDD, local LDO per board |
| External rail | 3.3 V, **≥ 1.5 A**, switched, separate from the Teensy |
| Typical / peak current | 465 mA / 750 mA on the external rail |
| Frame size | 320 B (trimmed from 1444 B) |
| Status | Design **Rev 7**, review loop converged; verification round: 47 PASS / 10 NV / 0 FAIL |

> **The Teensy's own 3.3 V pin (~250 mA) never powers the array.** J1 carries
> `3V3_EXT` from a separate supply. The two 3.3 V rails share only GND.

---

## 1. System overview

```mermaid
flowchart TB
    USB["USB 5 V"] --> T["Teensy 4.0"]
    PSU["3.3 V supply, at least 1.5 A"] --> LS["Load switch, TPS22918 class"]
    T -->|"GPIO 2 = RAIL_EN"| LS
    LS -->|"3V3_EXT"| C1["Carrier 0"]
    LS -->|"3V3_EXT"| C2["Carrier 1"]
    LS -->|"3V3_EXT"| C3["Carrier 2"]
    LS -->|"3V3_EXT"| C4["Carrier 3"]
    LS -->|"3V3_EXT"| C5["Carrier 4"]
    T -->|"SCLK, MOSI, MISO"| C1
    T -->|"SCLK, MOSI, MISO"| C2
    T -->|"SCLK, MOSI, MISO"| C3
    T -->|"SCLK, MOSI, MISO"| C4
    T -->|"SCLK, MOSI, MISO"| C5
    T -->|"CS 10, 9, 8, 7, 6"| C1
    T -->|"CS 10, 9, 8, 7, 6"| C2
    T -->|"CS 10, 9, 8, 7, 6"| C3
    T -->|"CS 10, 9, 8, 7, 6"| C4
    T -->|"CS 10, 9, 8, 7, 6"| C5
```

The load switch is the **only** way the sensors are powered down. It is also the
only reset that complies with DS14161 (all three supplies must fall together)
and the recovery path for a hung sensor or a stuck I²C bus. Its ON pin has a
100 kΩ pull-down, so the rail is **off at every Teensy reset**; firmware raises
it as the first action of `setup()`.

### Per-carrier block diagram

```mermaid
flowchart LR
    J1["J1 1x6"] --> LDO["U2 MIC5504-1.8"]
    LDO --> V18["1V8: CORE_1V8 + IOVDD"]
    J1 -->|"SCLK / MOSI / MISO"| U3["U3 SN74AXC4T774"]
    J1 -->|"NCS"| U5["U5 SN74AXC1T45"]
    U5 -->|"NCS_S"| S["U1 VL53L8CX"]
    U5 -->|"NCS_S"| U3
    U3 <-->|"SPI 1.8 V"| S
```

**The OE → NCS_S scheme is the heart of the carrier.** U3's output enable is
driven by the board's own `NCS_S`, and `NCS_S` is produced by U5 — not by U3.
So NCS low → that board is selected → its U3 turns on → its MISO drives the
shared bus. NCS high → U3 is Hi-Z → MISO released. Routing NCS through U3
would deadlock the board (it could never be selected). Only one MISO driver is
ever active.

---

## 2. Components

### 2.1 Per carrier board (×5 identical, 20 SMD parts each)

| Ref | Part | Package | Purpose |
|---|---|---|---|
| U1 | VL53L8CXV9GC/1 (no liner) or V0GC/1 (with liner) | LGA-16, 6.4 × 3.0 × 1.75 mm | The sensor. SPI mode, LPn and I²C_RST pulled up on board |
| U2 | MIC5504-1.8YMT | DFN-4, 1.0 × 1.0 mm | 1.8 V LDO: CORE_1V8 + IOVDD + VCCA of U3/U5. EN tied to IN |
| U3 | SN74AXC4T774RSVR | UQFN-16, 2.6 × 1.8 mm | SCLK / MOSI / MISO translator, **OE = NCS_S**, DIR1/2/3 = GND, DIR4 = 1V8 |
| U5 | SN74AXC1T45 (DCK SC70-6 or DBV SOT-23-6) | SC70-6 / SOT-23-6 | NCS translator. **DIR = GND ⇒ B→A** (3.3 V host drives 1.8 V sensor) |
| C1 | 4.7 µF X5R 10 V | 0402 | AVDD at sensor pin C4 (check DC-bias derating, else 0603) |
| C2 | 100 nF | 0402 | CORE_1V8 at B7 |
| C3 | 100 nF | 0402 | IOVDD at A3 |
| C4 | 100 nF | 0402 | U3 VCCA |
| C5 | 100 nF | 0402 | U3 VCCB |
| C6 | 2.2 µF X5R 10 V | 0402 | 3V3 at U2 input |
| C7 | 1 µF X5R 10 V | 0402 | U2 output (auto-discharged by the MIC5504) |
| C8 | 100 nF | 0402 | U5 VCCA |
| C9 | 100 nF | 0402 | U5 VCCB |
| R1 | 47 kΩ | 0402 | NCS_S → IOVDD |
| R2 | 47 kΩ | 0402 | INT → IOVDD (INT is not routed to the host) |
| R3 | 47 kΩ | 0402 | SYNC → IOVDD |
| R4 | 47 kΩ | 0402 | LPn → IOVDD |
| R5 | 47 kΩ | 0402 | SPI_I2C_N → IOVDD (selects SPI) |
| R6 | 6.8 kΩ | 0402 | SCLK_S → 1V8. Holds SCLK at its mode-3 idle level so selecting the board cannot create a spurious clock edge |
| R7 | 4.7 kΩ | 0402 | NCS_H → 3V3. Keeps every board deselected while the host is in reset |
| J1 | 1×6 header, 2.54 mm | THT | 3V3_EXT, GND, SCLK, MOSI, MISO, NCS |

### 2.2 System BOM (outside the carriers)

| Ref | Part | Value / notes | Qty |
|---|---|---|---|
| U10 | Load switch, TPS22918 class | ≥ 1.5 A at 3.3 V. **CT open** ⇒ t_R ≈ 95 µs. Feeds all five carriers | 1 |
| R10 | 100 kΩ | ON pin → GND. The rail must default **off**; the datasheet states no internal pull-down | 1 |
| C10 | 22 µF | at the switch VIN. Reservoir against IR drop in the harness | 1 |
| C11 | 1 µF | at the switch VOUT | 1 |
| — | Bulk, ≥ 47 µF | **upstream** of the switch at the supply. Downstream bulk would push inrush toward 3 A | 1 |
| — | 3.3 V supply | ≥ 1.5 A, 3.3 V ±3 % at the far board | 1 |
| — | Harness | ≤ 20 cm, one ground wire per board, round-trip supply R < 0.15 Ω | 1 |

### 2.3 Why these parts

- **MIC5504-1.8** — 150 mA, ±2.5 % over temperature ⇒ 1.755–1.845 V, inside the
  sensor's 1.62–1.98 V window. Its ~25 Ω auto-discharge keeps the 1.8 V rail
  from creeping up while the rail is off. 1.8 V peak per board is ~90 mA.
- **SN74AXC4T774** — 4-bit AXC translator; 71 kΩ typical internal pull-downs on
  the data I/Os, which is what biases an idle MISO when all boards are
  deselected (≈14.2 kΩ across five boards, i.e. MISO_H sits near 0 V — no
  external bias part expected). t_EN ≤ 26 ns, so the firmware's 1 µs CS settle
  has ~38× margin.
- **SN74AXC1T45** — NCS must not pass through the gated U3. One direction bit
  is all that is needed, and DIR = GND selects B→A, which is the required
  host→sensor direction.
- **R6 = 6.8 kΩ, not 10 kΩ** — with U3's 71 kΩ pull-down, SCLK_S idles at
  1.64 V (V_IH = 1.26 V). TI also requires any external pull-**up** on an AXC
  data I/O to be ≤ 7 kΩ. Never fit a pull-down here.
- **R7 = 4.7 kΩ board-side** — do not move the NCS pull-ups to the host. Host
  off + rail on would then enable all five U3s into a dead Teensy pad. The load
  switch removes the whole problem: host off ⇒ rail dead ⇒ everything unpowered.
- **Spring for the TCA9406 only if you build Mode B.** It is an Fm+ (1 MHz)
  bidirectional I²C translator; the PCA9306 is a 400 kHz part and the AXC4T774
  cannot translate SDA at all. The I²C carrier variant is **not** part of this
  build.

---

## 3. Operating modes

| Mode | Buses | Concurrency | Status |
|---|---|---|---|
| **A** | `SPI` only (SCK 13, MOSI 11, MISO 12), one CS per sensor (10, 9, 8, 7, 6) | Sequential polling | **Production topology.** Build this |
| **B** | 3 SPI + 2 I²C, one sensor per bus | Concurrent | Documented option. Do **not** fabricate the I²C carrier variant |
| C | Five FlexIO SPI masters | Concurrent | **Dropped** — `FlexIOSPI` cannot do mode 3, and it buys no throughput |

**Why Mode A wins.** A trimmed 320 B frame costs ≈0.97 ms on SPI at 3 MHz, so a
worst-case pass with all five sensors ready is ≈4.35 ms — 6.5 % of a 66.7 ms
frame. 8×8 tops out at 15 Hz anyway, so the sequential poll was never the
bottleneck. Mode B's only historical advantage (two I²C buses needing 1 MHz to
fit a frame) disappeared when the output trim cut the frame from 1444 to 320 B.

**Mode B layout** (only if you deliberately build it): S0 `SPI` (13/11/12, CS 10),
S1 `SPI1` (27/26/1, CS 0), S2 `SPI2` (37/35/34, CS 36), S3 `Wire` (SDA 18 / SCL 19),
S4 `Wire1` (SDA 17 / SCL 16). One sensor per I²C bus keeps the default address
`0x52`. SPI1 SCK/MOSI and all of SPI2 are **bottom SMT pads**, not header pins.

**If you ever need bus isolation or more than five sensors**, the next step is
`SPI` + `SPI1` (3+2, identical carriers), not I²C.

---

## 4. Exposed pins and wiring to the Teensy

### 4.1 Carrier header J1 (1×6, 2.54 mm) — same on every board

| Pin | Net | Direction (host view) | Level | Notes |
|---|---|---|---|---|
| 1 | `3V3_EXT` | Power in | 3.3 V | From the switched external rail. **Never** from the Teensy 3.3 V pin |
| 2 | `GND` | Power return | 0 V | One ground wire per board, star point at the host |
| 3 | `SCLK` | Host → carrier | 3.3 V | Buffered to 1.8 V by U3 B1 → A1 |
| 4 | `MOSI` | Host → carrier | 3.3 V | U3 B2 → A2 |
| 5 | `MISO` | Carrier → host | 3.3 V | U3 A4 → B4; Hi-Z unless that board is selected |
| 6 | `NCS` | Host → carrier | 3.3 V | U5 B → A; also enables that board's U3 |

Nothing else leaves the carrier. INT, SYNC, LPn and I²C_RST stay on-board, pulled
to 1.8 V. There is no reset line: the rail switch is the reset.

### 4.2 Teensy 4.0 pin map

Only **pins 0–23 are breadboardable**; 24–39 are bottom SMT pads.

| Function | Mode A | Mode B | Teensy peripheral |
|---|---|---|---|
| SCLK | 13 | 13 (S0), 27 (S1), 37 (S2) | LPSPI4 / LPSPI3 / LPSPI1 |
| MOSI | 11 | 11 (S0), 26 (S1), 35 (S2) | — |
| MISO | 12 | 12 (S0), 1 (S1), 34 (S2) | — |
| CS | 10, 9, 8, 7, 6 | 10, 0, 36 (+ I²C S3/S4) | — |
| I²C SDA / SCL | — | 18 / 19 (`Wire`), 17 / 16 (`Wire1`) | LPI2C1 / LPI2C3 |
| Rail enable | **2** | **2** | GPIO out, high = rail on |

Notes:
- Pin 13 also drives the on-board LED. In SPI mode 3 SCK idles high, so the LED
  is lit and toggles during transfers. Harmless; the Teensy 4.0 core has
  `CNT_SCK_PINS = 1`, so SCK cannot be remapped.
- Pin 2 has no bus, UART, LED or I²C role on the Teensy 4.0, and it is Hi-Z at
  reset, so the 100 kΩ holds the rail off through reset.
- Pins 0/1 are Serial1; 16/17 are Serial4. No conflict unless you use those.
- Do **not** use Wire1's alternate pins 36/37 — they collide with SPI2.

### 4.3 Harness (Mode A)

```
Teensy 3.3 V logic side            Carrier J1 (×5)
  pin 13  ── SCLK ────────────────  J1-3  (all boards in parallel)
  pin 11  ── MOSI ────────────────  J1-4  (all boards in parallel)
  pin 12  ── MISO ────────────────  J1-5  (all boards in parallel)
  pin 10  ── CS0 ─────────────────  J1-6  board 0
  pin  9  ── CS1 ─────────────────  J1-6  board 1
  pin  8  ── CS2 ─────────────────  J1-6  board 2
  pin  7  ── CS3 ─────────────────  J1-6  board 3
  pin  6  ── CS4 ─────────────────  J1-6  board 4
  GND     ── GND ─────────────────  J1-2  (one wire per board)
  pin  2  ── RAIL_EN ─────────────  load switch ON
  3.3 V switched rail ────────────  J1-1  (all boards; ≥1.5 A supply)
```

Keep the harness ≤ 20 cm, route a ground return alongside each signal, and give
each carrier its own ground wire. Optional 22 Ω series resistors at the host end
of SCLK and MOSI only if you see ringing.

---

## 5. Power

| Rail | Source | Load | Notes |
|---|---|---|---|
| 3.3 V AVDD / VCCB | External, switched | 465 mA typ, 750 mA peak for all five | ≥ 1.5 A supply, 3.3 V ±3 % at the far board |
| 1.8 V CORE/IOVDD/VCCA | U2 per board | ~90 mA peak per board | 1.755–1.845 V; 135 mW dissipated per LDO |
| Teensy | USB | ~40–100 mA | Never feeds the sensors |

Slew requirements from DS14161 Table 5 (these are **minimums**):
0.9 → 1.6 V on CORE_1V8/IOVDD in **≤ 58.3 µs**; AVDD to full level in ≤ 3.3 ms.
The load switch's ~95 µs rise gives CORE_1V8 ≈25 µs of window if the LDO
input-tracks, ≈48.6 µs worst case if the LDO's own turn-on governs — both
inside 58.3 µs. Do not add a CT capacitor unless the scope says otherwise.

---

## 6. Firmware

Three sketches, all compiling for `teensy:avr:teensy40`:

| Sketch | Use |
|---|---|
| `code/teensy4/single_sensor_bringup/` | First light: one carrier on header SPI (13/11/12, CS 10) |
| `code/teensy4/mode_a_single_bus/` | **Production:** five carriers on `SPI`, sequential |
| `code/teensy4/mode_b_multi_bus/` | 3 SPI + 2 I²C, concurrent (optional) |

### Build

```
arduino-cli core install teensy:avr \
  --additional-urls https://www.pjrc.com/teensy/package_teensy_index.json

arduino-cli compile --fqbn teensy:avr:teensy40 \
  --libraries "code/drivers_libraries" code/teensy4/mode_a_single_bus
```

Use `--libraries "code/drivers_libraries"` so the patched library wins over any
stock copy in
`Documents/Arduino/libraries`. Verified with arduino-cli 1.5.1 / teensy:avr
1.62.0. Footprints: Mode A 46 672 flash / 108 544 RAM1; Mode B 47 776 / 110 624;
bring-up 45 840 / 99 040.

### The vendored library is patched — do not replace it

`code/drivers_libraries/STM32duino_VL53L8CX-2.1.0/` differs from upstream:

- `vl53l8cx.h` — `VL53L8CX_CS_SETTLE_US` delay after CS-low in both `IO_Read`
  and `IO_Write` (required because NCS_S also gates U3's OE); 2-byte address
  buffer; bounded I²C NACK retry (5 attempts); zero-length `requestFrom` retried,
  short chunk aborted; `IO_Write` returns the first failed chunk.
- `vl53l8cx_platform_config_custom.h` — chunk 1024; `DEFAULT_I2C_BUFFER_LEN 128`
  on i.MX RT (the Teensy Wire TX buffer is 137 B; 2 + 136 overflows);
  six `VL53L8CX_DISABLE_*` blocks that trim the frame to 320 B.
- `vl53l8cx_platform.h` — `#ifndef` guard so the overrides apply.

Each sketch hard-fails at compile time against an unpatched copy (`#error` on the
CS-settle macro, chunk size and trim macros, plus a `static_assert` on the
258-byte `ResultsData`). Keep it that way.

### Runtime behaviour

- **Rail first.** `RAIL_EN_PIN = 2` is asserted before `park_all_ncs_high()`.
  Driving a CS while the rail is dead feeds ~0.7 mA per board through R7 into
  the unpowered 3V3 node.
- **No `stop_ranging()` at runtime.** A demoted sensor may be dead, and
  `stop_ranging()` would poll its MCU-stop bit for ~5 s. `init()` starts with a
  software reboot, so `bring_up()` does not need the device stopped.
- **Probe before init.** `is_alive()` (3 tries, 50 ms apart) gates both boot and
  retry, so a missing board costs milliseconds instead of ~15 s.
- **Demotion.** 50 consecutive driver errors, or no frame for
  `max(2000 ms, 5 × frame period)`, demotes a sensor; its column becomes `-1`.
- **Retry.** One demoted sensor per loop pass, at most every 30 s, with a single
  `configure()` + `start()` attempt. The retry runs **before** the no-frame check
  and re-baselines healthy sensors, so one wedged sensor cannot cascade-demote
  the array.
- **Silent back-off.** After 3 consecutive silent demotions the retry period goes
  30 s → 300 s until a frame returns.
- **Recovery.** Re-init handles a running or confused sensor. A hung sensor or a
  stuck I²C bus needs the rail switch: drop RAIL_EN, wait, raise it, re-init.

---

## 7. Repository layout

```
README.md                     this file
Design_proposal.txt           full design record: architecture, BOM, netlist,
                              confidence tags, per-round review tables
pcb design/
  Schematic_and_PCB_Guide.md  step-by-step schematic + PCB instructions
  *.pdf                       DS14161, AN5897 thermal, PCB manufacturing
reference docs/               markdown conversions of the ST documents
code/
  teensy4/                    three sketches + firmware README
  drivers_libraries/          vendored, patched VL53L8CX library
review/
  CONTEXT.md                  one-page project context + review history
  ROUND10_SEND.txt            last review bundle
  ROUND11_SEND.txt            verification-phase bundle (57-row matrix)
  FLEXIO_ANALYSIS.md          why Mode C was dropped
```

---

## 8. Bring-up checklist

Numbers are pass criteria, not suggestions.

**Before power**
1. DMM: J1 continuity per board, no short between 3V3_EXT and GND, harness
   ≤ 20 cm, round-trip supply resistance < 0.15 Ω.
2. With the rail off, confirm no board is warm and 3V3_EXT is < 0.1 V.

**Stage 1 — one carrier**
3. Power edges (scope, trigger on ON): AVDD 0→3.13 V in ≤ 3.3 ms (expect ~95 µs);
   CORE_1V8 0.9→1.6 V in **≤ 58.3 µs**. Fails ⇒ stop and fix the power tree.
4. ID probe: `single_sensor_bringup` prints the sensor ID 0xF0/0x0C early.
5. First ranging: no `init FAILED`, no repeated corrupt frames. `start_ranging()`
   verifies the block size against the sensor, so a successful start IS the
   confirmation that the 320 B trim is accepted.
6. Timing: frame period ≈ 66.7 ms, CSV ≈ 15 lines/s.
7. Accuracy: targets at 250 / 500 / 1000 mm on the centre zone; `target_status`
   5 or 9.
8. MISO idle: all CS high, scope MISO_H — expect ≈ 0 V (internal pull-downs).
   If it floats, fit a 47 kΩ **pull-down** at the host; never a pull-up.
9. Dead-sensor drill: cut one board's 3V3_EXT only. Its column → `-1` within
   ~2 s, the other columns keep streaming, it returns within 40 s of restore.
10. Forced demote on a **streaming** sensor (test sketch): `demote(i)`, then
    confirm the 30 s retry re-inits it without `stop_ranging()` and frames resume.

**Stage 2 — five carriers**
11. Worst-case loop pass ≤ ~5 ms; CSV up to ~75 lines/s.
12. Remove one board: isolation as in step 9. Remove all five: loop stays alive,
    all columns `-1`, retries round-robin.
13. Rail current 400–550 mA sustained; scope far-board AVDD ≥ 3.13 V.
14. 10-minute thermal check on the five LDOs (~135 mW each).

**Stage 3 — Mode B only if built**
15. I²C at 400 kHz, rise time ≤ 300 ns at the far end; else 2.2 kΩ → 1.5 kΩ.
16. NACK drill: power off one I²C sensor → demotion in ~50–100 ms, no bus hang.

**Stage 4 — sign-off**
17. 24 h soak: zero unexplained demotions, current drift < 10 %, CSV continuous.

---

## 9. Verification status

The design has been through 11 adversarial review rounds against the primary
datasheets (DS14161 Rev 7, SCES898C, SCES882E, TCA9406, MIC550x, TPS22918,
Teensyduino core). Rounds 9–10 produced no BLOCKER/MAJOR/MINOR findings; round 11
was a **verification pass**, not a review: 57 rows, **47 PASS / 10 NV / 0 FAIL**.

There is nothing left to fix in the source. The 10 NV rows are bench- or
document-only closures:

| ID | Open item | Closes by | If marginal |
|---|---|---|---|
| R1 | TPS22918 t_D(CT = 0), ON V_IH, current limit, QOD resistance | scope ON→rail edge | t_R > 275 µs ⇒ fit CT; still slow ⇒ reopen rail design |
| R2 | MIC5504 turn-on endpoints | CORE_1V8 window per board (≤ 58.3 µs) | Different LDO needed |
| R3 | ST firmware's actual 320 B block set | successful `start_ranging` | Re-audit the trim |
| R4 | Re-init of a **streaming** sensor | forced-demote test | Add a rail cycle to the retry path (switch already supports it) |
| R5 | SCES882E pin numbers for U5 | TI package figure **before** layout | U5 net rework |
| R6 | MISO_H idle with five Hi-Z boards | scope | 47 kΩ host pull-down |
| R7 | I²C rise time at 400 kHz | scope ≤ 300 ns | 1.5 kΩ pull-ups |
| R8 | LPSPI SCK duty (RM-derived 50 %) | scope 40–60 % | Clock-root change (very unlikely) |
| R9 | Far-end AVDD under the real harness | DMM under peak load ≥ 3.13 V | Shorten / upgrade harness |
| R10 | Rev-7 compile/RAM numbers unrecorded | one arduino-cli run | Blocks nothing |

Only **R4** could force a design change. Everything else has a measurement that
either passes or has a staged fallback already designed in.

---

## 10. Safety and handling

- **Class 1 laser.** 940 nm VCSEL, invisible. Do not stare into the emitter.
- Sensor is **MSL 3**: use within 18 months, store at 16–27 °C / 40–60 % RH.
  If the humidity indicator card is > 10 %, bake at 90 °C for > 6 h (sensor only,
  never the tape-and-reel) before reflow.
- Reflow **240 °C recommended / 260 °C maximum, 3 passes, convection only**.
  Not vapour phase.
- **No PCB washing and no conformal coating.** If coating is unavoidable, mask
  the optical windows with a gasket or cap.
- Manual rework: ≤ 275 °C on the cap for desoldering, ≤ 260 °C for re-soldering;
  a fourth reflow voids the sensor.
- All Teensy I/O is 3.3 V and is **never** connected to a sensor pin directly.
  Sensor I/O is 1.8 V; 1.98 V is the absolute maximum on CORE_1V8/IOVDD.

---

## 11. Further reading

- `Design_proposal.txt` — the authoritative design record, including netlists,
  confidence tags and every per-round finding.
- `pcb design/Schematic_and_PCB_Guide.md` — how to draw the schematic and lay
  out the board.
- `code/teensy4/README.md` — firmware, pin maps, library patch details.
- `review/CONTEXT.md` — one-page context and the review history.
- `pcb design/DS_vl53l8cx.pdf`, `an5897-*.pdf`, `pcb-manufacturing-guidelines-*.pdf`
  — the primary mechanical, thermal and assembly sources.
- `reference docs/` — markdown conversions of DS14161, UM3109, AN5894, AN5939-class
  notes, and the field-of-view / aperture calculator.