/**
 * mode_b_multi_bus.ino  —  Teensy 4.0
 *
 * MODE B: five sensors spread over the Teensy 4.0's 3 SPI and 2 I2C buses,
 * one sensor per bus, so all five range concurrently.
 *
 *   S0: SPI   (LPSPI4)  SCK=13  MOSI=11  MISO=12  CS=10      header pins
 *   S1: SPI1  (LPSPI3)  SCK=27  MOSI=26  MISO=1   CS=0       SCK/MOSI on BOTTOM PADS
 *   S2: SPI2  (LPSPI1)  SCK=37  MOSI=35  MISO=34  CS=36      BOTTOM PADS
 *   S3: Wire  (LPI2C1)  SCL=19  SDA=18
 *   S4: Wire1 (LPI2C3)  SCL=16  SDA=17
 *
 * Hardware notes:
 *   - On the Teensy 4.0 only pins 0..23 are breadboardable; 24..39 are bottom
 *     SMT pads. S1 and S2 therefore need soldered pads.
 *   - One sensor per I2C bus keeps the default address 0x52.
 *   - The I2C carrier variant must pull SPI_I2C_N low and allow I2C_RST low
 *     (the library drives I2C_RST low for I2C). Add a bidirectional translator
 *     (TCA9406) for SDA/SCL; the AXC4T774 is unidirectional.
 *   - I2C runs at 400 kHz. With the trimmed ~320 B frame that is ~7.3 ms per
 *     sensor, so two I2C sensors use ~22 % of a 66.7 ms frame. The TCA9406 is
 *     rated for Fm+ (1 MHz), but 400 kHz gives far more rise-time margin on a
 *     wired harness, so 1 MHz is not used.
 *   - Pin 13 drives the on-board LED (SCK idles high in mode 3).
 *   - Pins 0/1 are Serial1; 16/17 are Serial4. No conflict unless those UARTs
 *     are used.
 *
 * Serial: USB CDC @ 115200. CSV: ms,S0,S1,S2,S3,S4 (-1 = no fresh valid target).
 */

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

// The I2C chunk cap (DEFAULT_I2C_BUFFER_LEN 128) is set in the patched library
// (vl53l8cx_platform_config_custom.h), not here: IO_Read/IO_Write are inline
// members, so a #define in this sketch would only change this translation unit.
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
#if !defined(DEFAULT_I2C_BUFFER_LEN) || DEFAULT_I2C_BUFFER_LEN > 128
#error "VL53L8CX I2C buffer cap missing: Teensy TX buffer is 137 bytes, need <= 128"
#endif

static const uint8_t NUM_SENSORS = 5;

// The external 3V3_EXT rail sits behind a host-controlled load switch whose ON
// pin has a 100k pull-down, so the rail is OFF whenever the Teensy is reset or
// unpowered. Must be raised before any bus pin is driven.
static const uint8_t RAIL_EN_PIN = 2;

static const uint32_t SENSOR_SPI_HZ = 3000000UL;   // DS14161 SPI limit
static const uint32_t SENSOR_I2C_HZ = 400000UL;    // Fm; 1 MHz Fm+ not needed at 320 B frames
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

// Set this to a GPIO if the I2C carrier routes I2C_RST to the host. The library
// drives it LOW for I2C. Leave -1 if the board ties I2C_RST low itself.
static const int I2C_RST_PIN = -1;

static inline bool status_ok(uint8_t s) { return s == 5 || s == 9; }

static const uint8_t CS0 = 10;
static const uint8_t CS1 = 0;
static const uint8_t CS2 = 36;

static VL53L8CX s0(&SPI,  CS0, -1, -1, SENSOR_SPI_HZ);
static VL53L8CX s1(&SPI1, CS1, -1, -1, SENSOR_SPI_HZ);
static VL53L8CX s2(&SPI2, CS2, -1, -1, SENSOR_SPI_HZ);
static VL53L8CX s3(&Wire,  -1, I2C_RST_PIN);
static VL53L8CX s4(&Wire1, -1, I2C_RST_PIN);

static VL53L8CX *const sensors[NUM_SENSORS] = {&s0, &s1, &s2, &s3, &s4};

static bool     sensor_ok[NUM_SENSORS]     = {false};
static uint8_t  consec_errors[NUM_SENSORS] = {0};
static uint8_t  silent_fails[NUM_SENSORS]  = {0};
static int16_t  last_mm[NUM_SENSORS];
static uint32_t last_update_ms[NUM_SENSORS];
static uint32_t last_retry_ms[NUM_SENSORS];
static VL53L8CX_ResultsData results[NUM_SENSORS];

static void park_all_cs_high() {
  const uint8_t cs[3] = {CS0, CS1, CS2};
  for (uint8_t i = 0; i < 3; i++) {
    pinMode(cs[i], OUTPUT);
    digitalWrite(cs[i], HIGH);
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
  // Raise the external rail first. While the switch is off, driving any CS
  // high pushes ~0.7 mA through that board's R7 into a dead 3V3 node.
  pinMode(RAIL_EN_PIN, OUTPUT);
  digitalWrite(RAIL_EN_PIN, HIGH);
  delay(5);                        // switch t_R ~95 us + LDO turn-on

  park_all_cs_high();

  Serial.begin(115200);
  uint32_t t0 = millis();
  while (!Serial && (millis() - t0) < 3000) {}

  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    last_mm[i] = -1;
    last_update_ms[i] = 0;
    last_retry_ms[i] = 0;
  }

  Serial.println("# VL53L8CX MODE B: 3 SPI + 2 I2C, concurrent (Teensy 4.0)");

  SPI.begin();
  SPI1.begin();
  SPI2.begin();
  Wire.begin();
  Wire.setClock(SENSOR_I2C_HZ);
  Wire1.begin();
  Wire1.setClock(SENSOR_I2C_HZ);

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

    if (sensors[i]->get_ranging_data(&results[i]) != VL53L8CX_STATUS_OK) {
      if (++consec_errors[i] >= MAX_CONSEC_ERRORS) {
        Serial.printf("# S%u demoted after %u errors\n", i, consec_errors[i]);
        demote(i);
      }
      continue;
    }

    consec_errors[i] = 0;
    silent_fails[i] = 0;
    const uint8_t z = CENTER_ZONE * VL53L8CX_NB_TARGET_PER_ZONE;
    uint8_t tgt = results[i].nb_target_detected[CENTER_ZONE];
    uint8_t stt = results[i].target_status[z];
    last_mm[i] = (tgt > 0 && status_ok(stt)) ? results[i].distance_mm[z] : -1;
    last_update_ms[i] = millis();
    updated_any = true;
  }

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