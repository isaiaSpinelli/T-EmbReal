#include "GearSystemDevice.h"

namespace with_multitasking {
  
GearSystemDevice::GearSystemDevice(mbed::Callback<void()> cb) :
  m_usbSerial(true) {
  
  // attach the callback to the serial port
  m_usbSerial.attach(cb);
}

int GearSystemDevice::getCurrentGear() {
  // when a callback is registered, we need to read data
  // on the serial port upon getting the current gear
  while (m_usbSerial.available()) {
    // printf("Got character %c\n", m_usbSerial.getc());
    char str[2] = { (char) m_usbSerial.getc(), 0};
    m_currentGear = atoi(str);
  }

  return m_currentGear;
}

} // namespace
