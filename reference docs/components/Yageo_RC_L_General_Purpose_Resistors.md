# MIC5501/2/3/4 — 300 mA Single-Output LDO in Small Packages

Source file: `MIC5501-02-03-04-300mA-Single-Output-LDO-in-Small-Packages-DS20006006.pdf`

## Features
- Input voltage range: 2.5 V to 5.5 V
- Fixed output voltages: 1.0 V to 3.3 V
- 300 mA ensured output current
- High output accuracy: ±2%
- Low quiescent current: 38 µA
- Stable with 1 µF ceramic output capacitors
- Low dropout: 160 mV @ 300 mA
- Output discharge circuit: MIC5502, MIC5504
- Internal enable pull-down: MIC5503, MIC5504
- Thermal-shutdown and current-limit protection
- 4-lead 1.0 mm × 1.0 mm UDFN package
- MIC5501/4 available in 5-lead SOT-23

## Applications
- Smartphones
- DSC, GPS, PMP, PDAs
- Medical devices
- Portable electronics
- 5 V systems

## General Description
The MIC5501/2/3/4 are general-purpose 300 mA LDOs in ultra-small packages. MIC5502 and MIC5504 include auto-discharge when EN is low. MIC5503 and MIC5504 have an internal pull-down on EN, disabling output when EN floats. Ideal for battery-powered applications: 2% initial accuracy, low dropout, low ground current, zero-off-mode current when disabled. Operating junction temperature: -40°C to +125°C.

## Package Types
- MIC5501/2/3/4: 4-lead UDFN (MT), 1 mm × 1 mm
- MIC5501/4: 5-lead SOT-23-5 (M5)

## Typical Application Circuit
- \(V_{IN}\) to VIN
- 1 µF input capacitor
- 1 µF output capacitor
- EN control
- GND

## Absolute Maximum Ratings

| Parameter | Min | Max | Unit |
|---|---:|---:|---|
| Supply voltage \(V_{IN}\) | -0.3 | +6 | V |
| Enable voltage \(V_{EN}\) | -0.3 | \(V_{IN}\) | V |
| Power dissipation | Internally limited | — | — |
| ESD rating | 3 kV | — | V |

## Operating Ratings

| Parameter | Min | Max | Unit |
|---|---:|---:|---|
| Supply voltage \(V_{IN}\) | 2.5 | 5.5 | V |
| Enable voltage \(V_{EN}\) | 0 | \(V_{IN}\) | V |

## Electrical Characteristics
Conditions: \(V_{IN}=V_{EN}=V_{OUT}+1V\), \(C_{IN}=C_{OUT}=1\ \mu F\), \(I_{OUT}=100\ \mu A\), \(T_J=+25°C\); bold values indicate -40°C to +125°C.

| Parameter | Symbol | Min | Typ | Max | Unit | Conditions |
|---|---|---:|---:|---:|---|---|
| Output voltage accuracy | \(V_{OUT}\) | -2.0 | — | 2.0 | % | Variation from nominal |
| Output voltage accuracy over temp | \(V_{OUT}\) | -3.0 | — | 3.0 | % | -40°C to +125°C |
| Line regulation | — | — | 0.02 | 0.3 | %/V | \(V_{IN}=V_{OUT}+1V\) to 5.5 V |
| Load regulation | — | — | 8 | 40 | mV | \(I_{OUT}=100\ \mu A\) to 300 mA |
| Dropout voltage | \(V_{DO}\) | — | 80 | 190 | mV | \(I_{OUT}=150\ mA\) |
| Dropout voltage | \(V_{DO}\) | — | 160 | 380 | mV | \(I_{OUT}=300\ mA\) |
| Ground pin current | \(I_{GND}\) | — | 38 | 55 | µA | \(I_{OUT}=0\ mA\) |
| Ground pin current | \(I_{GND}\) | — | 42 | 65 | µA | \(I_{OUT}=300\ mA\) |
| Shutdown ground current | \(I_{GND(SHDN)}\) | — | 0.05 | 1 | µA | \(V_{EN}=0V\) |
| Ripple rejection | PSRR | — | 60 | — | dB | \(f=1\ kHz\), \(C_{OUT}=1\ \mu F\) |
| Current limit | \(I_{LIM}\) | 400 | 630 | 900 | mA | \(V_{OUT}=0V\) |
| Output voltage noise | \(e_n\) | — | 175 | — | µV RMS | 10 Hz–100 kHz |
| Auto-discharge NFET resistance | — | — | 25 | — | Ω | MIC5502/4, \(V_{EN}=0V\), \(V_{IN}=3.6V\), \(I_{OUT}=-3mA\) |
| Enable pull-down resistor | — | — | 4 | — | MΩ | MIC5503/5504 |
| Logic-low enable input voltage | \(V_{EN}\) | — | — | 0.2 | V | — |
| Logic-high enable input voltage | \(V_{EN}\) | 1.2 | — | — | V | — |

## Thermal Specifications
- Storage temperature: -65°C to +150°C
- Max junction temperature: +150°C
- Operating junction temperature: -40°C to +125°C
- Lead temperature: +260°C, soldering 10 s
- θJA: 250°C/W (1 mm × 1 mm UDFN), 253°C/W (SOT-23-5)

## Pin Function Table

| UDFN Pin | SOT23-5 Pin | Pin Name | Description |
|---|---|---|---|
| 1 | 5 | VOUT | Output voltage. MIC5502/5504 switch internal 25 Ω load to discharge external capacitor when disabled. |
| 2 | 2 | GND | Ground. |
| 3 | 3 | EN | Active-high enable. High = ON; Low = OFF. MIC5501/5502 should not float. MIC5503/5504 have internal pull-down and may float. |
| 4 | 1 | VIN | Supply input. |
| — | 4 | NC | No connection. |
| EP | — | ePAD | Exposed heatsink pad. Connect to GND for best thermal performance. |

## Application Information
- Input capacitor: 1 µF required from input to ground.
- Output capacitor: 1 µF or greater required for stability; low-ESR ceramic recommended.
- No-load stability: stable with no load.
- Enable/shutdown: active-high; forcing EN low disables regulator and enters virtually zero-current off state.
- Thermal: calculate power dissipation as  
  \[
  P_D=(V_{IN}-V_{OUT})\times I_{OUT}+V_{IN}\times I_{GND}
  \]
  Ground current contribution is usually <1% and can be ignored.

### Thermal Example
For \(V_{IN}=3.6V\), \(V_{OUT}=2.8V\), \(I_{OUT}=300mA\):
\[
P_D=(3.6-2.8)\times 0.3=0.240W
\]
Using \(\theta_{JA}=250°C/W\), \(T_{J(MAX)}=125°C\):
\[
T_A=65°C
\]

## Typical Application Schematic
- C1 = 1 µF/10 V input capacitor
- C2 = 1 µF/10 V output capacitor
- U1 = MIC550x-xYMT

## PCB Layout Recommendations
- Keep input and output capacitors close to the device.
- Connect exposed pad to GND for thermal performance.