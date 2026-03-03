#include "sensor.h"

namespace {
volatile bool pirInterruptFlag = false;
float emaDistance = Config::MAX_DISTANCE_CM;
float kalmanEstimate = Config::MAX_DISTANCE_CM;
float kalmanErrorEstimate = 1.0f;

float applyKalman(float measurement) {
  constexpr float processNoise = 0.1f;
  constexpr float sensorNoise = 4.0f;
  kalmanErrorEstimate += processNoise;
  const float kalmanGain = kalmanErrorEstimate / (kalmanErrorEstimate + sensorNoise);
  kalmanEstimate += kalmanGain * (measurement - kalmanEstimate);
  kalmanErrorEstimate *= (1.0f - kalmanGain);
  return kalmanEstimate;
}

float readDistanceCm() {
  digitalWrite(Config::PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(Config::PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(Config::PIN_TRIG, LOW);

  const uint32_t duration = pulseIn(Config::PIN_ECHO, HIGH, Config::ECHO_TIMEOUT_US);
  if (duration == 0) {
    return Config::MAX_DISTANCE_CM;
  }

  const float distance = duration * 0.0343f * 0.5f;
  return constrain(distance, Config::MIN_DISTANCE_CM, Config::MAX_DISTANCE_CM);
}
}  // namespace

namespace Sensor {
void begin() {
  pinMode(Config::PIN_TRIG, OUTPUT);
  pinMode(Config::PIN_ECHO, INPUT);
  pinMode(Config::PIN_PIR, INPUT_PULLUP);
  pinMode(Config::PIN_LDR, INPUT);
  attachInterrupt(digitalPinToInterrupt(Config::PIN_PIR), onPirInterrupt, CHANGE);
}

void onPirInterrupt() { pirInterruptFlag = true; }

void update(SensorData& data) {
  const float rawDistance = readDistanceCm();
  emaDistance = Config::EMA_ALPHA * rawDistance + (1.0f - Config::EMA_ALPHA) * emaDistance;
  data.distanceCm = Config::ENABLE_KALMAN ? applyKalman(emaDistance) : emaDistance;

  if (pirInterruptFlag) {
    pirInterruptFlag = false;
    data.motionDetected = digitalRead(Config::PIN_PIR) == HIGH;
    if (data.motionDetected) {
      data.lastMotionMs = millis();
    }
  }

  data.ldrRaw = analogRead(Config::PIN_LDR);
}
}  // namespace
