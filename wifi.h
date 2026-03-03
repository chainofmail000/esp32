#pragma once

#include "config.h"

namespace WiFiControl {
void begin();
void handle(const SensorData& data, LightingState& light, SystemState state);
}
