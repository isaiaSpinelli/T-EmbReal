#pragma once

#include "mbed.h"

#include "USBSerial.h"

namespace EventQueueNs {

class GearSystemDevice {
public:
  // constructor used for simulating the device with a thread
  GearSystemDevice();
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
  //USBSerial m_usbSerial;

  Callback<void()> cb;

  int m_currentGear = 0;
  
  Thread m_thread;
  void read();

};

} // namespace