# pcb-manufacturing-guidelines-vl53l8.pdf
[Image: Close up 3D render of the VL53L8 sensor package with the ST logo.][cite: 243]

## Purpose and Coverage
* This document describes requirements for SAFE ToF sensor handling, liner handling, PCB assembly, and de-soldering & re-soldering without damaging the sensor or PCB[cite: 247].
* Rework covers removing defective sensors, investigating poor soldering, and safely protecting the sensor while reworking nearby components[cite: 247].
* Covers: PCB layout rules, MSL3 handling, mechanical handling, soldering processes, and manual rework procedures[cite: 248].

## PCB Layout Rules for ToF Soldering
[Image: VL53L8 Pad type showing the 14 NSMD pads and the central thermal pad. Also shows the recommended solder pattern layout dimensions.][cite: 251]
[Image: Diagram indicating PCB land design parameters A, B, C, D, and the keep-out footprints.][cite: 252]
[Image: Photo showing poor solder paste application with uneven, rough solder blobs on the module pads.][cite: 253]

1. PCB design should be as symmetrical as possible[cite: 250].
2. DNC (Do Not Connect) pins must be left unconnected[cite: 250].
3. It is recommended to open solder mask external to the PCB land[cite: 250].
4. The area below the sensor must be a keep-out area. No vias or traces below the footprint[cite: 250].
5. For the thermal pad, use multiple pads with vias to GND instead of one large pad[cite: 252].
6. Solder paste thickness must be uniform (using SPI control). Stencil thickness of 90-150µm is recommended[cite: 253].

## MSL3 Handling, Packing, Shipping and Process
* ToF sensors are MSL Level 3 compliant. The vapor pressure of moisture inside a plastic package increases rapidly during reflow, which can cause internal delamination, die cracks, or package deformation leading to sensor offset drift[cite: 255].
* Handled as a non-hermetic package and dry mounted as per JEDEC-STD-033C[cite: 256].
* **Storage**: Use within 18 months from delivery. Store at 16° to 27°C and 40-60% RH[cite: 258].
* If the HIC (Humidity Indicator Card) is >10%, devices require baking before board mounting[cite: 259]. Bake at 90°C for >6 hours or 40°C for 13 days (sensor only, do not bake Tape & Reel at 90°C)[cite: 259].

## Handling at Customer Manufacturing Process
[Image: Flowchart of SMT manufacturing process pointing out steps where contamination can happen: SMT Process, PCB Washing, Coating, PCB Cutting.][cite: 264]

* **Reflow**: Vent hole clogging or dust from soldering. *Action*: Clean oven regularly, use a liner[cite: 265].
* **Manual Soldering**: Contamination or mechanical stress. *Action*: Use hot plate instead of hot air gun, blow dry clean air, handle carefully[cite: 265].
* **PCB Washing & Coating**: Not recommended. Use protective gasket/caps if required[cite: 265, 266].
[Image: Diagram showing a ToF sensor protected by a one-time-usable waterproof SMD/glue gasket and sealing resin during PCB coating/rinsing.][cite: 266]
* **Cleanliness**: Recommended to mount in a Class 1000 environment (if using the non-liner version)[cite: 262]. Dry cleaning requires neutral/clean gas blown vertically with a laminar flow[cite: 267].

## Manual Rework Procedures
[Image: Top down view of two heavily melted and deformed ToF sensors from excessive desoldering heat (350°C).][cite: 271, 283]
[Image: A technician using tweezers and a hot air gun with a 4-wall protective cube surrounding the sensor to safely desolder it.][cite: 275]
[Image: Stencil fixture tool used to precisely apply solder paste to the bottom pads of the ToF sensor before re-soldering.][cite: 281]

* **Safe De-soldering**: Maximum real temperature of 275°C on the sensor cap. Use a protective cube and a calibrated thermometer[cite: 272]. Set hot air to 250°C and adjust until the thermometer reads 260°C. Remove sensor after < 60 seconds[cite: 275].
* **Protecting ToF Sensor During Nearby Rework**: The sensor cap temperature must never exceed 260°C while nearby components are heated[cite: 272]. Use a 4-wall protective cube around the ToF sensor[cite: 277]. 
* **Safe Re-soldering**: Manual re-soldering maximum real temperature of 260°C (recommended to set station to 250°C to account for tool inaccuracies)[cite: 280]. Use a stencil fixture to align and paste the sensor pads[cite: 281].
* ST guarantees 3x reflow solderings (up to 260°C peak). A 4th reflow is not guaranteed; if required, the sensor must be replaced with a new one and recalibrated[cite: 273].