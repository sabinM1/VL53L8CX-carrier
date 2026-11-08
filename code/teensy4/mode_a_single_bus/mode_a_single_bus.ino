/**
 * mode_a_single_bus.ino  —  Teensy 4.0
 *
 * MODE A: all five sensors on ONE SPI bus, one CS per sensor, polled
 * sequentially. Uses the header-pin SPI bus (pins 0..23 are breadboardable).
 *
 * Bus: SPI (LPSPI4) — SCK=13, MOSI=11, MISO=12. CS = 10,9,8,7,6.
 *
 * Pin 13 also drives the on-board LED; in mode 3 SCK idles high, so the LED is
 * lit and toggles during transfers. Harmless, but noted.
 *
 * The carrier's OE->NCS_S scheme keeps exactly one board driving MISO.
 * Requires the patched library (CS-settle delay).
 *
 * Serial: USB CDC @ 115200. CSV: ms,S0..S4 (-1 = no fresh valid target).
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
#if !defined(VL53L8CX_DISABLE_AMBIENT_PER_SPAD) || !defined(VL53L8CX_DISABLE_SIGNAL_PER_SPAD) \
 || !defined(VL53L8CX_DISABLE_MOTION_INDICATOR)
#error "Output trimming missing: the patched platform_config_custom.h is not being used"
#endif
static_assert(sizeof(VL53L8CX_ResultsData) <= 512,
              "ResultsData should be the trimmed layout (~258 bytes)");

static const uint8_t PIN_SCK  = 13;
static const uint8_t PIN_MOSI = 11;
static const uint8_t PIN_MISO = 12;

static const uint8_t NUM_SENSORS = 5;
static const uint8_t NCS_PINS[NUM_SENSORS] = {10, 9, 8, 7, 6};

// The external 3V3_EXT rail sits behind a host-controlled load switch whose ON
// pin has a 100k pull-down, so the rail is OFF whenever the Teensy is reset or
// unpowered. Must be raised before any bus pin is driven.
static const uint8_t RAIL_EN_PIN = 2;

static const uint32_t SENSOR_SPI_HZ = 3000000UL;
static const uint8_t  RANGING_HZ    = 15;
static const uint8_t  CENTER_ZONE   = 27;
static const uint8_t  MAX_CONSEC_ERRORS = 50;
static const uint32_t RETRY_PERIOD_MS   = 30000;
static const uint8_t  SILENT_BACKOFF_AFTER = 3;    // consecutive silent demotions
static const uint32_t SILENT_BACKOFF_MULT  = 10;   // 30 s -> 300 s cap
static const uint16_t FRAME_PERIOD_MS   = 1000 / RANGING_HZ;
static const uint32_t STALE_MS          = 3UL * FRAME_PERIOD_MS;

// An SPI read never reports an error, so a hung sensor is only detectable by
// the absence of frames. Scale with the configured rate so a slow (4x4, 1 Hz)
// sensor is not demoted on a normal gap.
static const uint32_t NO_FRAME_TIMEOUT_MS =
  (5UL * FRAME_PERIOD_MS > 2000UL) ? (5UL * FRAME_PERIOD_MS) : 2000UL;

static inline bool status_ok(uint8_t s) { return s == 5 || s == 9; }

static VL53L8CX s0(&SPI, NCS_PINS[0], -1, -1, SENSOR_SPI_HZ);
static VL53L8CX s1(&SPI, NCS_PINS[1], -1, -1, SENSOR_SPI_HZ);
static VL53L8CX s2(&SPI, NCS_PINS[2], -1, -1, SENSOR_SPI_HZ);
static VL53L8CX s3(&SPI, NCS_PINS[3], -1, -1, SENSOR_SPI_HZ);
static VL53L8CX s4(&SPI, NCS_PINS[4], -1, -1, SENSOR_SPI_HZ);

static VL53L8CX *const sensors[NUM_SENSORS] = {&s0, &s1, &s2, &s3, &s4};

static bool     sensor_ok[NUM_SENSORS]     = {false};
static uint8_t  consec_errors[NUM_SENSORS] = {0};
static uint8_t  silent_fails[NUM_SENSORS]  = {0};
static int16_t  last_mm[NUM_SENSORS];
static uint32_t last_update_ms[NUM_SENSORS];
static uint32_t last_retry_ms[NUM_SENSORS];
static VL53L8CX_ResultsData results;       // reused, 64 zones

static void park_all_ncs_high() {
  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    pinMode(NCS_PINS[i], OUTPUT);
    digitalWrite(NCS_PINS[i], HIGH);
  }
}

static bool configure(uint8_t i) {
  uint8_t st = sensors[i]->init();
  if (st != VL53L8CX_STATUS_OK) { Serial.printf("# S%u init FAILED (%u)\n", i, st); return false; }
  st = sensors[i]->set_resolution(VL53L8CX_RESOLUTION_8X8);
  if (st != VL53L8CX_STATUS_OK) { Serial.printf("# S%u set_resolution FAILED (%u)\n", i, st); return false; }
  st = sensors[i]->set_ranging_frequency_hz(RANGING_HZ);
  if (st != VL53L8CX_STATUS_OK) { Serial.printf("# S%u set_frequency FAILED (%u)\n", i, st); return false; }
  st = sensors[i]->set_ranging_mode(VL53L8CX_RANGING_MODE_CONTINUOUS);
  if (st != VL53L8CX_STATUS_OK) { Serial.printf("# S%u set_mode FAILED (%u)\n", i, st); return false; }
  return true;
}

static bool start(uint8_t i) {
  uint8_t st = sensors[i]->start_ranging();
  if (st != VL53L8CX_STATUS_OK) { Serial.printf("# S%u start_ranging FAILED (%u)\n", i, st); return false; }
  return true;
}

static void demote(uint8_t i) {
  sensor_ok[i] = false;
  consec_errors[i] = 0;
  last_mm[i] = -1;
  last_retry_ms[i] = millis();
  // stop_ranging() is never called anywhere in this sketch. A demoted sensor
  // may be dead, and stop_ranging() would poll its MCU-stop bit for ~5 s; the
  // retry path must not block healthy sensors. init() starts with a software
  // reboot, so bring_up() does not need the device stopped first.
}

static bool probe(uint8_t i) {
  // is_alive() is a few register accesses. Probing first keeps a dead or
  // mis-strapped sensor from burning init()'s full poll chain (~2 s boot poll
  // + firmware download + ~2 s verify) on every attempt.
  for (uint8_t a = 0; a < 3; a++) {
    uint8_t alive = 0;
    if (sensors[i]->is_alive(&alive) == VL53L8CX_STATUS_OK && alive) return true;
    delay(50);   // the sensor may still be coming out of reset
  }
  return false;
}

static bool bring_up(uint8_t i) {
  if (!probe(i)) {
    Serial.printf("# S%u is_alive FAILED\n", i);
    return false;
  }
  // One attempt only: the RETRY_PERIOD_MS loop is the retry mechanism, and
  // every extra attempt here blocks polling for all healthy sensors.
  if (configure(i) && start(i)) {
    consec_errors[i] = 0;
    last_update_ms[i] = millis();
    return true;
  }
  return false;
}

static void bring_up_all() {
  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    sensor_ok[i] = false;
    if (!probe(i)) {
      Serial.printf("# S%u not responding at boot\n", i);
      continue;
    }
    for (uint8_t a = 0; a < 3 && !sensor_ok[i]; a++) {
      sensor_ok[i] = configure(i);
      if (!sensor_ok[i]) delay(50);
    }
  }
  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    if (sensor_ok[i]) {
      sensor_ok[i] = start(i);
      if (sensor_ok[i]) {
        last_update_ms[i] = millis();
        Serial.printf("# S%u ranging at %u Hz\n", i, RANGING_HZ);
      }
    }
  }
}

void setup() {
  // Raise the external rail first. While the switch is off, driving any NCS
  // high pushes ~0.7 mA through that board's R7 into a dead 3V3 node.
  pinMode(RAIL_EN_PIN, OUTPUT);
  digitalWrite(RAIL_EN_PIN, HIGH);
  delay(5);                        // switch t_R ~95 us + LDO turn-on

  park_all_ncs_high();

  Serial.begin(115200);
  uint32_t t0 = millis();
  while (!Serial && (millis() - t0) < 3000) {}

  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    last_mm[i] = -1;
    last_update_ms[i] = 0;
    last_retry_ms[i] = 0;
  }

  Serial.println("# VL53L8CX MODE A: single SPI bus (header pins), sequential");

  SPI.begin();
  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    sensors[i]->begin();
  }
  bring_up_all();

  Serial.println("# ms,S0,S1,S2,S3,S4");
}

void loop() {
  bool updated_any = false;

  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    if (!sensor_ok[i]) {
      continue;
    }

    uint8_t ready = 0;
    uint8_t st = sensors[i]->check_data_ready(&ready);
    if (st != VL53L8CX_STATUS_OK) {
      if (++consec_errors[i] >= MAX_CONSEC_ERRORS) {
        Serial.printf("# S%u demoted after %u errors\n", i, consec_errors[i]);
        demote(i);
      }
      continue;
    }
    if (!ready) {
      continue;
    }

    if (sensors[i]->get_ranging_data(&results) != VL53L8CX_STATUS_OK) {
      if (++consec_errors[i] >= MAX_CONSEC_ERRORS) {
        Serial.printf("# S%u demoted after %u errors\n", i, consec_errors[i]);
        demote(i);
      }
      continue;
    }

    consec_errors[i] = 0;
    silent_fails[i] = 0;
    const uint8_t z = CENTER_ZONE * VL53L8CX_NB_TARGET_PER_ZONE;
    uint8_t tgt = results.nb_target_detected[CENTER_ZONE];
    uint8_t stt = results.target_status[z];
    last_mm[i] = (tgt > 0 && status_ok(stt)) ? results.distance_mm[z] : -1;
    last_update_ms[i] = millis();
    updated_any = true;
  }

  // Print only when at least one sensor produced a frame.
  if (updated_any) {
    uint32_t now = millis();
    Serial.printf("%lu", (unsigned long)now);
    for (uint8_t i = 0; i < NUM_SENSORS; i++) {
      bool fresh = sensor_ok[i] && (now - last_update_ms[i] <= STALE_MS);
      Serial.printf(",%d", fresh ? (int)last_mm[i] : -1);
    }
    Serial.println();
  }

  // Retry one dead sensor per pass, at most every RETRY_PERIOD_MS. This runs
  // before the no-frame check because bring_up() blocks the whole loop for up
  // to ~2 s, which would otherwise age every healthy sensor past its timeout.
  bool did_retry = false;
  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    uint32_t period = (silent_fails[i] >= SILENT_BACKOFF_AFTER)
                        ? RETRY_PERIOD_MS * SILENT_BACKOFF_MULT
                        : RETRY_PERIOD_MS;
    if (!sensor_ok[i] && (millis() - last_retry_ms[i] >= period)) {
      last_retry_ms[i] = millis();
      Serial.printf("# retry S%u\n", i);
      sensor_ok[i] = bring_up(i);
      did_retry = true;
      break;
    }
  }

  if (did_retry) {
    // Re-baseline every healthy sensor: a blocking bring_up() is not their
    // fault, and without this one wedged sensor blanks the whole array.
    uint32_t t = millis();
    for (uint8_t i = 0; i < NUM_SENSORS; i++) {
      if (sensor_ok[i]) last_update_ms[i] = t;
    }
  } else {
    // No-frame timeout: demote a sensor that stops delivering frames even
    // though its reads keep returning OK (the SPI path cannot report an error).
    // Three consecutive silent demotions cost real array duty on every retry,
    // so back the retry period off x10 (capped) until a frame arrives again.
    uint32_t now_ms = millis();
    for (uint8_t i = 0; i < NUM_SENSORS; i++) {
      if (sensor_ok[i] && (now_ms - last_update_ms[i] > NO_FRAME_TIMEOUT_MS)) {
        if (silent_fails[i] < SILENT_BACKOFF_AFTER &&
            ++silent_fails[i] == SILENT_BACKOFF_AFTER) {
          Serial.printf("# S%u silent %ux -> retry period %lu ms\n", i,
                        SILENT_BACKOFF_AFTER,
                        (unsigned long)(RETRY_PERIOD_MS * SILENT_BACKOFF_MULT));
        }
        Serial.printf("# S%u no frame for %lu ms -> demoted\n", i,
                      (unsigned long)(now_ms - last_update_ms[i]));
        demote(i);
      }
    }
  }

  delay(2);
}