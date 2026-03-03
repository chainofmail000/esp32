#pragma once

#include "config.h"

namespace Sensor {
void begin();
void update(SensorData& data);
void onPirInterrupt();
}
