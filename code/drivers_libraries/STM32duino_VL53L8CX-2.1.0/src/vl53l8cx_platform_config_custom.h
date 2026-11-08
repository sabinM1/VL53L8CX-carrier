/**
 ******************************************************************************
 * @file    vl53l8cx_platform_config_custom.h
 * @brief   Local platform config for the VL53L8CX carrier (Rev 2).
 *
 * This file is picked up automatically by vl53l8cx_platform_config.h via
 * __has_include. It includes the default config first, then overrides only the
 * chunk size, so every macro the driver expects stays defined.
 ******************************************************************************
 */

#ifndef _VL53L8CX_PLATFORM_CONFIG_CUSTOM_H_
#define _VL53L8CX_PLATFORM_CONFIG_CUSTOM_H_

#include "vl53l8cx_platform_config_default.h"

/*
 * Frame trimming. A full 8x8 / 1-target frame is ~1444 bytes; the sketches only
 * use nb_target_detected, distance_mm and target_status. Dropping the other
 * outputs reduces data_read_size to ~320 bytes, i.e. ~1 ms per SPI read at
 * 3 MHz instead of ~4 ms, and ~7 ms per I2C read at 1 MHz instead of ~30 ms.
 *
 * These must be visible to every translation unit that includes
 * vl53l8cx_api.h: they change both the block-enable bitmask sent to the sensor
 * and the layout of VL53L8CX_ResultsData.
 */
#ifndef VL53L8CX_DISABLE_AMBIENT_PER_SPAD
  #define VL53L8CX_DISABLE_AMBIENT_PER_SPAD
#endif
#ifndef VL53L8CX_DISABLE_NB_SPADS_ENABLED
  #define VL53L8CX_DISABLE_NB_SPADS_ENABLED
#endif
#ifndef VL53L8CX_DISABLE_SIGNAL_PER_SPAD
  #define VL53L8CX_DISABLE_SIGNAL_PER_SPAD
#endif
#ifndef VL53L8CX_DISABLE_RANGE_SIGMA_MM
  #define VL53L8CX_DISABLE_RANGE_SIGMA_MM
#endif
#ifndef VL53L8CX_DISABLE_REFLECTANCE_PERCENT
  #define VL53L8CX_DISABLE_REFLECTANCE_PERCENT
#endif
#ifndef VL53L8CX_DISABLE_MOTION_INDICATOR
  #define VL53L8CX_DISABLE_MOTION_INDICATOR
#endif

/*
 * Reduce the SPI chunk buffer from 4096 to 1024 bytes. IO_Write puts
 * data_write[VL53L8CX_COMMS_CHUNK_SIZE + 2] on the stack, so this cuts the
 * per-call stack frame from ~4 KB to ~1 KB. The firmware download is ~82
 * chunks at 1 KB instead of ~21 at 4 KB; each extra chunk is ~2.7 ms of SPI
 * time at 3 MHz, which is negligible here.
 */
#ifndef VL53L8CX_COMMS_CHUNK_SIZE
  #define VL53L8CX_COMMS_CHUNK_SIZE 1024
#elif VL53L8CX_COMMS_CHUNK_SIZE > 1024
  #undef VL53L8CX_COMMS_CHUNK_SIZE
  #define VL53L8CX_COMMS_CHUNK_SIZE 1024
#endif

/*
 * Teensy 4.x (i.MX RT1062): WireIMXRT.h sets BUFFER_LENGTH 136 and the TX
 * buffer is BUFFER_LENGTH + 1 = 137 bytes. vl53l8cx.h IO_Write sends 2 address
 * bytes + DEFAULT_I2C_BUFFER_LEN in one transmission, so the default 136
 * overflows the driver. Cap it at 128 (2 + 128 = 130 <= 137).
 *
 * This must live in the library, not in the sketch: IO_Read/IO_Write are inline
 * members compiled in every translation unit that includes vl53l8cx.h (including
 * the library's own vl53l8cx.cpp). A #define in a sketch cannot change the copy
 * the library emits.
 */
#if defined(__IMXRT1062__)
  #ifndef DEFAULT_I2C_BUFFER_LEN
    #define DEFAULT_I2C_BUFFER_LEN 128
  #elif DEFAULT_I2C_BUFFER_LEN > 128
    #undef DEFAULT_I2C_BUFFER_LEN
    #define DEFAULT_I2C_BUFFER_LEN 128
  #endif
#endif

#endif  // _VL53L8CX_PLATFORM_CONFIG_CUSTOM_H_