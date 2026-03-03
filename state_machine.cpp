#include "state_machine.h"

namespace StateMachine {
SystemState update(const SensorData& data, SystemState current) {
  const uint32_t now = millis();

  if (data.motionDetected) {
    return SystemState::ACTIVE;
  }

  const uint32_t sinceMotion = now - data.lastMotionMs;
  if (sinceMotion >= Config::LONG_INACTIVITY_SLEEP_MS) {
    return SystemState::SLEEP;
  }

  if (sinceMotion >= Config::NO_MOTION_TIMEOUT_MS) {
    return SystemState::IDLE;
  }

  return current == SystemState::SLEEP ? SystemState::IDLE : current;
}
}  // namespace
