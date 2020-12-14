#include "LCDDisplay.h"

namespace static_scheduling {

// initialization of static variables
const std::chrono::microseconds LCDDisplay::TASK_RUN_TIME = 300000us;
const std::chrono::microseconds LCDDisplay::SUBTASK_RUN_TIME = 100000us;
const std::chrono::milliseconds LCDDisplay::TASK_PERIOD = 1600ms;

LCDDisplay::LCDDisplay() {
}

void LCDDisplay::show(int currentGear, int currentRotationCount, int subTaskIndex) {
  if (subTaskIndex == -1) {
    // simulate task computation by waiting for the required task run time
    wait_us(TASK_RUN_TIME.count());
    // use printf since we want to print in any case
    printf("Gear value is %d, wheel rotation count is %d\n", currentGear, currentRotationCount);
  } 
  else {    
    // simulate task computation by waiting for the required task run time
    wait_us(SUBTASK_RUN_TIME.count());
    if (subTaskIndex == (NBR_OF_SUBTASKS - 1)) {
      // use printf since we want to print in any case
      printf("Gear value is %d, wheel rotation count is %d\n", currentGear, currentRotationCount);
    }
  }
}

} // namespace
