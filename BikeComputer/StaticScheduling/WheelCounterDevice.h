#pragma once

#include "mbed.h"

namespace static_scheduling {


class WheelCounterDevice {
  public:
    WheelCounterDevice();

    // method called for getting the current wheel rotation count
    int getCurrentRotationCount() const;
 
    // method called for resetting the counter
    void reset();
  

  private:

    void turn();

    // reading rate in milliseconds when running a separate thread
    // The wheel counter value is updated every (50 ms)
    static const std::chrono::milliseconds WHEELING_RATE;

    // definition of task execution time (200 ms)
    static const std::chrono::microseconds TASK_RUN_TIME;

    int m_currentWheelCounter = 0;
   

    Thread m_thread;
};

} // namespace