#pragma once

#include "mbed.h"

#include "USBSerial.h"

namespace with_multitasking {

class GearSystemDevice {
public:
  // constructor used for event-driven behavior
  GearSystemDevice(mbed::Callback<void()> cb);

  // method called for updating the bike system
  int getCurrentGear();
  
private:
  // data members
  //USBSerial m_usbSerial;

  int m_currentGear = 0;
};

} // namespace
