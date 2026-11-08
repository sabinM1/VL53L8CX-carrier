# april26th-2022-gesture-recognition-based-on-st-flightsense-tof-sensors.pdf
[Image: A woman in a kitchen slicing vegetables while using hand gestures to interact with a tablet. The ST logo and title "Gesture Recognition Based on ST FlightSense ToF sensors" are displayed.][cite: 214]

**Agenda**[cite: 215]
1. What is Gesture Recognition?[cite: 215]
2. Why use Gesture Recognition?[cite: 215]
3. What is available?[cite: 215]

## What is Gesture Recognition?
[Image: Three icons representing Touch (finger touching a button), Voice (microphone), and Motion (hand waving) as forms of Human-Machine Interaction.][cite: 218]
* **The principle**: Natural human interaction enabler where hand movements are captured by a sensor and translated into computer commands[cite: 219].
[Image: Diagram showing a hand waving in front of a Time-of-Flight sensor, translating the motion into computer commands.][cite: 220]

**Application Control Flow using ST Gesture_EVK:**
1. **Hand tracking**: The sensor captures spatial data of the hand[cite: 221].
2. **Gesture recognition**: The software processes the data to recognize specific actions (e.g., Tap, Double Tap, Swipes, Level Control)[cite: 222].
3. **Application Control**: The recognized gesture controls the interface (e.g., swiping to the next photo in a gallery)[cite: 223].

## Why use Gesture Recognition?
* **User-friendly**: Control your devices when you cannot touch them with your hands[cite: 225].
* **Hygiene**: Touchless interface to avoid spreading germs[cite: 225].
* **Safety**: Protection distance from hot, cold, and sharp items (e.g., industrial robotics)[cite: 225].
* **Entertainment**: Playful solution for human-like interaction[cite: 225].

**Unlimited Markets**[cite: 226]:
* *User-friendly*: Thermostats, Switches, Service robots, Smart speakers, Laptops, Tablets, Smartphones, AR/VR[cite: 226].
* *Hygiene*: Digital Signage, Sanitary, Elevators, Vending machines, ATMs[cite: 226].
* *Safety*: Kitchen appliances, Industrial robots, Production lines[cite: 226].
* *Entertainment*: Toy robots, Gaming[cite: 226].

[Images: Application examples showing interface control for presentations, adjusting a stovetop temperature without touching the hot surface, human-to-machine interactions in a factory, interacting with a smart thermostat, and using a contactless beverage dispenser.][cite: 227, 228, 229, 230, 231]

## What is available today?
**VL53L5CX at a Glance**[cite: 234]:
* First Multi-zone (64 zones) Time-of-Flight ranging sensor[cite: 234].
* Parallel multizone ranging output: 4x4 or 8x8 zones separate regions of interest[cite: 234].
* Wide FoV: 45°x45° (63° diagonal)[cite: 234].
* Up to 400 cm ranging with multi-target detection[cite: 234].
* Frame rate capability up to 60 Hz[cite: 234].
* Immunity to cover glass cross-talk beyond 60cm via Histogram processing[cite: 234].
[Image: A histogram graph showing Object Detected vs Xtalk Smudge, and an image of the VL53L5CX sensor.][cite: 234]

**Hardware and Software Bricks for Easy Integration**[cite: 235]:
* **Software**: GUI Gesture EVK and Firmware for STM32 MCU[cite: 235].
* **Hardware**: ToF sensor, X-NUCLEO board, STM32 board[cite: 235].
[Image: Screenshots of the Gesture Software STSW-IMG035_EVK interface showing heatmaps of hand tracking, gesture icons, and data logging grids.][cite: 236]
[Image: Example code snippets for the Gesture Firmware STSW-IMG035, displaying C code initializing peripherals for STM32 MCU.][cite: 237]

**Multi-Zone FlightSense ToF Benefits**[cite: 238]:
* **Cost effective**: Inexpensive sensor and system architecture, easy to integrate[cite: 238].
* **Low power consumption**: Low power ToF compared to other technologies; light gesture algorithms can run on low-power MCUs[cite: 238].
* **Low processing complexity**: Light data flow unlike radar and cameras[cite: 238].
* **Independent**: Not subject to target color, shape, or size (unlike traditional IR)[cite: 238].
* **Privacy**: No images taken, only multiple distance measurements (unlike cameras)[cite: 238].

**Hardware Roadmap**[cite: 240]:
* **VL6180V1**: Tap, Left/Right swipe (needs 2 sensors)[cite: 240].
* **VL53L5CX**: Hand tracking, Tap/Double-tap, Level control, Right/Left swipe[cite: 240].
* **VL53LxxxX**: Coming soon with Up/Down swipe and more complex gestures[cite: 240].