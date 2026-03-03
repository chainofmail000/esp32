#pragma once

#include "config.h"

namespace StateMachine {
SystemState update(const SensorData& data, SystemState current);
}
