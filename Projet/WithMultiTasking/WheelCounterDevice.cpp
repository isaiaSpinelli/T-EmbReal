#include "WheelCounterDevice.h"

namespace with_multitasking {
  
// initialization of static variables
const std::chrono::milliseconds WheelCounterDevice::WHEEL_ROTATION_TIME = 1000ms;

WheelCounterDevice::WheelCounterDevice(Queue<int, COUNT_QUEUE_SIZE>& countQueue) :
  m_countQueue(countQueue) {
}

void WheelCounterDevice::start() {
  // initialize random seed
  srand (time(NULL));

  // start a ticker for signaling a wheel rotation
  m_ticker.attach(callback(this, &WheelCounterDevice::turn), WHEEL_ROTATION_TIME);
  printf("WheelCounterDevice started\n");
}

void WheelCounterDevice::turn() {
  // ISR context
  m_rotationCount++;
  if (m_countQueue.try_put((int*) m_rotationCount)) {
    m_rotationCount = 0;
  } 
}

} // namespace