#pragma once

#include "SensorData.h"

namespace with_multitasking {

struct ProcessedData {
    float averageSpeed = 0;
    float averagePower = 0;
    SensorData sensorData;
};

} // namespace

  