#pragma once

#include "mbed.h"

#include "USBSerial.h"

namespace with_event_queue {
  
class GearSystemDevice {
public:
  // constructor used for event-driven behavior
  GearSystemDevice(mbed::Callback<void()> cb);

  // method called for updating the bike system
  int getCurrentGear();
  
private:
  // reading rate in milliseconds when running a separate thread
  // The gear value is updated every second
  static const std::chrono::milliseconds READING_RATE;

  // definition of task execution time
  static const std::chrono::microseconds TASK_RUN_TIME;

  // data members
  USBSerial m_usbSerial;
};

} // namespace
