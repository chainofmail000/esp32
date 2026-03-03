#pragma once

#include <Arduino.h>

namespace Config {
constexpr uint8_t PIN_TRIG = 5;
constexpr uint8_t PIN_ECHO = 18;
constexpr uint8_t PIN_PIR = 19;
constexpr uint8_t PIN_LDR = 34;

constexpr uint8_t PIN_PWM_RED = 25;
constexpr uint8_t PIN_PWM_GREEN = 26;
constexpr uint8_t PIN_PWM_BLUE = 27;

constexpr uint8_t PWM_CH_RED = 0;
constexpr uint8_t PWM_CH_GREEN = 1;
constexpr uint8_t PWM_CH_BLUE = 2;
constexpr uint16_t PWM_FREQ = 5000;
constexpr uint8_t PWM_RES_BITS = 8;

constexpr float MIN_DISTANCE_CM = 20.0f;
constexpr float MAX_DISTANCE_CM = 120.0f;
constexpr uint32_t SENSOR_PERIOD_MS = 50;
constexpr uint32_t LIGHTING_PERIOD_MS = 16;  // ~60 FPS
constexpr uint32_t WIFI_PERIOD_MS = 100;
constexpr uint32_t POWER_PERIOD_MS = 250;

constexpr uint32_t NO_MOTION_TIMEOUT_MS = 60000;
constexpr uint32_t LONG_INACTIVITY_SLEEP_MS = 300000;

constexpr float EMA_ALPHA = 0.25f;
constexpr bool ENABLE_KALMAN = false;

constexpr bool ENABLE_WIFI = true;
constexpr bool ENABLE_GAMMA_CORRECTION = true;
constexpr bool ENABLE_LDR_BRIGHTNESS = true;

constexpr uint16_t ECHO_TIMEOUT_US = 30000;

constexpr uint8_t MAX_BRIGHTNESS = 255;
constexpr uint8_t MIN_BRIGHTNESS = 20;
}  // namespace Config

enum class SystemState : uint8_t {
  ACTIVE,
  IDLE,
  SLEEP,
};

struct SensorData {
  float distanceCm = Config::MAX_DISTANCE_CM;
  bool motionDetected = false;
  uint16_t ldrRaw = 0;
  uint32_t lastMotionMs = 0;
};

struct RgbColor {
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
};

struct LightingState {
  RgbColor current{};
  RgbColor target{};
  uint8_t brightness = Config::MAX_BRIGHTNESS;
  bool manualOverride = false;
};
