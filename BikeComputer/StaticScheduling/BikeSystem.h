#pragma once

#include "mbed.h"
#include "mbed_trace.h"

#include "LCDDisplay.h"
#include "GearSystemDevice.h"
#include "WheelCounterDevice.h"
#include "ResetDevice.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeSystem"
#endif // MBED_CONF_MBED_TRACE_ENABLE


namespace static_scheduling {

enum STATE_CYCLING  { gear=0, count, display_1, reset, count_2, display_2, gear_2, count_3, display_3, reset_2, count_4, wait  };

class BikeSystem {
public:
  BikeSystem();

  void start();
  
private:
  void updateCurrentGear();
  void updateWheelRotationCount();
  void checkAndPerformReset();
  void updateDisplay(int subTaskIndex = -1);

  void controleTimeExecution(std::chrono::microseconds &startTime, Timer &timer, long long TimeTestMS, char* task );

  static_scheduling::GearSystemDevice gearSystemDevice;
  static_scheduling::WheelCounterDevice wheelCounterDevice;
  static_scheduling::ResetDevice resetDevice;
  static_scheduling::LCDDisplay lcdDisplay;

  int m_currentGear = 0;
  int m_currentWheelCounter = 0;

  STATE_CYCLING stateCycling = gear;
  
};


} // namespace