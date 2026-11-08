# TN1531_Smart_Presence_Detection_FAQ.md

**Technical Specifications**
- **Supported Sensors:** VL53L7CX and VL53L8CX.
- **Resolution:** 8x8 in tracking mode, 4x4 in autonomous (sleep) mode.
- **Footprint:** 117029 bits in flash, 29951 in RAM.
- **AI required?** No, it does not require retraining models.

**Detection Capabilities**
- Motion is detected via firmware.
- Output includes X, Y, Z, and speed per user.
- **Still targets:** Trigger a "No Motion Timeout" causing the system to revert to sleep.
- **Pets/Objects:** Moving objects/pets will wake the system, and the algorithm evaluates motion profiles.

**Development, Customization, & Power**
- Distributed as a compiled library for Cortex-M architectures (M0+, M3, M33, M4, M7). Arduino is unsupported for the SPD library; Linux support is upcoming.
- Power consumption is 0.9 mW in autonomous mode and 28.1 mW in tracking mode.