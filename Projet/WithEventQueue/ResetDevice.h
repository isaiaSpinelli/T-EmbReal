#pragma once

#include "mbed.h"

namespace with_event_queue {
  
class ResetDevice {
public:
  // constructor used for event-driven behavior
  ResetDevice(mbed::Callback<void()> cb);

private:
  // definition of task execution time
  static const std::chrono::microseconds TASK_RUN_TIME;

  // data members  
  // instance representing the reset button
  InterruptIn m_resetButton;
};

} // namespace 