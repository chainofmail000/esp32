#include "power.h"

#include <esp_sleep.h>

namespace {
SystemState lastState = SystemState::ACTIVE;
}

namespace Power {
void begin() {
  esp_sleep_enable_ext0_wakeup(static_cast<gpio_num_t>(Config::PIN_PIR), 1);
}

void handle(SystemState state) {
  if (state == lastState) {
    return;
  }

  if (state == SystemState::IDLE) {
    // Placeholder for light sleep entry policies.
  } else if (state == SystemState::SLEEP) {
    delay(50);
    esp_deep_sleep_start();
  }

  lastState = state;
}
}  // namespace
