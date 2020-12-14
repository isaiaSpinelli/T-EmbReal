#pragma once

#include "mbed.h"

namespace with_event_queue {
  
class WheelCounterDevice {
public:
  WheelCounterDevice();

  // method called for getting the current wheel rotation count
  int getCurrentRotationCount();

  // method called for resetting the counter
  void reset();

  // definition of task period time
  static const std::chrono::milliseconds TASK_PERIOD;

private:
  // definition of task execution time
  static const std::chrono::microseconds TASK_RUN_TIME; 
  // definition of wheel rotation time
  static const std::chrono::milliseconds WHEEL_ROTATION_TIME;

  // data members 
  LowPowerTicker m_ticker;
  int m_rotationCount = 0;
  
  // private methods
  void turn();
};

} // namespace
