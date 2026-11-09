# Next steps — schematic, PCB, build, bring-up

Design frozen at **Rev 8**. This file is the execution checklist from here to
five working carriers. The detailed capture/layout instructions live in
`pcb design/Schematic_and_PCB_Guide.md`; this file is the order of operations,
the gates, and the bench procedure.

Everything below assumes the production topology: **Mode A** (one SPI bus, five
CS lines). Mode B and Mode C are documented in `README.md` §3 for reference but
are not implemented and are not part of any checklist here.

---

## 0. Tooling

### 0.0 Measurement matrix (read this first)

Your kit: **portable DSO 200 kHz / 2.5 MS/s**, **LogicAnalyzer V6.0**,
**DMM**, bench supply, USB serial. Every measurement in the build is listed
here with the instrument that closes it and whether your kit is sufficient.

| # | Measurement | Target | Instrument | Sufficient? |
|---|---|---|---|---|
| 1 | J1 continuity; no `3V3_IN`–GND or `3V3_SW`–GND short, and `3V3_IN` not shorted to `3V3_SW` | — | DMM | **Yes** |
| 2 | Harness round-trip resistance | < 0.15 Ω | DMM | **Yes** |
| 3 | Per-board 1V8 | 1.755–1.845 V | DMM | **Yes** |
| 4 | Far-board AVDD under load (R9) | ≥ 3.13 V | DMM | **Yes** |
| 5 | Supply current, all ranging | 400–550 mA | Bench supply on the DC1 input, or DMM in series at the DC1 output | **Yes** |
| 6 | AVDD 0→3.13 V edge | ≤ 3.3 ms | Portable DSO | **Yes** |
| 7 | CORE_1V8 0.9→1.6 V window (R2) | ≤ 58.3 µs | Portable DSO | **Yes** |
| 8 | Load-switch t_R, CT open | ≤ 275 µs (expect ~95 µs) | Portable DSO | **Yes** |
| 9 | QOD discharge after ON low | < 0.33 V in 10 ms | Portable DSO | **Yes** |
| 10 | MISO_H idle, five CS high (R6) | ≈ 0 V | Portable DSO or DMM | **Yes, with caveat** (10 MΩ probe can mask a true float; confirm with the 1 MΩ pull-up test if in doubt) |
| 11 | SPI SCK frequency and duty (R8) | 2.85–3.15 MHz, 40–60 % | LogicAnalyzer V6.0, 8 ch @ 100 Msps | **Yes** |
| 12 | NCS fall → first SCK edge | ≥ 0.9 µs | LogicAnalyzer V6.0 | **Yes** |
| 13 | SPI mode-3 idle levels, CS frames, 320 B decode | clean | LogicAnalyzer V6.0, 24 ch @ ≥ 20 Msps | **Yes** |
| 14 | One MISO driver / OE timing | only selected board drives | LogicAnalyzer V6.0 | **Yes** |
| 15 | Frame rate / CSV cadence | ≈ 15 lines/s | Serial terminal + host timer | **Yes** |
| 16 | `start_ranging` OK (R3) | status 0 | Serial terminal | **Yes** |
| 17 | Re-init while streaming (R4) | frames resume | Serial terminal, send `r` | **Yes** |
| 18 | Remove/restore a sensor | −1, then recovery | Serial terminal | **Yes** |
| 19 | 1 h soak, current drift | < 10 % | Bench supply / DMM | **Yes** |
| 20 | LDO temperature (optional) | no hotspot | Touch or thermal camera | Optional |

Every measurement in this project is covered by your kit. There is no scope-gap
row: Mode B (the only thing that would need a faster scope) is not implemented.

### 0.1 LogicAnalyzer V6.0 — what it can and cannot do

The gusmanb **LogicAnalyzer V6.0** (RP2040/RP2350) is a **digital** instrument:
24 channels, up to 100 Msps on RP2040 (400 Msps blast mode on RP2350), 131 071
samples in 8-channel mode / 32 767 in 24-channel mode. Inputs are 3.3 V logic by
default; the V6.0 board has a 3.3 V / 5 V / external-VREF selector, and the
level-shifter board accepts 1.65–5.5 V logic. For 1.8 V nets, set external VREF
to the board's 1V8 rail.

It cannot replace an oscilloscope for this project:

| Item | Logic analyzer? | Why |
|---|---|---|
| R2 CORE_1V8 0.9→1.6 V ≤ 58.3 µs | **No** | One digital threshold; cannot time the 0.9 V and 1.6 V crossings |
| R6 MISO_H idle ≈ 0 V | **No** | Reports 0/1 against a threshold, not the DC level of a floating node |
| R8 SPI clock 2.85–3.15 MHz, duty 40–60 % | **Yes** | Digital timing; use **8-channel mode at 100 Msps** for duty resolution |
| R9 far-end AVDD ≥ 3.13 V | **No** | Needs a DMM (or a scope) |

It *is* the right tool for the digital checks the procedure also needs:
CS-to-first-clock ≥ 1 µs, SPI mode-3 idle levels, one-driver-at-a-time MISO,
`NCS_S`/OE timing, and decoding a 320 B frame.

### 0.2 Minimum bench set

| Tool | Use |
|---|---|
| LogicAnalyzer V6.0 + probes | R8, SPI/I²C decode, CS/OE timing |
| Portable DSO, 200 kHz / 2.5 MS/s (10× probe) | **R2 and R6 only** — see §0.3 |
| DMM | R9, continuity, harness resistance, 1V8 accuracy |
| Bench supply 0–15 V, ≥ 2 A, current limit | Feeds DC1 during bring-up instead of the 3S battery; set 2 A for inrush |
| USB serial terminal | R3, R4, CSV capture |
| Teensy 4.0 + USB cable | Host |
| Iron / hot air, 30 AWG wire | Test points on J1 and the 1V8 net |

### 0.3 Portable DSO (200 kHz, 2.5 MS/s) — what it covers

Bandwidth 200 kHz ⇒ displayed rise time ≈ 0.35 / 200 kHz = **1.75 µs**;
sampling 2.5 MS/s ⇒ 0.4 µs per point. That is enough for the slow power-tree
checks and nothing fast:

| Check | Portable DSO? | Why |
|---|---|---|
| R2 CORE_1V8 0.9→1.6 V ≤ 58.3 µs | **Yes** | The edge is ~25–49 µs; 1.75 µs of smoothing is a few percent and errs long (conservative) |
| Load-switch t_R ≈ 95 µs, QOD decay τ ≈ 1 ms | **Yes** | Both are ≫ 1.75 µs |
| R6 MISO_H idle ≈ 0 V | **Yes, with a caveat** | DC measurement. A 10 MΩ probe can pull a *truly floating* node to 0 V and hide the fault. The design biases it through 14.2 kΩ, so a real float is unlikely; if in doubt, lift the probe and confirm with a 1 MΩ pull-up test or a DMM |
| R8 SPI 3 MHz duty | **No** | 200 kHz cannot see 3 MHz. Use the LogicAnalyzer |
| R9 AVDD ≥ 3.13 V | **No** | Use the DMM |

So the portable scope plus the V6.0 logic analyzer covers R2, R3, R4, R6 and R8.
No measurement in the implemented design needs a faster scope.

---

## 1. Remaining procurement

Carrier BOM is fully sourced (`pcb design/BOM.csv`). Only three system parts
remain, and **there is no separate power board** — the load switch now lives on
each carrier.

| Ref | Spec to source | Notes |
|---|---|---|
| DC1 | 3S (9.0–12.6 V) → 3.3 V buck, ≥ 1 A | Pololu **#5266** (APM81815, 3.3 V, 10.2 × 19.1 × 3.2 mm, ≈ USD 7). 1.1 A at 48 V in and *more* at 3S; 2 % accuracy; soft-start; no reverse-voltage protection |
| R10 | 100 kΩ, 0402 | **One** resistor at the host, RAIL_EN → GND. The TPS22918 has no internal pull-down |
| CB1 | ≥ 47 µF, **≥ 25 V** | At the **DC1 input** (battery side) for LC-spike suppression. Not a 3.3 V part |
| — | Stencil, 90–150 µm | Split thermal-pad apertures (AN5897 Fig. 3) |

### 1.1 Power tree (changed from Rev 8)

The single external switch board is gone. Each carrier carries its own
**U10 (TPS22918)** between the header and everything else:

```
3S LiPo ──► CB1 ──► DC1 (3.3 V buck) ──► 3V3_IN on all five J1-1
                                              │
                              per carrier: C10 (4.7 µF)
                                              │
                                          U10 (TPS22918)
                                              │
                              3V3_SW ──► U1 AVDD, U2 VIN,
                                         U3 VCCB, U5 VCCB, R7
                                              │
                                    U2 ──► 1V8 ──► CORE_1V8, IOVDD, VCCA
```

- **RAIL_EN** is J1-7 on every carrier, wired in parallel, driven by Teensy
  pin 2. One 100 kΩ (R10) at the host holds it low until firmware raises it.
- **CT open, no footprint. QOD strapped to VOUT.** Same as Rev 8 — the
  internal 25 Ω discharges the local rail when ON drops.
- Why this is better than the Rev-8 external switch: **AVDD is now switched
  too**, so a power cycle removes AVDD and the 1.8 V rail together (DS14161
  requires exactly that), the switch drop falls from 40–80 mV to ~5 mV, and
  there is no sixth assembly to build.
- Cost: **+2 parts per carrier** (U10, C10) and **J1 becomes 1×7**. Per-board
  count is now 5 IC + 7 R + 10 C = **22**.

### 1.2 Ordering rule, still mandatory

`RAIL_EN` must be asserted **before** any CS pin is driven. With the rail off,
a driven NCS_H sinks ~0.7 mA per board through R7 into the dead 3V3_SW node.
`setup()` already raises pin 2 first, before `park_all_ncs_high()`.

---

## 2. Schematic design

Work on one carrier sheet; instantiate it five times. Use the detailed steps in
`pcb design/Schematic_and_PCB_Guide.md` §3.

- [ ] **2.1** Create the project, title block, and **one** sheet: `carrier`.
      There is no `system_power` sheet any more — the switch is on the carrier
      and the buck module (DC1) is an off-board part wired to J1-1.
- [ ] **2.2** Place U1 (VL53L8CX), U2 (MIC5504-1.8, UDFN-4), U3
      (SN74AXC4T774RSVR, RSV), U5 (SN74AXC1T45DTQR, X2SON-6), U10
      (TPS22918TDBVRQ1, SOT-23-6) and J1 (1×7).
- [ ] **2.3** Enter the verified pin numbers, not a generic symbol:
      - U3: VCCA 14, VCCB 13, GND 8, OE 7, A1–A4 1–4, B1–B4 12/11/10/9,
        DIR1/2 15/16, DIR3/4 5/6.
      - U5: 1 VCCA, 2 GND, 3 A, 4 B, 5 DIR, 6 VCCB; **DIR = GND ⇒ B→A**.
      - U2: 1 VOUT, 2 GND, 3 EN, 4 VIN, EP = GND.
      - U10: 1 VIN, 2 GND, 3 ON, 4 CT, 5 QOD, 6 VOUT.
      - J1: 1 3V3_IN, 2 GND, 3 SCLK, 4 MOSI, 5 MISO, 6 NCS, 7 RAIL_EN.
- [ ] **2.4** Capture the nets exactly as listed in the guide §2.1–§2.3. The
      load-bearing connection: **`NCS_S` drives U1.C2, R1, and U3.OE (pin 7)**.
      NCS must not pass through U3.
- [ ] **2.5** Straps: R1–R5 47 kΩ to `1V8`, R6 6.8 kΩ `SCLK_S`→`1V8`, R7 4.7 kΩ
      `NCS_H`→`3V3_SW` (**switched** rail, not 3V3_IN). A6/A7/C6 (`RSVD1–3`) to
      GND. C1 `SPI_I2C_N` via R5 to 1V8.
- [ ] **2.6** Decoupling: C1 at U1.C4, C2 at B7, C3 at A3, C4/C5 at U3, C6/C7 at
      U2, C8/C9 at U5, **C10 at U10.1 (3V3_IN)**. Values per BOM.
- [ ] **2.7** On-carrier power: **J1-1 (3V3_IN) → C10 → U10 → 3V3_SW**, and
      3V3_SW feeds U1 AVDD, U2.VIN, U3.VCCB, U5.VCCB and R7. U10: **CT open,
      no footprint; QOD (5) strapped to VOUT (6); ON (3) = J1-7 RAIL_EN**. U2.EN
      tied to U2.IN. R10 and CB1 are host-side and not on this sheet.
- [ ] **2.8** ERC: no unconnected pins except U3.A3 (no connect) and U10.CT
      (no footprint). Check power flags on `3V3_IN`, `3V3_SW`, `1V8`, `GND` —
      these are four distinct nets; do **not** merge 3V3_IN and 3V3_SW.
- [ ] **2.9** Human review against guide §4 and the do-not list §14.
- [ ] **2.10** Generate the netlist and cross-check every BOM reference and
      value. Tag the schematic `rev8-schematic`.

**Gate:** ERC clean, netlist diffed against the guide, and U1/U3/U5/U10 pin
numbers checked against the datasheet figures.

---

## 3. PCB layout

Follow `pcb design/Schematic_and_PCB_Guide.md` §5–§12. Order matters:

- [ ] **3.1** Stackup: 2-layer FR4, 1.6 mm, 1 oz, ENIG. Board outline and
      mounting holes fixed before placement.
- [ ] **3.2** Placement: U1 first (optical axis clear of every part and the
      board edge), then C1/C2/C3 within 3 mm of their pins, then U2/U3/U5/U10
      within ~5 mm, C10 within 3 mm of U10.1, then J1.
- [ ] **3.3** Sensor land pattern copied from DS14161, not redrawn. Thermal pad
      = 8 paste windows + GND vias, AN5897 Fig. 3. NSMD mask openings.
- [ ] **3.4** Route `NCS_S` first (short, U5.3 → U1.C2 / U3.7 / R1). Keep it
      away from `SCLK_S`.
- [ ] **3.5** Route SPI: `SCLK_S`, `MOSI_S`, `MISO_S` short and direct; no long
      parallel run of `SCLK_H` next to `MISO_H`.
- [ ] **3.6** Power: `3V3_IN` (J1-1 → C10/U10.1) and `3V3_SW` (U10.6 → the
      rest) sized for 150 mA with margin, joined **only** through U10; `1V8`
      sized for 100 mA; solid GND on the bottom layer, no splits under
      U1/U3; U2 and U10 copper on both layers with vias.
- [ ] **3.7** Silkscreen: J1 pin 1 and all **seven** pin names (3V3_IN, GND,
      SCLK, MOSI, MISO, NCS, RAIL_EN); board ID and revision. There is no
      moulded pin-1 mark on the header.
- [ ] **3.8** DRC at the fab minimums; archive the report.
- [ ] **3.9** Outputs: Gerbers, drill, BOM, pick-and-place, stencil with split
      thermal apertures.
- [ ] **3.10** Run the layout review checklist (guide §12, L1–L15).

**Gate:** DRC clean, thermal pad and stencil verified visually, optical keep-out
checked in 3D.

---

## 4. Fabrication and assembly

- [ ] Order 5 carriers + 2 spares; system parts separately.
- [ ] Reflow **240 °C recommended / 260 °C max, 3 passes maximum, convection
      only**.
- [ ] Sensor is **MSL 3**: observe floor life; bake at 90 °C > 6 h if the
      humidity card says so.
- [ ] **Remove the protective liner after reflow, before the cover glass.**
- [ ] **No PCB washing, no conformal coating.**
- [ ] Inspect: no paste bridges on the LGA, thermal pad wetted, no tombstoned
      passives, J1 straight and fully seated.

---

## 5. Bench bring-up

Record every measurement; a step is not closed until the number is written down.

### B0 — Harness (DMM)
- [ ] Continuity J1-1…J1-7 on all five; no 3V3_IN–GND short.
- [ ] Harness ≤ 20 cm, one ground per board, round-trip supply R < 0.15 Ω.

### B1 — Rail statics (DMM, all five ranging)
- [ ] DC1 output 3.23–3.37 V (2 % accuracy) at the module.
- [ ] `3V3_IN` at the far J1-1: expect ≥ 3.16 V (2 % of 3.3 V, minus the
      < 0.15 Ω harness at 465 mA ≈ 70 mV).
- [ ] **Far-board AVDD (`3V3_SW`) ≥ 3.13 V.** Budget: 3.234 V worst-case
      module output − 70 mV harness − ~5 mV switch = 3.159 V, so margin is
      only ≈ 30 mV. **R9** — if this fails, shorten the harness, never firmware.
- [ ] Per-board 1V8 = 1.755–1.845 V (absolute floor 1.62 V).
- [ ] Supply current 400–550 mA sustained at the DC1 output (spec 465 mA).
- [ ] Battery draw: ≈ 465 mA × 3.3 V / (0.85 × 11.1 V) ≈ **165 mA** typ at 3S.

### B2 — Power edges (portable DSO; this is R2)
- [ ] Trigger on ON rising; far board first, then all five.
- [ ] Per-board AVDD (`3V3_SW`) 0→3.13 V ≤ 3.3 ms (expect ~95 µs).
- [ ] All five boards reach AVDD within ~1 ms of each other (same RAIL_EN).
- [ ] **CORE_1V8 0.9→1.6 V ≤ 58.3 µs on every board.**
- [ ] Rail rise monotonic with the source current limit at 2.0 A (set it on
      the bench supply at the DC1 input).
- [ ] Drop ON: rail < 0.33 V within 10 ms (QOD τ ≈ 1 ms).

If the CORE window fails: do not fit CT (it only slows the rail). The fallback
is a different LDO or accepting the input-tracking case after measuring it.

### B3 — MISO idle (portable DSO or DMM; this is R6)
- [ ] All five CS high. Expect ≈ 0 V (five 71 kΩ internal pull-downs).
- [ ] If it floats or sits mid-rail, fit the staged **47 kΩ pull-down at the
      host** (never a pull-up).

### B4 — First frames (serial; this is R3)
- [ ] Five `# S%u ranging at 15 Hz` within 15 s of boot (typical ~3 s).
- [ ] No `# … FAILED`.
- [ ] CSV ≥ 12 rows/s; all five `start_ranging` OK. A successful start is the
      acceptance of the 320 B block set.

### B5 — SPI signals (LogicAnalyzer V6.0; this is R8)
- [ ] SCK 2.85–3.15 MHz, duty 40–60 %, idle high (mode 3).
- [ ] NCS fall → first SCK edge ≥ 0.9 µs.
- [ ] No runts; `NCS_S` and OE follow NCS.
- [ ] Decode one 320 B read: address bytes correct, CS frames clean.
- [ ] 8-channel mode at 100 Msps for duty; 24-channel mode at ≥ 20 Msps for
      frame decoding (sample depth is 32 k in 24-channel mode).

### B6 — Remove/restore a sensor (live)
- [ ] Removed: column `-1` within ≤ 3 s with a `# …` line; other four stream.
- [ ] Restored: `# retry` within 30 s, frames within ≤ 40 s total.

### B7 — Forced re-init on a streaming sensor (this is R4)
- [ ] In `single_sensor_bringup`, send `r`.
- [ ] `init` returns OK, frames resume, no hang, no `stop_ranging()`.
- [ ] If this fails, add a rail cycle to the retry path (the switch supports
      it). **This is the only bench item that can force a design change.**

### B8 — Soak
- [ ] 1 h: zero unexplained demotions, current drift < 10 %, CSV continuous.

---

## 6. Closure register

| ID | Item | Closes by | If marginal |
|---|---|---|---|
| R2 | CORE_1V8 window | B2 scope, ≤ 58.3 µs | Different LDO |
| R3 | 320 B block set | B4 successful `start_ranging` | Re-audit the trim |
| R4 | Re-init while streaming | B7 serial `r` | Rail cycle in the retry path |
| R6 | MISO_H idle | B3 scope/DMM | 47 kΩ host pull-down |
| R8 | SPI duty/frequency | B5 logic analyzer | Clock-root change (unlikely) |
| R9 | Far-end AVDD | B1 DMM ≥ 3.13 V | Shorten/upgrade harness; only ≈30 mV of margin now |
| R10 | Build record | Done (Rev 8) | — |

---

## 7. Definition of done

- Five carriers assembled, liner removed, no coating.
- All six CSV columns live at 15 Hz for 1 h with zero unexplained demotions.
- R2–R9 closed with recorded numbers.
- Rail enable and power-cycle recovery demonstrated (drop RAIL_EN, raise,
  frames return).
- This checklist and `README.md` §8–§9 updated with the measured values.