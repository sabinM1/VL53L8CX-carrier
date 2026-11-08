# DS_vl53l8cx-1.pdf
[Image: Two ST VL53L8CX sensors. One is face-up showing the black casing, emitter, and receiver apertures with the ST logo and "VL53L8CX" printed on it. The other is face-down showing the 16 gold LGA contact pads on the green substrate.][cite: 169]

## Product Status and Features
**Product status link**: VL53L8CX[cite: 169]
* New generation, multizone Time-of-Flight (ToF) sensor with low-power and enhanced distance ranging performance[cite: 169]
* Multizone distance measurement capability with either 4x4 or 8x8 separate zones[cite: 169]
* Autonomous low-power mode with interrupt programmable threshold to wake up the host[cite: 169]
* Ranging up to 400 cm, with enhanced performance under ambient light[cite: 169]
* Multitarget detection and distance measurement in each zone[cite: 169]
* Histogram processing and algorithmic compensation to minimize or remove the impact of cover glass crosstalk[cite: 169]
* Motion indicator for each zone to show if targets have moved and how they have moved[cite: 169]
* Frame rate capability of 60 Hz[cite: 169]
* Fully integrated miniature module with wide field of view (FoV)[cite: 169]
* New generation, high-power emitter: 940 nm invisible light VCSEL (vertical-cavity surface-emitting laser) and integrated analog driver[cite: 169]
* 65° diagonal square FoV using DOEs (diffractive optical elements) on both transmitter and receiver[cite: 169]
* Receiving array of SPADs (single photon avalanche diodes)[cite: 169]
* Low-power microcontroller running firmware[cite: 169]
* Size: 6.4 x 3.0 x 1.75 mm[cite: 169]
* Single reflowable component requiring 1.8 V core supply and 3.3 V AVDD supply[cite: 169]
* Optional 1.2 V or 1.8 V IOVDD interface voltage levels[cite: 169]
* I2C (up to 1 MHz) or SPI (up to 3 MHz) interface[cite: 169]
* Compatible with a wide range of cover glass materials and can be hidden behind dark cover glass[cite: 169]

## Application and Description
**Applications**: Robotic applications in difficult environments (SLAM, wall tracking, small object detection, cliff prediction, floor type recognition), System activation under ambient light for smart buildings/lighting, Content management (tanks, trucks, waste bins), Liquid level monitoring, Gesture recognition, Keystone correction for video projectors, AR/VR enhancement, IoT and battery-powered devices, and Laser assisted autofocus (LAF)[cite: 170]. 

**Description**: The VL53L8CX is an 8x8 multizone ToF ranging sensor enhancing performance under ambient light with reduced power consumption. Based on FlightSense technology, it ranges up to 400 cm with a 65° diagonal FoV. It integrates a powerful new generation VCSEL and advanced metasurface lenses[cite: 170]. It detects and tracks multiple targets within a 64-zone depth measurement and uses ST's histograms for cover glass crosstalk immunity above 60 cm. The VCSEL emits fully invisible 940 nm IR light (Class 1 eye-safe)[cite: 170].

## Product Overview
[Image: System FoV and exclusion zone description. A 3D diagram showing a sensor emitting a cone of light. The yellow inner cone represents the System FoV (45°x45°, 65° diagonal). The blue outer cone represents the Collector exclusion zone (57.6°x57.6°, 85.9° diagonal).][cite: 172]
[Image: VL53L8CX FoI. A heatmap square showing relative emitted power from 0% (blue) at the edges to 100% (dark red) in the center, demonstrating a relatively uniform square emission.][cite: 173]
[Image: VL53L8CX block diagram. Shows the ToF module containing ToF silicon with SPAD detection array, NVM, ROM, RAM, Microcontroller, Advanced Ranging Core, and VCSEL Driver connected to a 940nm VCSEL. Pins include SCL, SDA, INT, GND, AVDD, IOVDD.][cite: 174]
[Image: VL53L8CX pinout (bottom view). A diagram of the 16 bottom pads (A1-A7, B1, B4, B7, C1-C7) on the green substrate.][cite: 174]

* **Package**: Optical LGA16, Size: 6.4 x 3.0 x 1.75 mm[cite: 172]
* **Operating voltage**: AVDD: 3.3 V, CORE_1V8: 1.8 V, IOVDD: 1.2/1.8 V[cite: 172]
* **Operating temperature**: -30 to 85°C[cite: 172]
* **Sample rate**: Up to 60 Hz[cite: 172]
* **Interface**: I2C (1 MHz, address 0x52) or SPI (3 MHz)[cite: 172]

| Pin Number | Signal Name | Signal Type | Signal Description |
| :--- | :--- | :--- | :--- |
| A1 | GPIO1 | Digital I/O | General purpose I/O, INT output (needs 47 kΩ pullup)[cite: 175] |
| A2 | LPn | Digital input | I2C communication enable (0=disable, 1=enable)[cite: 175] |
| A3 | IOVDD | Power | 1.2 V or 1.8 V I/O supply[cite: 175] |
| A4 | SDA/MOSI | Digital I/O | I2C data / SPI MOSI[cite: 175] |
| A5 | SCL/MCLK | Digital input | I2C clock / SPI MCLK[cite: 175] |
| B1 | GPIO2 | Digital I/O | SYNC input[cite: 175] |
| B4 | THERMALPAD | Ground | Connect to ground plane[cite: 175] |
| B7 | CORE_1V8 | Power | 1.8 V analog core supply[cite: 175] |
| C1 | SPI_I2C_N | Digital input | I2C interface reset / SPI pullup[cite: 175] |
| C2 | NCS | Digital input | SPI Chip select (I2C: pulldown)[cite: 175] |
| C4 | AVDD | Power | 3.3 V analog and VCSEL supply[cite: 175] |
| C5 | MISO | Digital output | SPI MISO[cite: 175] |

[Image: Typical application schematic for I2C. Shows the VL53L8CX connected to a Host MCU via I2C with proper pullup resistors (2.2k on SDA/SCL, 47k on others) and decoupling capacitors (4.7uF on AVDD, 100nF on 1V8/IOVDD).][cite: 176]
[Image: Typical application schematic for SPI. Shows the VL53L8CX connected to a Host MCU via SPI with proper pullup resistors and decoupling capacitors.][cite: 177]

## Functional Description
[Image: Power state machine diagram. Shows transitions between POWER OFF, HP IDLE, LP IDLE, and RANGING states.][cite: 178]
[Image: Power up slew diagram. Graphs showing voltage thresholds and minimum slew rates for AVDD, CORE_1V8, and IOVDD.][cite: 179]
* The VL53L8CX requires three power supplies (CORE_1V8, AVDD, IOVDD)[cite: 179].
* The minimum reset time is 63µs after CORE_1V8 reaches the 0.9V threshold[cite: 179].
* **I2C Interface**: Maximum speed 1 Mbit/s, device address 0x52[cite: 180]. 
[Image: Data transfer protocol diagrams for I2C and SPI timing characteristics.][cite: 180, 181, 183, 185, 186, 187]

## Thermal and Electrical Characteristics
* **Storage Temperature**: -40 to 125°C[cite: 188]
* **Operating Temperature**: -30 to 85°C[cite: 188]
* **AVDD Supply**: 3.3 V (Typ)[cite: 189]
* **CORE_1V8 Supply**: 1.8 V (Typ)[cite: 189]
* **Current Consumption (Active Ranging)**: AVDD = 50 mA (Max), CORE_1V8 = 50 mA (Typ), IOVDD = 0.003 mA (Typ)[cite: 190]
* **Typical Power Consumption**: 215 mW in Continuous mode[cite: 190]

## Ranging Performances
[Image: Zone mapping in 4x4 mode. A 4x4 grid labeled 0 to 15 showing how zones increment. Corners are highlighted.][cite: 192]
[Image: Zone mapping in 8x8 mode. An 8x8 grid labeled 0 to 63 showing the zone layout. Corners are highlighted.][cite: 192]
[Image: Effective orientation. Shows an "F" target being imaged onto the SPAD array, demonstrating that the lens flips the image horizontally and vertically.][cite: 193]

* **Maximum Ranging Distance (4x4, Dark)**: Up to 4000 mm for White, Light Gray, and Gray targets[cite: 194].
* **Maximum Ranging Distance (8x8, Dark)**: Up to 4000 mm for White (Inner), 3300 mm for Light Gray, 2450 mm for Gray[cite: 194].
* **Range Accuracy**: ±10 mm to ±12 mm at short range (20-200mm); ±3% to ±5% at long range (200-4000mm)[cite: 195].

## Outline Drawings & Laser Safety
[Image: Outline drawing (1/4) showing top, bottom, and side dimensions of the module. Dimensions are 6.400 x 3.030 x 1.750 mm.][cite: 200]
[Image: Outline drawing (2/4) showing pad dimensions and pitch layout.][cite: 201]
[Image: Exclusion cones drawing (4/4) showing the Rx and Tx optical cones emitting from the module.][cite: 202]
* **Laser Safety**: Complies with IEC 60825-1:2014 Class 1 laser safety limits. The VCSEL is safe for the eyes under all reasonably foreseeable conditions[cite: 203]. 

## Packing and Reflow
[Image: 2D marking product code on module cap. Shows the QR-style 2D tracking code aligned with pin C7.][cite: 204]
[Image: Tape and reel packaging drawing showing dimensions of the carrier tape for the modules.][cite: 205]
[Image: Recommended solder pattern layout for the PCB.][cite: 206]
[Image: Solder profile graph showing the temperature ramp-up, peak, and cool-down for Pb-free solder reflow.][cite: 207]
* Moisture sensitivity is level 3 (MSL)[cite: 206]. 
* Recommended peak solder temperature: 240°C. Maximum peak: 260°C[cite: 207].