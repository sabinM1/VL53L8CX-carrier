# AN5897 — VL53L8 thermal guidelines

**1. Thermal Design Basics**
The VL53L8 consumes ~215 mW in continuous mode and requires proper PCB thermal management to keep the junction temperature below 110°C. 
*[Image: Figure 1. VL53L8 ranging sensor module - 3D render of the black optical module with red and blue tinted windows.]*

**2. Layout and Thermal Guidelines**
Target thermal resistance (θpcb) is 35°C/W.
- Maximize copper coverage.
- Solder the central B4 pad with an exposed bottom mask.
- Stitch 8 vias on the thermal pad.
*[Image: Figure 2. VL53L8 pinout and thermal pad - Underside of the module showing outer signal pads A1-A7, C1-C7, B1, B7, and the large central B4 ground/thermal pad.]*
*[Image: Figure 3. Recommendation for the thermal pad and via on the PCB - PCB layout diagram showing the B4 pad broken into eight solder paste rectangles with central thermal vias.]*