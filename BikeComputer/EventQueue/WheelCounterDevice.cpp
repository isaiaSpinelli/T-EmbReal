#include "WheelCounterDevice.h"

namespace EventQueueNs {

// initialization of static variables
const std::chrono::milliseconds WheelCounterDevice::WHEELING_RATE = 50ms;

// the time that the task will actually take
const std::chrono::microseconds WheelCounterDevice::TASK_RUN_TIME = 100000us;
// the perdio time of the task
const std::chrono::milliseconds WheelCounterDevice::TASK_PERIOD = 400ms;
//const std::chrono::microseconds WheelCounterDevice::TASK_PERIOD = 100000us;


WheelCounterDevice::WheelCounterDevice() :
    m_currentWheelCounter(0) {

        // Lancer le thread avec la fonction read
      m_thread.start(callback(this, &WheelCounterDevice::turn ));
    }


int WheelCounterDevice::getCurrentRotationCount() const {

    // simulate task computation by waiting for the required task run time
    wait_us(TASK_RUN_TIME.count());
    return m_currentWheelCounter;
}


void WheelCounterDevice::reset(){
    m_currentWheelCounter = 0;
}


void WheelCounterDevice::turn() {

    while (true) {
        m_currentWheelCounter++;
        ThisThread::sleep_for(WHEELING_RATE);
    }
}

} // namespace