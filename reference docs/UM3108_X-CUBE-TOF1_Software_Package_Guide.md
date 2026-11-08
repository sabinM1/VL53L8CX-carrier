# UM3109_VL53L8CX_User_Manual.md

**1. Introduction & Functional Description**
The VL53L8CX is a low-power, high-performance ToF multizone sensor utilizing a metasurface lens for a 45x45 degree FoV.
*[Image: Figure 1. VL53L8CX sensor module - Photograph of the black rectangular optical module showing the Tx and Rx windows and underside LGA pads.]*
*[Image: Figure 2. VL53L8CX system overview - Diagram of Host (running Ultra Lite Driver) communicating via I2C/SPI to the Module (running Firmware).]*
*[Image: Figure 3. VL53L8CX effective orientation - Diagram showing a red target 'F' being inverted optically onto a 4x4 SPAD array where zone 0 is bottom-left.]*
*[Image: Figure 4. Multiple sensors on I2C bus - Schematic showing host connected to two ToF sensors sharing SDA/SCL but with individual LPn control pins.]*
*[Image: Figure 5. Multiple sensors on SPI - Schematic showing host SPI bus with separate NCS (chip select) lines for each ToF sensor.]*

**2. Package Content and Data Flow**
*[Image: Figure 6. Driver architecture - Folder structure showing mandatory API files and optional plugins (xtalk, motion, detection).]*
*[Image: Figure 7. Crosstalk calibration flow - Flowchart: Start Xtalk calibration -> Get Xtalk data -> Set Xtalk data.]*
*[Image: Figure 8. Ranging flow - Flowchart: Initialize -> Optional Xtalk/Functions -> Start ranging -> Wait for data -> Get measurements -> Stop ranging.]*

**3. Available Features**
Includes Ranging mode (Continuous/Autonomous), Sharpener, Target Order, Xtalk margin, and Motion detection.
*[Image: Figure 9. Integration time for 4x4 autonomous - Timing diagram showing INT pin pulses and single integration time per measurement.]*
*[Image: Figure 10. Integration time for 8x8 autonomous - Timing diagram showing four separate integration times per measurement block.]*
*[Image: Figure 11. Example of scene using several sharpener values - Multiple 4x4 matrices illustrating how sharpener percentages (0%, 20%, 40%, 99%) affect edge detection between a 100mm and 500mm target.]*
*[Image: Figure 12. Example of histogram with two targets - Bar chart showing signal peaks at 100mm and 700mm.]*
*[Image: Figure 13. Xtalk margin - Bar chart demonstrating crosstalk signal threshold due to coverglass up to 600mm.]*
*[Image: Figure 14. External synchronization flow - Flowchart showing synchronization via an external SYNC pin triggering individual ranging events.]*