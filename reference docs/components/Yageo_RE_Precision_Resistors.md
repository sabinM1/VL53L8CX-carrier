# SN74AXC1T45 — Single-Bit Dual-Supply Bus Transceiver

Source file: `sn74axc1t45.pdf`

## Features
- Up/down translation across 0.65 V to 3.6 V
- Operating temperature: -40°C to +125°C
- Glitch-suppression circuitry for improved power sequencing
- Maximum quiescent current \((I_{CCA}+I_{CCB})\): 10 µA at 85°C max, 16 µA at 125°C max
- Up to 500 Mbps when translating 1.8 V to 3.3 V
- \(V_{CC}\) isolation: if either \(V_{CC}\) is below 100 mV, all I/Os are disabled and high-impedance
- \(I_{off}\) supports partial-power-down
- Latch-up performance exceeds 100 mA per JESD 78, Class II
- ESD: ±8000 V HBM, ±1000 V CDM

## Applications
- Enterprise and communications
- Industrial
- Personal electronics

## Description
The SN74AXC1T45 is a single-bit non-inverting bus transceiver using two configurable supply rails. \(V_{CCA}\) and \(V_{CCB}\) can both be as low as 0.65 V. A port tracks \(V_{CCA}\); B port tracks \(V_{CCB}\).

DIR determines direction:
- DIR HIGH: A → B
- DIR LOW: B → A

DIR is referenced to \(V_{CCA}\).

The \(I_{off}\) circuitry prevents excessive current in partial-power-down. \(V_{CC}\) isolation places both I/O ports in high-impedance if either \(V_{CCA}\) or \(V_{CCB}\) is <100 mV. Glitch suppression allows either rail to power on/off in any order.

## Package Information

| Part Number | Package | Package Size |
|---|---|---|
| SN74AXC1T45DBV | SOT-23, 6 | 2.9 mm × 2.8 mm |
| SN74AXC1T45DCK | SC70, 6 | 2 mm × 2.1 mm |
| SN74AXC1T45DRL | SOT-5X3, 6 | 1.6 mm × 1.6 mm |
| SN74AXC1T45DEA | X2SON, 6 | 1 mm × 1 mm |
| SN74AXC1T45DTQ | X2SON, 6 | 1 mm × 0.8 mm |

## Pin Functions

| Pin | Name | Type | Description |
|---|---|---|---|
| 1 | \(V_{CCA}\) | — | A-port supply, 0.65 V to 3.6 V |
| 2 | GND | — | Ground |
| 3 | A | I/O | A-port input/output; referenced to \(V_{CCA}\). Do not float when input. |
| 4 | B | I/O | B-port input/output; referenced to \(V_{CCB}\). Do not float when input. |
| 5 | DIR | I | Direction control: High = A→B; Low = B→A |
| 6 | \(V_{CCB}\) | — | B-port supply, 0.65 V to 3.6 V |

## Absolute Maximum Ratings

| Parameter | Min | Max | Unit |
|---|---:|---:|---|
| \(V_{CCA}\) | -0.5 | 4.2 | V |
| \(V_{CCB}\) | -0.5 | 4.2 | V |
| Input voltage: A, B, control | -0.5 | 4.2 | V |
| Output voltage, high-Z/power-off | -0.5 | 4.2 | V |
| Output voltage, high/low: A | -0.5 | \(V_{CCA}+0.2\) | V |
| Output voltage, high/low: B | -0.5 | \(V_{CCB}+0.2\) | V |
| Input clamp current | — | -50 | mA |
| Output clamp current | — | -50 | mA |
| Continuous output current | -50 | 50 | mA |
| Continuous current through \(V_{CC}\)/GND | -100 | 100 | mA |
| \(T_J\) | — | 150 | °C |
| \(T_{STG}\) | -65 | 150 | °C |

## ESD Ratings

| Rating | Value | Unit |
|---|---:|---|
| HBM | ±8000 | V |
| CDM | ±1000 | V |

## Recommended Operating Conditions

| Parameter | Min | Max | Unit |
|---|---:|---:|---|
| \(V_{CCA}\) | 0.65 | 3.6 | V |
| \(V_{CCB}\) | 0.65 | 3.6 | V |
| \(V_I\) input voltage, active/tri-state | 0 | 3.6 | V |
| Input transition rate | — | 100 | ns/V |
| \(T_A\) | -40 | 125 | °C |

## Electrical Characteristics (Representative)

| Parameter | Test Conditions | Min/Typ/Max | Unit |
|---|---|---|---:|
| \(V_{OH}\) | \(I_{OH}=-100\ \mu A\), 0.7–3.6 V | \(V_{CC}-0.1\) | V |
| \(V_{OH}\) | \(I_{OH}=-12\ mA\), 3 V | 2.3 | V |
| \(V_{OL}\) | \(I_{OL}=100\ \mu A\) | 0.1 | V |
| \(V_{OL}\) | \(I_{OL}=12\ mA\), 3 V | 0.7 | V |
| \(I_I\) | Control input | ±1 µA | µA |
| \(I_{off}\) | A/B port | ±4 µA | µA |
| \(I_{CCA}\) | \(I_O=0\) | 5 µA typ, 7.5 µA max | µA |
| \(C_I\) | Control input | 4.4 pF | pF |
| \(C_{IO}\) | A/B I/O | 5 pF | pF |

## Switching Characteristics
- Tables provided for \(V_{CCA}\) = 0.7 V, 0.8 V, 0.9 V, 1.2 V, 1.5 V, 2.5 V, 3.3 V.
- Supports up to 500 Mbps for 1.8 V → 3.3 V.

## Device Functional Modes

| DIR | Operation |
|---|---|
| L | B data to A bus |
| H | A data to B bus |

## Application Information
Used for interfacing systems with different interface voltages. Maximum data rate up to 500 Mbps for 1.8 V → 3.3 V.

### Enable Times
\[
t_{A\_en}(DIR\ to\ A)=t_{dis}(DIR\ to\ B)+t_{pd}(B\ to\ A)
\]
\[
t_{B\_en}(DIR\ to\ B)=t_{dis}(DIR\ to\ A)+t_{pd}(A\ to\ B)
\]

In bidirectional applications, take care to avoid bus contention when changing direction.

## Power Supply Recommendations
- Two separate rails: \(V_{CCA}\), \(V_{CCB}\), each 0.65 V to 3.6 V.
- Connect ground before supplies.
- \(V_{CCA}\) and \(V_{CCB}\) may ramp in any order.

## Layout Guidelines
- Use bypass capacitors on supplies.
- Use short trace lengths to avoid excessive loading.
- Place pads for loading capacitors/pull-up resistors to adjust rise/fall times if needed.