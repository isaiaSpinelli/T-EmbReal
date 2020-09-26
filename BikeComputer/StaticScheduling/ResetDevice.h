#pragma once

#include "mbed.h"

namespace static_scheduling {

class ResetDevice {
  public:
    ResetDevice();
    ResetDevice(Callback<void()> callback);

    // method called for checking the reset status
    bool checkReset();


  private:

    // definition of task execution time
    static const std::chrono::microseconds TASK_RUN_TIME;

    Callback<void()> callback;

    static DigitalIn buttonReset;
};

} // namespace