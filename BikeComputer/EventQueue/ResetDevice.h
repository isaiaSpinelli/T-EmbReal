#pragma once

#include "mbed.h"

namespace EventQueueNs {

class ResetDevice {
  public:
    ResetDevice();
    //ResetDevice(Callback<void()> callback);
    ResetDevice(mbed::Callback<void()> cb);

    // method called for checking the reset status
    bool checkReset();


  private:

    // definition of task execution time
    static const std::chrono::microseconds TASK_RUN_TIME;

    Callback<void()> cb;

    //static DigitalIn buttonReset;
    InterruptIn _interrupt;
    void setReset();
    volatile bool resetWithoutCallback=false;
};

} // namespace