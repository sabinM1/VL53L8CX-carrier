# SN74AXC4T774 — 4-Bit Dual-Supply Bidirectional Voltage-Level Translator

Source file: `sn74axc4t774.pdf`

## Features
- Fully configurable dual-rail design: each port operates from 0.65 V to 3.6 V.
- Operating temperature: -40°C to +125°C.
- Independent direction-control pins for up/down translation.
- Glitch-free power-supply sequencing.
- Up to 310 Mbps when translating from 1.8 V to 3.3 V.
- \(V_{CC}\) isolation: if either \(V_{CC}\) is below 100 mV, all I/Os are disabled and high-impedance.
- \(I_{off}\) supports partial-power-down operation.
- Compatible with AVC-family level shifters.
- Latch-up performance exceeds 100 mA per JESD 78, Class II.
- ESD protection exceeds JESD 22:
  - ±8000 V human-body model (HBM)
  - ±1000 V charged-device model (CDM)

## Applications
- Enterprise and communications
- Industrial
- Personal electronics
- Wireless infrastructure
- Building automation
- Point of sale

## Description
The SN74AXC4T774 is a 4-bit non-inverting bus transceiver with two individually configurable supply rails. Both \(V_{CCA}\) and \(V_{CCB}\) can be as low as 0.65 V. The A port tracks \(V_{CCA}\); the B port tracks \(V_{CCB}\). Both supplies accept 0.65 V to 3.6 V. The device supports asynchronous communication between data buses.

Data direction is controlled by DIRx. OE disables outputs and isolates buses. Control pins DIRx and OE are referenced to \(V_{CCA}\).

To place I/Os in high-impedance during power-up/down, tie OE to \(V_{CCA}\) through a pull-up resistor.

The device is fully specified for partial-power-down using \(I_{off}\). The \(V_{CC}\) isolation feature disables both I/O ports if either \(V_{CCA}\) or \(V_{CCB}\) is less than 100 mV. Glitch-free power sequencing allows either rail to be powered on/off in any order.

## Package Information

| Part Number | Package | Package Size |
|---|---|---|
| SN74AXC4T774PW | TSSOP, 16 | 5 mm × 6.4 mm |
| SN74AXC4T774BQB | WQFN, 16 | 3.5 mm × 2.5 mm |
| SN74AXC4T774RSV | UQFN, 16 | 2.6 mm × 1.8 mm |

## Functional Block Diagram
One of four transceivers:
- A-side referenced to \(V_{CCA}\)
- B-side referenced to \(V_{CCB}\)
- DIRx and \(\overline{OE}\) control data flow and tri-state.

## Pin Functions

| Name | Type | Description |
|---|---|---|
| A1–A4 | I/O | A-port I/O, referenced to \(V_{CCA}\) |
| B1–B4 | I/O | B-port I/O, referenced to \(V_{CCB}\) |
| DIR1–DIR4 | I | Direction-control inputs, referenced to \(V_{CCA}\) |
| \(\overline{OE}\) | I | Tri-state output enable; pull high to place all outputs in tri-state; referenced to \(V_{CCA}\) |
| GND | — | Ground |
| \(V_{CCA}\) | — | A-port supply, 0.65 V to 3.6 V |
| \(V_{CCB}\) | — | B-port supply, 0.65 V to 3.6 V |

## Absolute Maximum Ratings

| Parameter | Min | Max | Unit |
|---|---:|---:|---|
| \(V_{CCA}\) supply voltage | -0.5 | 4.2 | V |
| \(V_{CCB}\) supply voltage | -0.5 | 4.2 | V |
| Input voltage: I/O A, I/O B, control | -0.5 | 4.2 | V |
| Output voltage, high-Z or power-off: A/B | -0.5 | 4.2 | V |
| Output voltage, high/low state: A port | -0.5 | \(V_{CCA}+0.2\) | V |
| Output voltage, high/low state: B port | -0.5 | \(V_{CCB}+0.2\) | V |
| Input clamp current, \(V_I < 0\) | — | -50 | mA |
| Output clamp current, \(V_O < 0\) | — | -50 | mA |
| Continuous output current | -50 | 50 | mA |
| Continuous current through \(V_{CC}\) or GND | -100 | 100 | mA |
| Junction temperature, \(T_J\) | — | 150 | °C |
| Storage temperature, \(T_{stg}\) | -65 | 150 | °C |

## ESD Ratings

| Rating | Value | Unit |
|---|---:|---|
| HBM, ANSI/ESDA/JEDEC JS-001 | ±8000 | V |
| CDM, JEDEC JESD22-C101 | ±1000 | V |

## Recommended Operating Conditions

| Parameter | Min | Max | Unit |
|---|---:|---:|---|
| \(V_{CCA}\) | 0.65 | 3.6 | V |
| \(V_{CCB}\) | 0.65 | 3.6 | V |
| \(V_I\) input voltage | 0 | 3.6 | V |
| \(V_O\) output voltage, active | 0 | \(V_{CC}\) | V |
| \(V_O\) output voltage, tri-state | 0 | 3.6 | V |
| Input transition rise/fall time | — | 10 | ns/V |
| \(T_A\) operating free-air temperature | -40 | 125 | °C |

## Electrical Characteristics (Representative)

| Parameter | Test Conditions | Min/Typ/Max | Unit |
|---|---|---|---:|
| \(V_{OH}\) | \(I_{OH}=-100\ \mu A\), 0.7–3.6 V | \(V_{CC}-0.1\) | V |
| \(V_{OH}\) | \(I_{OH}=-12\ mA\), 3 V | 2.3 | V |
| \(V_{OL}\) | \(I_{OL}=100\ \mu A\) | 0.1 | V |
| \(V_{OL}\) | \(I_{OL}=12\ mA\), 3 V | 0.7 | V |
| \(I_I\) control inputs | \(V_I=V_{CCA}\) or GND | ±0.5 µA | µA |
| \(I_{off}\) | A/B port, 0–3.6 V | ±4 µA | µA |
| \(I_{OZ}\) | Tri-state output current | ±4 µA | µA |
| \(I_{CCA}+I_{CCB}\) | \(I_O=0\) | 21 µA typ, 40 µA max | µA |
| \(C_I\) | Control input capacitance | 4.5 pF | pF |
| \(C_{IO}\) | Data I/O capacitance | 6.5 pF | pF |

## Switching Characteristics
- Propagation delay, enable/disable times specified across \(V_{CCA}\) = 0.7 V, 0.8 V, 0.9 V, 1.2 V, 1.5 V, 1.8 V, 2.5 V, 3.3 V.
- Maximum data rate: up to 310 Mbps for 1.8 V → 3.3 V translation.

## Device Functional Modes

| OE | DIR | A Port | B Port | Operation |
|---|---|---|---|---|
| L | L | Output enabled | Input Hi-Z | B data to A bus |
| L | H | Input Hi-Z | Output enabled | A data to B bus |
| H | X | Input Hi-Z | Input Hi-Z | Isolation |

## Application Information
Used for interfacing devices/systems operating at different interface voltages. Ideal for push-pull drivers. Example: SPI translation from SoC to GPS module.

## Power Supply Recommendations
- Apply GND first.
- Glitch-free power sequencing; no ramp-order or ramp-rate requirement.
- Use bypass capacitors, typically 0.1 µF, near supply pins; 1 µF + 0.1 µF in parallel can improve transient performance.

## Layout Guidelines
- Place bypass capacitors close to supply pins.
- Consider routing/load conditions to prevent ringing due to high drive strength.

## Package Options
- PW: TSSOP-16
- BQB: WQFN-16
- RSV: UQFN-16