#pragma once

#include "mbed.h"

#include "config.h"

namespace with_multitasking {
  
class WheelCounterDevice {
public:
  WheelCounterDevice(Queue<int, COUNT_QUEUE_SIZE>& countQueue);

  void start();

private:
  // definition of task execution time
  static const std::chrono::microseconds TASK_RUN_TIME; 
  // definition of wheel rotation time
  static const std::chrono::milliseconds WHEEL_ROTATION_TIME;

  // data members 
  LowPowerTicker m_ticker;
  Queue<int, COUNT_QUEUE_SIZE>& m_countQueue;
  int m_rotationCount = 0;
  
  // private methods
  void turn();
};

} // 