#pragma once

#include "GearSystemDevice.h"
#include "WheelCounterDevice.h"
#include "ResetDevice.h"
#include "LCDDisplay.h"

namespace static_scheduling {

class BikeSystem {
public:
  BikeSystem();

  void start();
  
private:
  void updateCurrentGear();
  void updateWheelRotationCount();
  
  void checkAndPerformReset();
  void updateDisplay(int subTaskIndex);

  // number of wheel rotation (read from the device)
  int m_wheelRotationCount = 0; 
  // current gear (read from the device)
  int m_gear = 0;
  // data member that represents the device for manipulating the gear
  GearSystemDevice m_gearSystemDevice;
  // data member that represents the device for counting wheel rotations  
  WheelCounterDevice m_wheelCounterDevice;
  // data member that represents the device used for resetting
  ResetDevice m_resetDevice;
  // data member that represents the device display
  LCDDisplay m_lcdDisplay;
  
  void logPeriodAndExecutionTime(int taskIndex, const std::chrono::microseconds& taskStartTime);
  Timer m_timer;
  static const int NBR_OF_TASKS = 4;
  static const int RESET_TASK_INDEX = 0;
  static const int GEAR_TASK_INDEX = 1;
  static const int COUNT_TASK_INDEX = 2;
  static const int DISPLAY_TASK_INDEX = 3;
  static const char* TASK_DESCRIPTORS[];
  std::chrono::microseconds m_taskStartTime[NBR_OF_TASKS];  
  std::chrono::microseconds m_resetTime;
};

} // namespace
