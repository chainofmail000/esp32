#pragma once

#include "config.h"

namespace Lighting {
void begin();
void updateFromSensors(const SensorData& data, LightingState& light);
void render(LightingState& light);
void setManualOverride(const RgbColor& color, LightingState& light);
void clearManualOverride(LightingState& light);
}
