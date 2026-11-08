\# RP2350-Zero



\[Image: RP2350-Zero mini development board featuring a USB Type-C port, two push buttons labeled BOOT and RUN, and castellated pin holes along the edges.]\[cite: 284]



\*\*Overview\*\*

The RP2350-Zero is a mini development board equipped with a Type-C female port.\[cite: 284] It utilizes the RP2350A developed by Raspberry Pi as its core, leading out all unoccupied pins in a very small form factor.\[cite: 284] Using castellated processing technology, it can be welded directly onto custom carrier boards.\[cite: 284]



\*\*Features\*\*

\* Adopts the RP2350A microcontroller chip designed by Raspberry Pi in the United Kingdom.\[cite: 284]

\* Features a unique dual-core and dual-architecture design, equipped with a dual-core ARM Cortex-M33 processor and dual-core Hazard3 RISC-V processor, with a flexible clock running up to 150 MHz.\[cite: 284]

\* Built-in 520KB of SRAM and 4MB of on-chip Flash.\[cite: 284]

\* Type-C interface, meaning there is no need to worry about front and back plugging.\[cite: 284]

\* Castellated module allows soldering directly to carrier boards.\[cite: 284]

\* USB 1.1 host and device support.\[cite: 284]

\* Supports low-power sleep and hibernation modes.\[cite: 284]

\* Drag-and-drop downloads can be made by USB recognition as mass storage.\[cite: 284]

\* 29 GPIO pins of the RP2350A (20 can be led out through pin headers, the rest can be led out only by soldering).\[cite: 284]

\* Multiple hardware peripherals include 2x SPI, 2x I2C, 2x UART, 4x 12-bit ADC, and 16x Controllable PWM channels.\[cite: 284]

\* Accurate clock and timer on-chip, along with a temperature sensor.\[cite: 284]

\* Accelerated floating-point libraries on-chip.\[cite: 284]

\* 12 x Programmable I/O (PIO) state machines for custom peripheral support.\[cite: 284]



\## Pinout Definition \& Dimensions

\[Image: RP2350-Zero pinout diagram detailing the functions of each pin, including Power, Ground, UART, GPIO, PIO, PWM, ADC, SPI, Debugging, and I2C, with a callout indicating that the WS2812 RGB LED uses the GP16 pin.]\[cite: 285]

\[Image: Dimensional drawing of the RP2350-Zero showing physical measurements of 18.00mm width by 23.50mm length, with 2.54mm pin spacing.]\[cite: 286]



\## Pico Getting Started

\*\*Firmware Download\*\*

\* MicroPython Firmware Download\[cite: 286]

\* C\_Blink Firmware Download\[cite: 286]



\*\*MicroPython Series\*\*

\* \*\*Install Thonny IDE\*\*: Download the Thonny IDE to facilitate the development of Pico/Pico2 boards with MicroPython.\[cite: 286] Installation packages are Windows versions; refer to Thonny's official website for other versions.\[cite: 286]

\* After installation, configure the language and motherboard environment. For Pico/Pico2, select the Raspberry Pi option for the motherboard environment.\[cite: 286]



\[Image: Thonny IDE blank interface window showing the text editor and Shell.]\[cite: 287]



\* \*\*Configure MicroPython environment and choose Pico/Pico2 port\*\*: Connect the Pico/Pico2 to your computer first.\[cite: 287] In the lower right corner of Thonny, left-click the configuration environment option and select "Configure interpreter".\[cite: 287]



\[Image: Thonny IDE interface with a red arrow pointing to the interpreter selection area in the bottom right corner.]\[cite: 288]



\* In the pop-up window, select MicroPython (Raspberry Pi Pico), and choose the corresponding port.\[cite: 287]



\[Image: Thonny options dialog box under the 'Interpreter' tab, showing 'MicroPython (Raspberry Pi Pico)' selected along with a COM port.]\[cite: 289]



\* \*\*Flash Firmware\*\*: Click OK to return to the Thonny main interface, download the corresponding firmware library and flash it to the device, then click the Stop button to display the current environment in the Shell window.\[cite: 289]

\* You can directly use the firmware provided by MicroPython official for the Pico series board; for RP series boards, use the firmware provided in the program package.\[cite: 289]

\* How to download firmware for Pico/Pico2 in Windows: Hold down the BOOT button while connecting to the computer, then release the BOOT button. A removable disk will appear; copy the firmware library into it.\[cite: 290]

\* How to download firmware for RP2040/RP2350 in Windows: Connect to the computer, press BOOT and RESET keys at the same time, release RESET first, then release BOOT. A removable disk will appear; copy the firmware library into it.\[cite: 290]



\[Image: Thonny IDE Shell displaying the MicroPython version v1.24.0 and Raspberry Pi Pico2 with RP2350 connection status.]\[cite: 290]



\## C/C++ Series

For C/C++, it is recommended to use Pico VSCode for development.\[cite: 291] This Microsoft Visual Studio Code extension assists in creating, developing, and debugging projects for the Raspberry Pi Pico series.\[cite: 291]



\*\*Install VSCode\*\*

1\. Download the pico-vscode package, unzip it, and double-click to install VSCode.\[cite: 291]



\[Image: Windows File Explorer showing the 'pico-vscode' folder containing the VSCode installer file.]\[cite: 291]

\[Image: Visual Studio Code welcome screen and 'About' dialog showing version 1.93.1.]\[cite: 292]



\*\*Install Extension\*\*

1\. Click Extensions and select Install from VSIX.\[cite: 292]

2\. Select the package with the `.vsix` suffix (raspberry-pi-pico-0.15.2.vsix) and click Install.\[cite: 293]

3\. VSCode will automatically install the raspberry-pi-pico extension and its dependencies; you can click Refresh to check the installation progress.\[cite: 293]

4\. Text in the lower right corner shows that the installation is complete. Close VSCode.\[cite: 294]



\[Image: VSCode Extensions tab showing the 'Install from VSIX...' option.]\[cite: 293]

\[Image: File selection dialog to install the 'raspberry-pi-pico-0.15.2.vsix' extension.]\[cite: 293]

\[Image: VSCode Extensions panel displaying the installation progress of the Pico extension and its dependencies, such as Cortex-Debug and CMake.]\[cite: 294]

\[Image: VSCode showing a notification in the bottom right indicating the installation is complete.]\[cite: 294]



\*\*Configure Extension\*\*

1\. Open the directory `C:\\Users\\username` and copy the entire `.pico-sdk` folder to that directory.\[cite: 294]

2\. Open VSCode and configure the paths for the Raspberry Pi Pico extensions under Settings.\[cite: 295]

&#x20;  \* Cmake Path: `${HOME}/.pico-sdk/cmake/v3.28.6/bin/cmake.exe`\[cite: 296]

&#x20;  \* Git Path: `${HOME}/.pico-sdk/git/cmd/git.exe`\[cite: 296]

&#x20;  \* Ninja Path: `${HOME}/.pico-sdk/ninja/v1.12.1/ninja.exe`\[cite: 296]

&#x20;  \* Python3 Path: `${HOME}/.pico-sdk/python/3.12.1/python.exe`\[cite: 296]



\[Image: Windows File Explorer showing the '.pico-sdk' folder being copied to the user's directory.]\[cite: 295]

\[Image: Windows File Explorer displaying the contents of the '.pico-sdk' directory.]\[cite: 295]

\[Image: VSCode settings window for configuring paths for Cmake, Git, Ninja, and Python3.]\[cite: 296]



\*\*New Project \& Compilation\*\*

1\. Create a new project, enter the project name, select the path, and click Create.\[cite: 296] 

2\. The project is created successfully.\[cite: 297]

3\. Select the SDK version.\[cite: 297]

4\. Select "Yes" for advanced configuration.\[cite: 297]

5\. Choose the toolchain: `13.2.Rel1` is for ARM cores, `RISCV.13.3` is for RISCV cores.\[cite: 297]

6\. Select "Default" for CMake version and Ninja version.\[cite: 298]

7\. Select the development board model.\[cite: 298]

8\. Click "Compile" to compile.\[cite: 298]

9\. The `.uf2` format file is successfully compiled.\[cite: 299]



\[Image: VSCode 'New Project' setup interface showing fields for project name, location, and feature toggles.]\[cite: 296]

\[Image: VSCode editor showing a generated C++ 'Hello World' project file.]\[cite: 297]

\[Image: VSCode prompt asking 'Configure advanced options?' with 'Yes' and 'No' buttons.]\[cite: 297]

\[Image: VSCode command palette showing options to select the ARM/RISCV Embedded Toolchain version.]\[cite: 298]

\[Image: VSCode command palettes to select the CMake version and Ninja version.]\[cite: 298]

\[Image: VSCode command palette showing the selection of the development board.]\[cite: 298]

\[Image: VSCode interface highlighting the 'Compile' button at the bottom status bar.]\[cite: 299]

\[Image: VSCode terminal output showing a successful build and the generation of a .uf2 file.]\[cite: 299]



\*\*Flash Firmware via VSCode\*\*

\* Method 1: Connect the development board to the computer, and click "Run" to flash the firmware directly using the pico-vscode plugin.\[cite: 299]

\* Method 2 (Manual): Press and hold Boot, connect to the computer, wait for the USB device to be recognized, and copy the `.uf2` file to the USB drive.\[cite: 300]



\[Image: VSCode interface highlighting the 'Run' button at the bottom status bar for flashing firmware.]\[cite: 300]



\*\*Import Project \& Update Extension\*\*

1\. Select the project directory and import the project.\[cite: 300]

2\. The Cmake file of the imported project cannot have Chinese characters (including comments).\[cite: 300]

3\. Add `set(PICO\_BOARD pico CACHE STRING "Board type")` to the Cmake file to switch between pico and pico2 normally.\[cite: 300]

4\. The extension version can be updated to the latest version after the installation is complete.\[cite: 301]



\[Image: VSCode 'Import Project' wizard interface.]\[cite: 300]

\[Image: VSCode editor showing the CMakeLists.txt file with the added set(PICO\_BOARD...) line.]\[cite: 301]

\[Image: VSCode extension page showing the 'Update' button for the Raspberry Pi Pico extension.]\[cite: 301]



\## Arduino IDE Series

\*\*Install Arduino IDE\*\*

1\. Go to the Arduino official website to download the Arduino IDE 2.0.0 installation package.\[cite: 301]

2\. Select "Just Download".\[cite: 302]

3\. Complete the download and click Install. During installation, if prompted to install a driver, click Install.\[cite: 303]

4\. The IDE opens in English; you can change the language in File -> Preferences.\[cite: 303]



\[Image: Arduino website showing 'Downloads' section and 'Arduino IDE 2.0.0' download options.]\[cite: 302]

\[Image: Arduino donation page highlighting the 'JUST DOWNLOAD' button.]\[cite: 303]

\[Image: Arduino IDE installation window showing the License Agreement.]\[cite: 303]

\[Image: Arduino IDE 'File' menu showing the 'Preferences' option and the language dropdown.]\[cite: 304]



\*\*Install Arduino-Pico Core\*\*

1\. Open the Arduino IDE, go to File -> Preferences.\[cite: 304]

2\. Add the following link to the Additional boards manager URLs: `https://github.com/earlephilhower/arduino-pico/releases/download/4.5.2/package\_rp2040\_index.json`\[cite: 305]

3\. Click Tools -> Board -> Boards Manager, search for `pico`, and install the "Raspberry Pi Pico/RP2040 by Earle F. Philhower, III" package.\[cite: 305]



\[Image: Arduino IDE Preferences window showing the RP2040 JSON link entered in the 'Additional boards manager URLs' field.]\[cite: 305]

\[Image: Arduino IDE Boards Manager searching for 'pico', showing the 'Raspberry Pi Pico/RP2040 by Earle F. Philhower, III' package.]\[cite: 306]



\*\*Upload Demo at the First Time\*\*

1\. Press and hold the BOOTSET button, connect the Pico to the USB port, and release the button after the computer recognizes the RPI-RP2 removable disk.\[cite: 306]

2\. Go to Tools -> Port. For the first connection, select "UF2 Board".\[cite: 308]

3\. Go to Tools -> Board -> Raspberry Pi Pico/RP2040 and select the corresponding model (e.g., Raspberry Pi Pico or Raspberry Pi Pico 2).\[cite: 309]

4\. Click the right arrow to upload the program.\[cite: 310]



\[Image: Graphic instructing to hold the BOOTSET button while connecting the Pico via USB cable.]\[cite: 307]

\[Image: Arduino IDE 'Tools' menu showing the 'Port' selection with various COM ports.]\[cite: 308]

\[Image: Arduino IDE 'Tools' menu showing the 'Port' selection with 'UF2 Board' highlighted.]\[cite: 309]

\[Image: Arduino IDE 'Tools' menu showing the 'Board' selection pointing to 'Raspberry Pi Pico/RP2040'.]\[cite: 309]

\[Image: Arduino IDE interface showing the 'Upload' button (right arrow icon) being clicked.]\[cite: 310]



If you need to reinstall or update the Arduino IDE, uninstall it completely and manually delete the `C:\\Users\\\[name]\\AppData\\Local\\Arduino15` folder before proceeding with a fresh installation.\[cite: 310]



\## FAQ

\*\*Question: Why do I put a file on a disk and the disk disappears and I don't see the previous file when I re-enter the disk?\*\*

\*\*Answer\*\*:

1\. The Bootrom of the RP2350 provides a standard USB bootloader that can be recognized as a writable drive for using UF2 files to copy code. The UF2 file is downloaded and written to Flash or RAM, and the device automatically restarts.\[cite: 311]

2\. Any file can be written to the USB drive, but they are not stored; it appears so due to host caching.\[cite: 312] When a valid UF2 file is written, special content is recognized and data is flashed to the designated location.\[cite: 312]

3\. UF2 files will not be stored; the firmware is flashed according to the file format.\[cite: 312]



\*\*Question: How to flash the firmware?\*\*

\*\*Answer\*\*:

Press RESET first, then press BOOT. Release RESET first, then release BOOT to enter flashing mode.\[cite: 312] Just drag and drop or copy the firmware into it.\[cite: 312]

