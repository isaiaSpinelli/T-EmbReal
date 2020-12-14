#pragma once

#include "mbed.h"

namespace with_event_queue {
  
class LCDDisplay {
public:
  LCDDisplay();

  // method called for displaying information
  void show(int currentGear, int currentRotationCount, int subTaskIndex = -1);

  // definition of task period time
  static const std::chrono::milliseconds TASK_PERIOD;
  
private:
  // data members
  // definition of task execution time
  static const std::chrono::microseconds TASK_RUN_TIME;
  static const std::chrono::microseconds SUBTASK_RUN_TIME; 

  // for simulating a task run time, we sleep for this time in the show() method
  static const int NBR_OF_SUBTASKS = 3;
};

} // namespace

