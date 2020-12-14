#pragma once

#include "mbed.h"

namespace rtc {

class LCDDisplay {
public:

  LCDDisplay();

  // method called for displaying information
  void show(int currentGear, int currentRotationCount);

  // definition of task period time
  static const std::chrono::milliseconds TASK_PERIOD;
  
private:
  // data members
  // definition of task execution time
  static const std::chrono::microseconds TASK_RUN_TIME;
};

} // namespace
