#include "LCDDisplay.h"

namespace rtc {

// initialization of static variables
const std::chrono::microseconds LCDDisplay::TASK_RUN_TIME = 300000us;
const std::chrono::milliseconds LCDDisplay::TASK_PERIOD = 1600ms;

LCDDisplay::LCDDisplay() {
}

void LCDDisplay::show(int currentGear, int currentRotationCount) {
  // simulate task computation by waiting for the required task run time
  wait_us(TASK_RUN_TIME.count());
  printf("Gear value is %d, wheel rotation count is %d\n", currentGear, currentRotationCount);
}

} // namespace
