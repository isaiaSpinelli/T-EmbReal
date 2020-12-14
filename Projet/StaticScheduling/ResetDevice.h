#pragma once

#include "mbed.h"

namespace static_scheduling {

class ResetDevice {
public:
  // constructor 
  ResetDevice();

  // method called for checking the reset status
  bool checkReset();

private:
  // definition of task execution time
  static const std::chrono::microseconds TASK_RUN_TIME;

  // reading rate in milliseconds when running a separate thread
  // The gear value is updated every second
  static const std::chrono::milliseconds READING_RATE;
  
  // data members  
  // instance representing the reset button
  DigitalIn m_resetButton;
};

} // namespace
