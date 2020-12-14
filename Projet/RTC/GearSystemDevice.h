#pragma once

#include "mbed.h"

#include "USBSerial.h"

namespace rtc {

class GearSystemDevice {
public:
  // constructor used for event-driven behavior
  GearSystemDevice(mbed::Callback<void()> cb);

  // method called for updating the bike system
  int getCurrentGear();
  
private:
  // definition of task execution time
  static const std::chrono::microseconds TASK_RUN_TIME;

  // data members
  USBSerial m_usbSerial;
};

} // namespace
