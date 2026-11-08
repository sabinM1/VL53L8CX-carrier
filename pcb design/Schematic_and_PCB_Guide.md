# Schematic and PCB design guide — VL53L8CX carrier

How to turn the frozen design in `../Design_proposal.txt` (Rev 7) into a
schematic and a fabricated board. Read `../README.md` first for the system
overview.

This is a **two-layer, 20-part carrier** repeated five times, plus one small
system power board (or a wired assembly) for the 3.3 V load switch. Nothing here
is high speed: 3 MHz SPI, 400 kHz I²C, 150 mA per LDO.

**Document precedence.** Where this guide and a primary document disagree, the
primary document wins:

1. `DS_vl53l8cx.pdf` — sensor electrical, pin, land-pattern and reflow data.
2. `an5897-pcb-thermal-guidelines-*.pdf` — thermal pad, via and paste pattern.
3. `pcb-manufacturing-guidelines-vl53l8.pdf` — assembly, MSL3, rework.
4. `../Design_proposal.txt` — this project's frozen decisions.
5. This guide — procedure and checklists.

---

## 0. Design inputs (do not redesign these)

| Item | Value | Source |
|---|---|---|
| Sensor interface | SPI mode 3, 3 MHz | DS14161 Table 8 |
| Sensor supplies | AVDD 3.3 V, CORE_1V8/IOVDD 1.8 V | DS14161 Table 9/10 |
| 1.8 V absolute max | 1.98 V | DS14161 |
| AVDD window | 3.13–3.47 V recommended | DS14161 |
| CORE_1V8 slew | 0.9 → 1.6 V in **≤ 58.3 µs** (minimum 0.012 V/µs) | DS14161 Table 5 |
| AVDD slew | full level in ≤ 3.3 ms (minimum 0.001 V/µs) | DS14161 Table 5 |
| SPI sink/source | V_OL ≤ 0.2 × IOVDD at 4 mA; V_IH ≥ 0.7 × IOVDD | DS14161 Table 15 |
| Header | J1 1×6: 3V3_EXT, GND, SCLK, MOSI, MISO, NCS | Design |
| Address/straps | SPI_I2C_N, LPn, I²C_RST pulled to IOVDD | Design |
| Reset | **None on board.** The system load switch is the reset | Design |
| Board count | 5 identical carriers | Design |

Carrier part count: **20 SMD parts** (4 ICs, 7 resistors, 9 capacitors) + J1.
Do not add parts without a review entry in `Design_proposal.txt`.

---

## 1. Sheet plan

Use three schematic sheets (or three clearly separated areas on one sheet):

| Sheet | Contents |
|---|---|
| 1 — Carrier (root) | U1 sensor, U2 LDO, U3/U5 translators, all passives, J1. This sheet is copied ×5 |
| 2 — System power | 3.3 V input, load switch U10, R10, C10/C11, bulk, star ground, harness connector |
| 3 — Host interface | Teensy 4.0 pin map as a connector or notes block, only if you draw the harness |

Keep the carrier sheet **identical** across the five instances. Use a
hierarchical sheet or a replicated block so a change cannot be applied to only
one board.

---

## 2. Net list to capture (per carrier)

Capture exactly these nets. Net names matter — later checks and the firmware
documentation reference them.

### 2.1 Power nets

| Net | Connections |
|---|---|
| `3V3` | J1-1, C1, U1.C4 (AVDD), U2.IN, C6, U3.13 (VCCB), C5, U5.6 (VCCB), C9, R7 |
| `1V8` | U2.OUT, C7, U1.A3 (IOVDD) + C3, U1.B7 (CORE_1V8) + C2, U3.14 (VCCA) + C4, U5.1 (VCCA) + C8, R1, R2, R3, R4, R5, R6 |
| `GND` | J1-2, U1.C3, U1.C7, U1.B4 (thermal pad), U2.GND, U3.8, U5.2, C1–C9 returns |

`U2.EN` ties to `U2.IN` (3V3). There is **no** per-board enable network; the
system load switch does the sequencing.

### 2.2 Signal nets

| Net | Connections |
|---|---|
| `SCLK_H` | J1-3, U3.12 (B1), R6 is **not** here |
| `MOSI_H` | J1-4, U3.11 (B2) |
| `MISO_H` | J1-5, U3.9 (B4) |
| `NCS_H` | J1-6, U5.4 (B), R7 to `3V3` |
| `SCLK_S` | U3.1 (A1), U1.A5 (SCL/MCLK), **R6 to `1V8`** |
| `MOSI_S` | U3.2 (A2), U1.A4 (SDA/MOSI) |
| `MISO_S` | U3.4 (A4), U1.C5 (MISO) |
| `NCS_S` | U5.3 (A), U1.C2 (NCS), R1 to `1V8`, **U3.7 (OE)** |

`SCLK_S`, `MOSI_S`, `MISO_S` and `NCS_S` are 1.8 V nets. `NCS_S` has **four**
connections — it is both the chip select and the translator enable. That is the
whole scheme; keep it.

### 2.3 Straps and unused pins

| Pin | Net |
|---|---|
| U1.A1 (GPIO1/INT) | R2 47 kΩ to `1V8`; no host connection |
| U1.B1 (GPIO2/SYNC) | R3 47 kΩ to `1V8`; no host connection |
| U1.A2 (LPn) | R4 47 kΩ to `1V8` (device always enabled) |
| U1.C1 (SPI_I2C_N) | R5 47 kΩ to `1V8` (selects SPI) |
| U1.A6, U1.A7, U1.C6 (RSVD1–3) | Wired to `GND` in this design — **see the verification note below** |
| U3.3 (A3) | No connect (idle channel) |
| U3.10 (B3) | `GND` (drives the idle A3 low) |
| U3.15, U3.16 (DIR1, DIR2) | `GND` (B→A) |
| U3.5 (DIR3) | `GND` |
| U3.6 (DIR4) | `1V8` (A→B: sensor MISO to host) |
| U5.5 (DIR) | `GND` ⇒ B→A, i.e. NCS_H (3.3 V) drives NCS_S (1.8 V) |

> **Verify before netlisting:** the PCB manufacturing guideline says "DNC pins
> must be left unconnected", while DS14161 Table 3 is understood to require
> RSVD pins tied to GND. Open DS14161 Table 3 and confirm A6/A7/C6. If they are
> marked *reserved, do not connect*, delete the GND connection; if they are
> marked *connect to GND*, keep it. Do the same for any pad the datasheet calls
> DNC. This is the only pin-level ambiguity in the design.

---

## 3. Schematic capture, step by step

### 3.1 Sensor symbol (U1)

1. Use the ST land pattern / symbol with pads A1–A7, B1, B4, B7, C1–C7
   (16 pads). B4 is the **thermal and ground pad**, not a signal.
2. Place the decoupling capacitors so the schematic reflects the layout
   intent: C1 at C4 (AVDD), C3 at A3 (IOVDD), C2 at B7 (CORE_1V8).
3. Do not route INT, SYNC or LPn off-board. They are pull-ups only.
4. Add a text note on the sheet: *"I/O is 1.8 V. Never connect a host pin
   directly. Absolute max on CORE_1V8/IOVDD is 1.98 V."*

### 3.2 LDO (U2, MIC5504-1.8)

1. `IN` = `3V3`, `OUT` = `1V8`, `GND` = `GND`, `EN` = `IN`.
2. C6 (2.2 µF) at IN, C7 (1 µF) at OUT, both within a few mm in layout.
3. UDFN pinout is **1 = VOUT, 2 = GND, 3 = EN, 4 = VIN, exposed pad = GND**
   (confirmed from the Microchip datasheet). The EP is not optional — connect it.
4. Add a note: *"EN tied to IN — the 3V3 rail edge does the sequencing."*

### 3.3 SPI translator (U3, SN74AXC4T774)

1. VCCA = `1V8` (pin 14), VCCB = `3V3` (pin 13), GND = pin 8.
2. OE = pin 7 = `NCS_S`. Active low: NCS high ⇒ U3 Hi-Z.
3. Channel map (A side is the 1.8 V sensor, B side the 3.3 V host):
   - A1/B1 = SCLK, DIR1 = GND (B→A)
   - A2/B2 = MOSI, DIR2 = GND (B→A)
   - A3/B3 = unused; B3 to GND, A3 no connect, DIR3 = GND
   - A4/B4 = MISO, DIR4 = `1V8` (A→B)
4. C4 (100 nF) at VCCA, C5 (100 nF) at VCCB.
5. **Confirm the RSV (UQFN-16) pin numbers on TI's own pinout figure.** The
   current assignment comes from a secondary mirror and matches the netlist, but
   it has not been checked against TI's drawing.

> **Do not** route NCS through U3. If NCS is on the A/B pair that U3 gates, the
> board can never be selected because OE is derived from that same NCS.

### 3.4 NCS translator (U5, SN74AXC1T45)

1. VCCA = pin 1 = `1V8`, GND = pin 2, A = pin 3 = `NCS_S`, B = pin 4 = `NCS_H`,
   DIR = pin 5 = `GND`, VCCB = pin 6 = `3V3`.
2. **DIR = GND selects B→A**, so the 3.3 V host side drives the 1.8 V sensor
   side. This is the required direction, and the arrow in older text that said
   "A→B" was wrong.
3. C8 (100 nF) at VCCA, C9 (100 nF) at VCCB.
4. Pin numbers and direction are **confirmed** from the TI datasheet (residual R5
   is closed). If you hand-assemble, prefer DCK (SC70-6) or DBV (SOT-23-6); the
   X2SON-6 DTQ part is 1.0 × 0.8 mm.

### 3.5 Passives

| Ref | Value | From | To | Placement intent |
|---|---|---|---|---|
| R1 | 47 kΩ | `NCS_S` | `1V8` | Holds NCS_S high while U5 is indeterminate |
| R2 | 47 kΩ | `U1.A1` | `1V8` | INT unused |
| R3 | 47 kΩ | `U1.B1` | `1V8` | SYNC unused (per-acquisition trigger; do not tie boards together) |
| R4 | 47 kΩ | `U1.A2` | `1V8` | LPn high = enabled |
| R5 | 47 kΩ | `U1.C1` | `1V8` | SPI mode |
| R6 | 6.8 kΩ | `SCLK_S` | `1V8` | Holds mode-3 idle level so selecting the board cannot clock a bit |
| R7 | 4.7 kΩ | `NCS_H` | `3V3` | Keeps the board deselected during host reset |

R6 and R7 are engineering additions, not datasheet requirements. Do not
increase R6 above 7 kΩ (TI's external pull-up limit against the 71 kΩ internal
pull-down) and do not move R7 to the host.

### 3.6 Header (J1)

1. 1×6, 2.54 mm, THT, pin 1 = `3V3_EXT`.
2. Pin order must be **3V3_EXT, GND, SCLK, MOSI, MISO, NCS**. A keyed or
   shrouded header is fine if the harness matches.
3. Add a silk label for pin 1 and for the signal names. Five identical boards
   will be plugged in blind.

### 3.7 System power sheet (one per system)

1. 3.3 V input → ≥ 47 µF bulk → U10 VIN (with C10 22 µF) → U10 VOUT
   (with C11 1 µF) → `3V3_EXT` to all five carriers.
2. U10 = TPS22918 class, ≥ 1.5 A, **CT pin open** (t_R ≈ 95 µs).
3. R10 = 100 kΩ from ON to GND. The rail must be **off** whenever the host is
   off, resetting, or before firmware runs.
4. ON ← Teensy pin 2 (`RAIL_EN_PIN`).
5. Star ground: one point at the supply; five separate ground wires to the
   carriers; Teensy GND joins only at that point. Never join the Teensy 3.3 V
   pin to `3V3_EXT`.
6. The bulk capacitor stays **upstream** of U10. Downstream bulk raises inrush
   from ≈1.27 A toward ≈2.9 A.

---

## 4. Schematic review (ERC and human checks)

Run ERC, then check each row by hand. Every row must pass before layout.

| # | Check | Pass criterion |
|---|---|---|
| S1 | `3V3` reaches sensor AVDD, both translator VCCBs and the LDO input | One net, no series part |
| S2 | `1V8` reaches sensor IOVDD + CORE_1V8, both translator VCCAs, all pull-ups | One net |
| S3 | No 3.3 V net touches any sensor pin except through U3/U5 | Zero exceptions |
| S4 | U3.OE = `NCS_S` (not VCC, not GND) | Exactly one connection, to `NCS_S` |
| S5 | U5 DIR = GND, U5 B = `NCS_H`, U5 A = `NCS_S` | Matches the direction table |
| S6 | U3 DIR4 = `1V8`, DIR1/2/3 = GND | — |
| S7 | U3 B3 = GND, A3 = NC | — |
| S8 | R1–R5 = 47 kΩ to `1V8`; R6 = 6.8 kΩ; R7 = 4.7 kΩ to `3V3` | Values, not just references |
| S9 | Every IC has a 100 nF within a few mm in the placement plan | C1–C9 all used, none orphaned |
| S10 | No net named `3V3` is connected to J1 pin 1 | J1 carries `3V3_EXT` |
| S11 | DNC/RSVD pins match DS14161 Table 3 | Documented decision, not a guess |
| S12 | Pin numbers match TI's pinout figures | U5 confirmed (1/2/3/4/5/6); U3 RSV still to check |
| S13 | LDO EN = IN, no per-board enable network | No 2N7002, no RC |
| S14 | No reset line to the sensor | Only the rail switch |
| S15 | No part added or removed vs the 20-part BOM | Count = 20 |

---

## 5. Board mechanical and stackup

There is no frozen outline. Choose one and record it, then reuse it for all five.

Suggested starting point for a hand-solderable, single-sensor carrier:

- Outline ≈ 22–26 mm × 12–15 mm, 1.6 mm FR4, 2 layers, 1 oz copper.
- Sensor U1 at one end, optical axis clear of every other part and of the board
  edge (the 45°×45° FoV plus the collector exclusion cone must not see the
  carrier, the harness, or the next board).
- J1 on the opposite edge, pointing away from the sensor.
- U3 and U5 adjacent to each other, both close to U1.
- LDO on the J1 side, in copper.
- Two M2 mounting holes, or a 3D-printed frame — the optical axis must be
  repeatable across the five boards.
- Fiducials: two global ones for the panel if you stencil-assemble.

Surface finish: **ENIG preferred** for the LGA land flatness; HASL works but
gives a less planar land. Minimum trace 0.15 mm, via 0.3 mm drill / 0.6 mm pad.

---

## 6. Sensor land pattern and thermal pad (the critical part)

This is where the board is most likely to fail. Follow the primary documents,
not intuition.

1. **Use ST's land pattern** from DS14161 (recommended solder pattern figure)
   for the 14 signal pads. Do not invent or scale it.
2. **NSMD pads** (solder-mask defined, i.e. mask opening larger than the copper
   land). The manufacturing guideline explicitly recommends opening the mask
   external to the land.
3. **Symmetric design.** Keep copper distribution and paste area symmetric around
   the sensor so it self-centres and does not tilt during reflow.
4. **Keep-out below the footprint for signals.** No signal vias or traces under
   the sensor body.
5. **Thermal pad B4:** do not use one large paste aperture. Per AN5897 Fig. 3,
   split the thermal pad paste into **eight rectangles** around a central group
   of vias to GND (8 vias, 0.3 mm class, plugged or tented from the bottom).
   This is both a thermal and a paste-volume control measure.
6. **Stencil 90–150 µm**, uniform paste thickness, SPI-controlled if the
   assembler can. Paste must be flat and even; rough blobs on the LGA land cause
   tilted or open joints.
7. **No wet cleaning and no conformal coating** after assembly. If you must
   coat, mask the optical windows with a gasket/cap.

> The "no vias under the footprint" rule and "vias in the thermal pad" are not
> in conflict: the keep-out applies to signal routing, the thermal vias are the
> deliberate exception inside B4.

---

## 7. Placement

Order of priority:

1. **U1** at its mechanical/optical position (fixed by the enclosure or frame).
2. **C1, C2, C3** — AVDD, CORE_1V8, IOVDD — within 2–3 mm of their pins, on the
   same side as the sensor if possible.
3. **U3** close to U1 (the 1.8 V SPI nets are the short ones), **U5** beside U3
   so `NCS_S` is a few millimetres.
4. **U2** with C6/C7, in copper, away from the sensor's optical axis.
5. **R1–R7** near the pin they bias; R6 at `SCLK_S`, R7 at `NCS_H`.
6. **J1** at the edge, pin 1 marked.

Thermal: the LDO dissipates ≈135 mW at full load. Give U2 a copper pour on both
layers connected by vias; this is the only deliberately warm part.

---

## 8. Routing rules

| Net | Rule |
|---|---|
| `SCLK_H`, `MOSI_H`, `MISO_H` | Route as a group, short and direct. Keep `SCLK_H` away from `MISO_H` (no long parallel run). 3 MHz makes impedance control unnecessary |
| `MISO_H` | One driver at a time (guaranteed by OE = NCS_S). Keep the trace off other switching nets |
| `NCS_H` | Direct from J1 to U5; R7 at the U5 end |
| `NCS_S` | **Shortest 1.8 V net on the board.** It drives U3's OE and the sensor's NCS. Keep it away from `SCLK_S` |
| `SCLK_S` | Short; R6 at the sensor end or at U3 A1 |
| `MOSI_S`, `MISO_S` | Short, direct |
| `3V3` | ≥ 0.5 mm where possible, or a small pour; C1 at the sensor AVDD pin |
| `1V8` | Pour or ≥ 0.4 mm; C7 at the LDO, C2/C3 at the sensor |
| `GND` | Continuous ground pour on the bottom layer, stitched to top around the sensor and the LDO |

Two-layer stackup: signal + components on top, solid ground on the bottom. Do
not split the ground under the sensor. Do not route the 1.8 V rail as a long
thin trace across the board.

Optional: 22 Ω series resistors at the **host** end of SCLK and MOSI, one set
only, if scoping shows ringing. Do not fit them pre-emptively.

---

## 9. Fabrication outputs

- Gerbers: 2 layers, 1.6 mm, 1 oz, ENIG preferred.
- BOM: `BOM.csv` in this folder — one file, carrier and system rows, DNP options
  at Qty 0. Send the carrier rows to the assembler, not the whole file.
- Drill file with the 0.3 mm thermal vias called out (plugged or tented).
- **Stencil**: 90–150 µm, with the thermal-pad apertures split into the eight
  rectangles; do not let the stencil house merge them into one opening.
- Assembly drawing: mark U1 pin 1 and the B4 thermal pad, and print the cleaning
  and coating prohibition on it (see §10).
- Panelize the five carriers **identically oriented** so the optical axis and
  connector side match after separation. Add fiducials to the panel.
- Provide the assembler with: MSL3 handling note, reflow limits, no-wash /
  no-coat instruction, and the sensor rework limits.

---

## 10. Assembly and reflow

| Item | Requirement |
|---|---|
| MSL | Level 3. Use within 18 months. Store 16–27 °C, 40–60 % RH |
| Baking | If the humidity indicator card is > 10 %: bake 90 °C for > 6 h (sensor only; never bake the tape-and-reel at 90 °C) |
| Reflow | 240 °C recommended, 260 °C maximum, **3 passes max**, convection only, not vapour phase |
| Stencil | 90–150 µm, uniform paste |
| Cleaning | **No wet cleaning** of the sensor |
| Coating | **Not recommended.** If required, mask the optical windows with a gasket/cap |
| Environment | Class 1000 recommended if using the non-liner sensor variant |
| Rework: desolder | ≤ 275 °C on the cap, < 60 s, protective cube |
| Rework: resolder | ≤ 260 °C (set the station to 250 °C), stencil fixture |
| Fourth reflow | Not guaranteed — replace the sensor and recalibrate |

Do not use hot air without a protective cube around the sensor for nearby
rework. Do not exceed 260 °C on the cap under any circumstance.

---

## 11. Optical integration

- FoV is **45°×45°** (65° diagonal), 940 nm. The collector exclusion cone is
  larger (≈57.6°×57.6°), so the mechanical keep-out is bigger than the FoV.
- If a cover glass or window is used: keep total air gap + glass **< 1.5 mm**;
  no gasket needed below 0.5 mm air gap; a gasket is required above 0.7 mm.
  Crosstalk target ≤ 300 kcps/SPAD.
- The aperture calculator in `../reference docs/Multizone_Cover_Window_Aperture_Calculator.md`
  gives a starting single-rectangle aperture (6.60 × 2.47 mm, offset 0.07 mm in Y)
  for this sensor family — verify against your window geometry.
- **Any cover glass requires crosstalk calibration in firmware**
  (`calibrate_xtalk` / `set_caldata_xtalk`) with a target ≥ 600 mm.
- Keep adjacent sensors' fields of view from overlapping. Five emitters at 940 nm
  with no shared SYNC line are not characterised for mutual interference. SYNC
  is a per-acquisition trigger; never tie all five SYNC pins together.
- Laser class 1: invisible radiation. Do not stare into the emitter; do not
  magnify it with optics.

---

## 12. Layout review checklist (before ordering)

| # | Check | Pass criterion |
|---|---|---|
| L1 | Land pattern matches DS14161 | Copied, not redrawn |
| L2 | Mask openings external to the lands (NSMD) | Visual/DRC |
| L3 | Thermal pad = 8 paste windows + GND vias | Matches AN5897 Fig. 3 |
| L4 | No signal via/trace under the sensor body | DRC keep-out |
| L5 | C1/C2/C3 within 3 mm of their sensor pins | Measured |
| L6 | U3 and U5 within ~5 mm of U1; `NCS_S` < 5 mm | Measured |
| L7 | `SCLK_H` is not a long parallel neighbour of `MISO_H` | Visual |
| L8 | `NCS_S` does not run alongside `SCLK_S` | Visual |
| L9 | Solid GND on the bottom layer, no splits under U1/U3 | Visual |
| L10 | 3V3 and 1V8 copper sized for 150 mA with margin | Visual |
| L11 | U2 has copper on both layers + vias | Visual |
| L12 | J1 pin 1 and signal names on silkscreen | Visual |
| L13 | Optical axis clear of every component and the board edge | Visual + 3D |
| L14 | Stencil layer contains the split thermal apertures | Check the fab output |
| L15 | DRC clean at the fab's minimums; ERC clean | Reports archived |

---

## 13. After fabrication

1. Visual: no paste bridges on the LGA, thermal pad wetted, no tombstoned
   passives.
2. DMM: `3V3`–`GND` not shorted, `1V8`–`GND` not shorted. Power the first board
   from a current-limited supply (100 mA limit) before connecting the Teensy.
3. With the rail on, measure `1V8` = 1.755–1.845 V and AVDD = 3.13–3.47 V.
4. Scope the rail edge and the CORE_1V8 window (pass: ≤ 58.3 µs for 0.9→1.6 V).
5. Run `single_sensor_bringup`; a successful start confirms the block size and
   that the trim is accepted.
6. Then follow `../README.md` §8 (bring-up checklist) for the full sequence.

Only build the I²C carrier variant if you deliberately choose Mode B; it needs
a TCA9406, different strap resistors (SPI_I2C_N low, I2C_RST low or GPIO) and
I²C pull-ups on both rails. The production build is Mode A.

---

## 14. Do-not list

- Do not connect the Teensy 3.3 V pin to `3V3_EXT`.
- Do not route NCS through U3.
- Do not tie U3.OE to GND or VCC; it must be `NCS_S`.
- Do not fit a pull-down on SCLK_S or any AXC data I/O.
- Do not move R7 to the host side.
- Do not add a per-board EN network or a sensor reset line.
- Do not add bulk capacitance downstream of the load switch.
- Do not use a CT capacitor on the load switch unless the scope says t_R is
  too slow.
- Do not wash or coat the board.
- Do not exceed 260 °C on the sensor cap, or reflow more than three times.
- Do not put vias or traces under the sensor body (thermal-pad vias excepted).
- Do not tie the five SYNC pins together.
- Do not shrink the stencil thermal aperture into one opening.