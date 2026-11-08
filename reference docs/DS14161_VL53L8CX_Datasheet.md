# DS14161 — VL53L8CX datasheet

**1. Product Overview**
The VL53L8CX is an 8x8 multizone ToF sensor with a 65° diagonal FoV, utilizing a 940 nm VCSEL.
*[Image: Cover image - Close up of VL53L8CX sensor from top and bottom.]*
*[Image: Figure 1. System FoV and exclusion zone - 3D geometric illustration defining the 45x45 degree system FoV within the larger 57.6x57.6 degree collector exclusion zone.]*
*[Image: Figure 2. VL53L8CX FoI - Colorful heatmap showing 43.4x43.4 degree core illumination.]*
*[Image: Figure 3. VL53L8CX block diagram - Silicon architecture showing SPAD array, ROM/RAM, Advanced Ranging Core, and VCSEL driver.]*
*[Image: Figure 4. VL53L8CX pinout - Bottom view of 16 LGA pads (A1-A7, B1, B4, B7, C1-C7).]*
*[Image: Figure 5. Typical application schematic for I2C - Wiring diagram showing 47k pullups on digital lines and SDA/SCL connections to host.]*
*[Image: Figure 6. Typical application schematic for SPI - Wiring diagram adapting the interface for SPI (NCS, MISO, MOSI, MCLK).]*

**2. Functional Description & Timing**
*[Image: Figure 7. System functional description - High level block diagram linking Host API, I2C/SPI, and Sensor Firmware.]*
*[Image: Figure 8. Power state machine - State transition diagram (Power Off -> HP Idle <-> LP Idle / Ranging).]*
*[Image: Figure 9. Power up slew - Voltage graph specifying slew rates for AVDD, CORE_1V8, and IOVDD reaching 1.8V/3.3V.]*
*[Images: Figures 10-20. Timing Diagrams - Comprehensive digital waveform diagrams documenting I2C start/stop conditions, read/write byte sequences, and SPI clock polarity/phase timing constraints.]*

**3. Ranging Performances**
*[Image: Figures 21-22. Zone mapping - Grids defining the numbered array layout for 4x4 (0-15) and 8x8 (0-63) reporting structures.]*
*[Image: Figure 23. Effective orientation - Diagram showing optical inversion mapping physical targets to the SPAD grid.]*
*[Images: Figures 24-25. Range accuracy vs distance - Graphs showing error margins (+/- mm at short range, +/- % at long range) across continuous and autonomous modes.]*

**4. Mechanical & Packaging**
*[Images: Figures 26-29. Outline drawings - Engineering CAD drawings specifying 6.4 x 3.0 x 1.75 mm dimensions, pad pitches, and optical exclusion cone constraints.]*
*[Image: Figure 30. Class 1 laser label - Standard safety warning label.]*
*[Image: Figure 31. 2D marking - Laser etched tracking code on the module cap.]*
*[Image: Figure 32. Tape and reel packaging - Dimensions and feed direction for automated assembly lines.]*
*[Image: Figure 33. Recommended solder pattern - PCB land pattern drawing.]*
*[Image: Figure 34. Solder profile - Reflow temperature curve targeting 240°C peak.]*