/*
  OpenPlant Arduino
  Open-source soil moisture monitor and optional low-voltage watering controller.

  Safety:
  - Keep the prototype low-voltage.
  - Never drive a pump directly from an Arduino I/O pin.
  - Keep PUMP_ENABLED false during calibration and initial testing.
  - Use a correctly rated driver and pump power supply.
*/

#include <Arduino.h>

// ---------- Pin configuration ----------
const uint8_t MOISTURE_PIN = A0;
const uint8_t PUMP_PIN = 7;
const uint8_t STATUS_LED_PIN = LED_BUILTIN;

// ---------- Calibration ----------
// Replace these with readings from YOUR sensor and soil.
// The mapping works whether the wet reading is higher or lower than the dry reading.
int DRY_RAW = 750;
int WET_RAW = 350;

// ---------- Control configuration ----------
const int WATER_ON_BELOW_PERCENT = 30;
const int WATER_OFF_ABOVE_PERCENT = 45;

const unsigned long SAMPLE_INTERVAL_MS = 1000UL;
const unsigned long MAX_PUMP_RUNTIME_MS = 5000UL;
const unsigned long SOAK_DELAY_MS = 15000UL;

// Keep false until calibration and driver tests are complete.
const bool PUMP_ENABLED = false;

// Some relay modules turn on when their input is LOW.
const bool PUMP_ACTIVE_LOW = false;

// ---------- State ----------
bool pumpOn = false;
unsigned long lastSampleMs = 0;
unsigned long pumpStartedMs = 0;
unsigned long nextEvaluationMs = 0;

void setPump(bool turnOn) {
  if (!PUMP_ENABLED) {
    turnOn = false;
  }

  if (turnOn == pumpOn) {
    return;
  }

  pumpOn = turnOn;
  const uint8_t onLevel = PUMP_ACTIVE_LOW ? LOW : HIGH;
  const uint8_t offLevel = PUMP_ACTIVE_LOW ? HIGH : LOW;

  digitalWrite(PUMP_PIN, pumpOn ? onLevel : offLevel);
  digitalWrite(STATUS_LED_PIN, pumpOn ? HIGH : LOW);

  if (pumpOn) {
    pumpStartedMs = millis();
    Serial.println(F("Pump state: ON"));
  } else {
    Serial.println(F("Pump state: OFF"));
    nextEvaluationMs = millis() + SOAK_DELAY_MS;
  }
}

int readMoistureRaw() {
  // Average a few readings to reduce noise.
  const uint8_t samples = 8;
  unsigned long total = 0;

  for (uint8_t i = 0; i < samples; i++) {
    total += analogRead(MOISTURE_PIN);
    delay(5);
  }

  return (int)(total / samples);
}

int rawToPercent(int raw) {
  if (DRY_RAW == WET_RAW) {
    return 0; // Avoid division by zero if calibration is invalid.
  }

  long percent = ((long)(DRY_RAW - raw) * 100L) / (DRY_RAW - WET_RAW);
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;
  return (int)percent;
}

void setup() {
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);

  // Initialize output to OFF before any sensor readings.
  const uint8_t offLevel = PUMP_ACTIVE_LOW ? HIGH : LOW;
  digitalWrite(PUMP_PIN, offLevel);
  digitalWrite(STATUS_LED_PIN, LOW);

  Serial.begin(9600);
  while (!Serial && millis() < 2000UL) {
    ; // Wait briefly on boards with native USB serial.
  }

  Serial.println(F("\nOpenPlant starting"));
  Serial.println(F("Pump output is disabled unless PUMP_ENABLED is true."));
  lastSampleMs = millis();
}

void loop() {
  const unsigned long now = millis();

  // Safety cutoff: stop a pump that has run continuously for too long.
  if (pumpOn && (now - pumpStartedMs >= MAX_PUMP_RUNTIME_MS)) {
    Serial.println(F("Safety cutoff: maximum pump runtime reached."));
    setPump(false);
  }

  if (now - lastSampleMs < SAMPLE_INTERVAL_MS) {
    return;
  }
  lastSampleMs = now;

  const int raw = readMoistureRaw();
  const int moisturePercent = rawToPercent(raw);

  Serial.print(F("raw="));
  Serial.print(raw);
  Serial.print(F(" moisture="));
  Serial.print(moisturePercent);
  Serial.print(F("% pump="));
  Serial.println(pumpOn ? F("ON") : F("OFF"));

  if (!PUMP_ENABLED) {
    return;
  }

  // Avoid starting another watering cycle during the soak period.
  if (!pumpOn && (long)(now - nextEvaluationMs) < 0) {
    return;
  }

  // Hysteresis: start below the lower threshold, stop above the upper threshold.
  if (!pumpOn && moisturePercent < WATER_ON_BELOW_PERCENT) {
    setPump(true);
  } else if (pumpOn && moisturePercent >= WATER_OFF_ABOVE_PERCENT) {
    setPump(false);
  }
}
