#pragma once

#include "mbed.h"

namespace static_scheduling {

class LCDDisplay {
  public:
    LCDDisplay();

    // method called for displaying information
    void show(int currentGear, int currentRotationCount,              
              int subTaskIndex = -1);

  private:

    // definition of task execution time
    static const std::chrono::microseconds TASK_RUN_TIME;
};

} // namespace