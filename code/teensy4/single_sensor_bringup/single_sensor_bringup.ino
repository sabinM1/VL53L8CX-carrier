/**
 * single_sensor_bringup.ino  —  Teensy 4.0
 *
 * First-light test for ONE VL53L8CX carrier board, on the header-pin SPI bus.
 *
 * Teensy 4.0 facts used here (verified in the installed core):
 *   - I/O is 3.3 V, not 5 V tolerant.
 *   - SPI (LPSPI4) pins are 13 (SCK), 11 (MOSI), 12 (MISO), 10 (CS). All on the
 *     header (pins 0..23 are breadboardable; 24..39 are bottom pads).
 *   - Pin 13 also drives the on-board LED: in SPI mode 3 SCK idles high, so the
 *     LED is lit and toggles during transfers. Harmless, but noted.
 *   - The sensor runs from the external 3.3 V rail, not the Teensy 3.3 V pin
 *     (which is limited to ~250 mA).
 *
 * Sensor SPI limit is 3 MHz, mode 3 (DS14161). The library default is 5 MHz.
 *
 * Serial: USB CDC @ 115200. 'g' dumps the 8x8 grid.
 */

#include <Arduino.h>
#include <SPI.h>
#include <vl53l8cx.h>

#ifndef VL53L8CX_CS_SETTLE_US
#error "Unpatched VL53L8CX library: CS-settle delay missing. See code/teensy4/README.md"
#endif
#if !defined(VL53L8CX_COMMS_CHUNK_SIZE) || VL53L8CX_COMMS_CHUNK_SIZE > 1024
#error "VL53L8CX chunk-size override missing: copy vl53l8cx_platform_config_custom.h"
#endif
// Output trimming (round 6): ambient / spads / signal / sigma / reflectance /
// motion blocks are disabled in the custom config, cutting a frame from ~1444
// to ~320 bytes.
#if !defined(VL53L8CX_DISABLE_AMBIENT_PER_SPAD) || !defined(VL53L8CX_DISABLE_NB_SPADS_ENABLED) \
 || !defined(VL53L8CX_DISABLE_SIGNAL_PER_SPAD) || !defined(VL53L8CX_DISABLE_RANGE_SIGMA_MM) \
 || !defined(VL53L8CX_DISABLE_REFLECTANCE_PERCENT) || !defined(VL53L8CX_DISABLE_MOTION_INDICATOR)
#error "Output trimming missing: the patched platform_config_custom.h is not being used"
#endif
static_assert(sizeof(VL53L8CX_ResultsData) <= 512,
              "ResultsData should be the trimmed layout (~258 bytes)");

// ---------------------------------------------------------------------------
// Pin map — Teensy 4.0 SPI (LPSPI4), header pins
// ---------------------------------------------------------------------------
static const uint8_t PIN_SCK  = 13;
static const uint8_t PIN_MOSI = 11;
static const uint8_t PIN_MISO = 12;
static const uint8_t PIN_CS   = 10;

static const uint32_t SENSOR_SPI_HZ = 3000000UL;
static const uint8_t  RANGING_HZ    = 15;
static const uint8_t  CENTER_ZONE   = 27;

static inline bool status_ok(uint8_t s) { return s == 5 || s == 9; }

// In-place construction (the constructor stores `this`; never copy-init).
static VL53L8CX sensor(&SPI, PIN_CS, -1, -1, SENSOR_SPI_HZ);

static VL53L8CX_ResultsData results;
static bool ranging_ok = false;
static uint32_t frame_count = 0;

static void park_cs() {
  pinMode(PIN_CS, OUTPUT);
  digitalWrite(PIN_CS, HIGH);
}

static void print_grid(const VL53L8CX_ResultsData *r) {
  Serial.println("# 8x8 distance [mm]");
  for (uint8_t row = 0; row < 8; row++) {
    Serial.print("# ");
    for (int8_t col = 7; col >= 0; col--) {
      uint8_t zone = row * 8 + (uint8_t)col;
      uint8_t st = r->target_status[zone * VL53L8CX_NB_TARGET_PER_ZONE];
      if (r->nb_target_detected[zone] > 0 && status_ok(st)) {
        Serial.printf("%5d ", (int)r->distance_mm[zone * VL53L8CX_NB_TARGET_PER_ZONE]);
      } else {
        Serial.print("    - ");
      }
    }
    Serial.println();
  }
}

static bool init_and_start() {
  uint8_t st = sensor.init();
  Serial.printf("# init: status=%u\n", st);
  if (st != VL53L8CX_STATUS_OK) {
    Serial.println("# INIT FAILED");
    return false;
  }

  st = sensor.set_resolution(VL53L8CX_RESOLUTION_8X8);
  if (st != VL53L8CX_STATUS_OK) { Serial.println("# set_resolution FAILED"); return false; }

  st = sensor.set_ranging_frequency_hz(RANGING_HZ);
  if (st != VL53L8CX_STATUS_OK) { Serial.println("# set_frequency FAILED"); return false; }

  st = sensor.set_ranging_mode(VL53L8CX_RANGING_MODE_CONTINUOUS);
  if (st != VL53L8CX_STATUS_OK) { Serial.println("# set_mode FAILED"); return false; }

  st = sensor.start_ranging();
  Serial.printf("# start_ranging: status=%u\n", st);
  return st == VL53L8CX_STATUS_OK;
}

void setup() {
  // Raise the external rail first: the load switch ON pin has a 100k pull-down,
  // so the rail is off at reset, and driving CS high while it is off feeds
  // current through R7 into a dead node.
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  delay(5);

  park_cs();

  Serial.begin(115200);
  uint32_t t0 = millis();
  while (!Serial && (millis() - t0) < 3000) {}

  Serial.println("# VL53L8CX single-sensor bring-up (Teensy 4.0)");
  Serial.printf("# SPI: SCK=%u MOSI=%u MISO=%u CS=%u  %lu Hz\n",
                PIN_SCK, PIN_MOSI, PIN_MISO, PIN_CS, (unsigned long)SENSOR_SPI_HZ);

  // Teensy 4.0 uses fixed SPI pins; no setSCK/setTX/setRX calls.
  SPI.begin();

  sensor.begin();

  uint8_t alive = 0;
  uint8_t st = VL53L8CX_STATUS_OK;
  bool alive_ok = false;
  // Same 3 x 50 ms probe as the array sketches: the rail needs only ~0.5 ms,
  // but a sensor booting out of reset must not be declared dead on one try.
  for (uint8_t a = 0; a < 3 && !alive_ok; a++) {
    st = sensor.is_alive(&alive);
    if (st == VL53L8CX_STATUS_OK && alive) alive_ok = true;
    else delay(50);
  }
  Serial.printf("# is_alive: status=%u alive=%u\n", st, alive);
  if (!alive_ok) {
    Serial.println("# PROBE FAILED: check wiring, 3V3 rail, SPI mode, CS pin.");
    return;
  }

  ranging_ok = init_and_start();
}

void loop() {
  if (!ranging_ok) {
    delay(1000);
    return;
  }

  uint8_t ready = 0;
  if (sensor.check_data_ready(&ready) == VL53L8CX_STATUS_OK && ready) {
    if (sensor.get_ranging_data(&results) == VL53L8CX_STATUS_OK) {
      frame_count++;
      const uint8_t z = CENTER_ZONE * VL53L8CX_NB_TARGET_PER_ZONE;
      uint8_t tgt = results.nb_target_detected[CENTER_ZONE];
      uint8_t st  = results.target_status[z];
      if (tgt > 0 && status_ok(st)) {
        Serial.printf("# frame %lu zone %u: %d mm (status %u)\n",
                      (unsigned long)frame_count, CENTER_ZONE,
                      (int)results.distance_mm[z], st);
      }
      if ((frame_count % 15) == 0) {
        print_grid(&results);
      }
    }
  }

  if (Serial.available()) {
    char c = (char)Serial.read();
    if (c == 'g') {
      print_grid(&results);
    } else if (c == 'r') {
      // R4 drill: re-init the sensor while it is still streaming. No
      // stop_ranging() first - that polls the MCU-stop bit for ~5 s and is
      // never used by the production sketches either.
      Serial.println("# forced re-init while streaming");
      ranging_ok = init_and_start();
    }
  }

  delay(2);
}