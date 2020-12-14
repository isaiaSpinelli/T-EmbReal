#include "WheelCounterDevice.h"

namespace with_event_queue {
  
// initialization of static variables
const std::chrono::microseconds WheelCounterDevice::TASK_RUN_TIME = 200000us;
const std::chrono::milliseconds WheelCounterDevice::TASK_PERIOD = 400ms;
const std::chrono::milliseconds WheelCounterDevice::WHEEL_ROTATION_TIME = 200ms;

WheelCounterDevice::WheelCounterDevice() {
  // start the ticker for incrementing wheel counter
  m_ticker.attach(callback(this, &WheelCounterDevice::turn), WHEEL_ROTATION_TIME);
}

int WheelCounterDevice::getCurrentRotationCount() {
  // simulate task computation by waiting for the required task run time
  wait_us(TASK_RUN_TIME.count());
  return m_rotationCount;
}

void WheelCounterDevice::reset() {
  m_rotationCount = 0;
}

void WheelCounterDevice::turn() {
  // ISR context
  // increment rotation count
  // the call for incrementing the count should be atomic
  m_rotationCount++;
}

} // namespace
