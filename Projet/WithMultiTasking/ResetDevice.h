#pragma once

#include "mbed.h"

namespace with_multitasking {

class ResetDevice {
public:
  // constructor used for event-driven behavior
  ResetDevice(mbed::Callback<void()> cb);

private:
  // data members  
  // instance representing the reset button
  InterruptIn m_resetButton;
};

} // namespace
