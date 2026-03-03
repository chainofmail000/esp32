#include "lighting.h"

namespace {
float fadeProgress = 1.0f;
RgbColor fadeStart{};
uint32_t fadeStartMs = 0;
constexpr uint16_t FADE_DURATION_MS = 250;

uint8_t gammaCorrect(uint8_t value) {
  if (!Config::ENABLE_GAMMA_CORRECTION) {
    return value;
  }
  constexpr float gamma = 2.2f;
  const float normalized = static_cast<float>(value) / 255.0f;
  return static_cast<uint8_t>(pow(normalized, gamma) * 255.0f + 0.5f);
}

RgbColor mapDistanceToColor(float distanceCm) {
  const float t = (distanceCm - Config::MIN_DISTANCE_CM) /
                  (Config::MAX_DISTANCE_CM - Config::MIN_DISTANCE_CM);
  const float clamped = constrain(t, 0.0f, 1.0f);
  RgbColor color;
  color.r = static_cast<uint8_t>((1.0f - clamped) * 255.0f);
  color.g = 0;
  color.b = static_cast<uint8_t>(clamped * 255.0f);
  return color;
}

uint8_t computeBrightness(uint16_t ldrRaw) {
  if (!Config::ENABLE_LDR_BRIGHTNESS) {
    return Config::MAX_BRIGHTNESS;
  }

  const float ldrNormalized = constrain(static_cast<float>(ldrRaw) / 4095.0f, 0.0f, 1.0f);
  const float scaled = (1.0f - ldrNormalized) *
                       (Config::MAX_BRIGHTNESS - Config::MIN_BRIGHTNESS) +
                       Config::MIN_BRIGHTNESS;
  return static_cast<uint8_t>(scaled);
}

void writeColor(const RgbColor& color, uint8_t brightness) {
  const auto scale = [brightness](uint8_t value) -> uint8_t {
    return static_cast<uint8_t>((static_cast<uint16_t>(value) * brightness) / 255);
  };

  ledcWrite(Config::PWM_CH_RED, gammaCorrect(scale(color.r)));
  ledcWrite(Config::PWM_CH_GREEN, gammaCorrect(scale(color.g)));
  ledcWrite(Config::PWM_CH_BLUE, gammaCorrect(scale(color.b)));
}
}  // namespace

namespace Lighting {
void begin() {
  ledcSetup(Config::PWM_CH_RED, Config::PWM_FREQ, Config::PWM_RES_BITS);
  ledcSetup(Config::PWM_CH_GREEN, Config::PWM_FREQ, Config::PWM_RES_BITS);
  ledcSetup(Config::PWM_CH_BLUE, Config::PWM_FREQ, Config::PWM_RES_BITS);
  ledcAttachPin(Config::PIN_PWM_RED, Config::PWM_CH_RED);
  ledcAttachPin(Config::PIN_PWM_GREEN, Config::PWM_CH_GREEN);
  ledcAttachPin(Config::PIN_PWM_BLUE, Config::PWM_CH_BLUE);
}

void updateFromSensors(const SensorData& data, LightingState& light) {
  if (!light.manualOverride) {
    light.target = mapDistanceToColor(data.distanceCm);
  }
  light.brightness = computeBrightness(data.ldrRaw);

  if (light.target.r != fadeStart.r || light.target.g != fadeStart.g ||
      light.target.b != fadeStart.b) {
    fadeStart = light.current;
    fadeStartMs = millis();
    fadeProgress = 0.0f;
  }
}

void render(LightingState& light) {
  if (fadeProgress < 1.0f) {
    const uint32_t elapsed = millis() - fadeStartMs;
    fadeProgress = constrain(static_cast<float>(elapsed) / FADE_DURATION_MS, 0.0f, 1.0f);
    light.current.r = static_cast<uint8_t>(static_cast<int16_t>(fadeStart.r) +
                                          static_cast<int16_t>((static_cast<int16_t>(light.target.r) -
                                                                static_cast<int16_t>(fadeStart.r)) * fadeProgress));
    light.current.g = static_cast<uint8_t>(static_cast<int16_t>(fadeStart.g) +
                                          static_cast<int16_t>((static_cast<int16_t>(light.target.g) -
                                                                static_cast<int16_t>(fadeStart.g)) * fadeProgress));
    light.current.b = static_cast<uint8_t>(static_cast<int16_t>(fadeStart.b) +
                                          static_cast<int16_t>((static_cast<int16_t>(light.target.b) -
                                                                static_cast<int16_t>(fadeStart.b)) * fadeProgress));
  } else {
    light.current = light.target;
  }

  writeColor(light.current, light.brightness);
}

void setManualOverride(const RgbColor& color, LightingState& light) {
  light.manualOverride = true;
  light.target = color;
  fadeStart = light.current;
  fadeProgress = 0.0f;
  fadeStartMs = millis();
}

void clearManualOverride(LightingState& light) { light.manualOverride = false; }
}  // namespace
