# um3108-how-to-use-stmicroelectronics-xcubetof1-timeofflight-sensor-software-packages-for-stm32cubemx-stmicroelectronics.pdf

**1. Introduction**
The X-CUBE-TOF1 expansion software package for STM32Cube includes drivers that recognize Time-of-Flight (ToF) sensors and perform ranging. Supported sensors include VL53L4CX, VL53L4CD, VL53L7CX, and VL53L8CX. Supported boards include X-NUCLEO expansion boards and SATEL breakout boards.

**2. X-CUBE-TOF1 Software Expansion**
Provides a complete software framework, STM32CubeMX compatibility, and easy portability.
*[Image: Figure 1. X-CUBE-TOF1 software architecture - A block diagram showing Application (Ranging measurement example), Hardware Abstraction (STM32Cube HAL), and Hardware (STM32 Nucleo expansion and development boards).]*
*[Image: Figure 2. X-CUBE-TOF1 package folder structure - File tree showing Documentation, Drivers, Projects, and STM32CubeMX folders.]*

**3. System Setup Guidelines**
Hardware components require an STM32 Nucleo board, an X-NUCLEO or SATEL board, and a USB cable.
*[Image: Figure 3. STM32 Nucleo board - Photograph of a white STM32 Nucleo development board.]*
*[Image: Figure 4. Example of an X-NUCLEO-53L7A1 expansion board with a cover glass - Blue expansion board with multiple cover glass spacer options.]*
*[Image: Figure 5. Example of a SATEL-VL53L7CX breakout board - Small blue PCB with a ToF sensor, mounting holes, and pinout.]*
*[Image: Figure 6. Sensor expansion board plugged to STM32 Nucleo board - The blue X-NUCLEO board physically mounted on top of the white Nucleo board.]*
*[Image: Figure 7. Connection between an STM32 Nucleo board and a SATEL - Wiring diagram showing jumper wires connecting the SATEL breakout board pins to the Nucleo headers.]*

**4. Sample Application Examples**
Examples include SimpleRanging and ThresholdDetection, accessible via precompiled binaries or STM32CubeMX generation.
*[Image: Figure 8. SimpleRanging folder architecture - File tree showing Binary, EWARM, MDK-ARM, and STM32CubeIDE folders.]*
*[Image: Figure 9. NUCLEO-F401RE folder architecture - File explorer view highlighting the Binary folder for the 53L7A1.]*
*[Image: Figure 10. NODE drive - Windows explorer showing NODE_F401RE mass storage drive.]*
*[Image: Figure 11. Tera Term serial port setup - Dialog box configuring COM port to 460800 baud.]*
*[Image: Figure 12. Tera Term terminal setup - Dialog box showing terminal size and new-line settings.]*
*[Image: Figure 13. VL53L7CX ranging results - Command-line interface showing an 8x4 matrix of distance and signal measurements.]*
*[Image: Figures 14-38. STM32CubeIDE Configuration - A series of software screenshots detailing project creation, board selection, middleware software pack selection for X-CUBE-TOF1, GPIO pin configurations (PB3, PA4, PB0, PB4), I2C1 setup, NVIC interrupt activation, and STLink debugger upgrading.]*
*[Image: Figure 39. Results - Terminal output displaying standard SimpleRanging matrix data.]*
*[Image: Figure 40-41. Threshold Detection Configuration - STM32CubeMX screenshots showing ThresholdDetection application selection and platform pin settings.]*
*[Image: Figure 42. Result of the Threshold Detection example - Terminal output showing conditional distance matrix updates.]*
*[Image: Figure 43. Interrupt - C code snippet setting LowThreshold to 200 and HighThreshold to 600.]*
*[Image: Figures 44-47. SATEL Breakout Configuration - File trees and STM32CubeMX menus for the CUSTOM SimpleRanging application profile.]*