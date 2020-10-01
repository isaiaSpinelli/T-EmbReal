#include "GearSystemDevice.h"

namespace StaticSchedulingWithEvent {

// initialization of static variables
const std::chrono::milliseconds GearSystemDevice::READING_RATE = 1000ms;
const std::chrono::microseconds GearSystemDevice::TASK_RUN_TIME = 100000us;

StaticSchedulingWithEvent::GearSystemDevice::GearSystemDevice() :
  m_usbSerial(true) {

    // Lancer le thread avec la fonction read
    m_thread.start(callback(this, &GearSystemDevice::read ));
}

StaticSchedulingWithEvent::GearSystemDevice::GearSystemDevice(Callback<void()> callback) :
     m_usbSerial(true), cb(callback) {

    // Callback lors de la reception de caractère
    this->m_usbSerial.attach(cb);
}


void GearSystemDevice::read() {
 //while (true) {
   // if ( m_usbSerial.available()) {
      char str[2] = { (char) m_usbSerial.getc(), 0};
      m_currentGear = atoi(str);
   // }
    //ThisThread::sleep_for(READING_RATE);
  //}
}

int GearSystemDevice::getCurrentGear() {
  // when a callback is registered, we need to read data
  // on the serial port upon getting the current gear

   GearSystemDevice::read();

  // simulate task computation by waiting for the required task run time
  wait_us(TASK_RUN_TIME.count());
  return m_currentGear;
}

} // namespace