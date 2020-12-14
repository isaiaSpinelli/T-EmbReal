#include "GearSystemDevice.h"

namespace with_event_queue {
  
// initialization of static variables
const std::chrono::milliseconds GearSystemDevice::READING_RATE = 1000ms;
const std::chrono::microseconds GearSystemDevice::TASK_RUN_TIME = 100000us;

GearSystemDevice::GearSystemDevice(mbed::Callback<void()> cb) :
  m_usbSerial(true) {
  
  // attach the callback to the serial port
  m_usbSerial.attach(cb);
}

int GearSystemDevice::getCurrentGear() {
  // when a callback is registered, we need to read data
  // on the serial port upon getting the current gear
  int currentGear = 0;
  while (m_usbSerial.available()) {
    // printf("Got character %c\n", m_usbSerial.getc());
    char str[2] = { (char) m_usbSerial.getc(), 0};
    currentGear = atoi(str);
  }

  // simulate task computation by waiting for the required task run time
  wait_us(TASK_RUN_TIME.count());
  return currentGear;
}

} // namespace
