#pragma once

#include "mbed.h"

namespace with_multitasking {

class ButtonDevice {
public:
  // constructor used for event-driven behavior
  ButtonDevice(mbed::Callback<void()> cb_fall, mbed::Callback<void()> cb_rise);

private:
  // data members  
  // instance representing the reset button
  InterruptIn m_resetButton;
};

} // namespace
