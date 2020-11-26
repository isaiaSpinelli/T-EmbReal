#pragma once

#include "mbed.h"
#include "mbed_trace.h"

#include "LCDDisplay.h"
#include "GearSystemDevice.h"
#include "WheelCounterDevice.h"
#include "ResetDevice.h"

#include "TaskInformationTable.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeSystem"
#endif // MBED_CONF_MBED_TRACE_ENABLE

//#define USE_SECOND_THREAD 

namespace EventQueueNs {

enum STATE_CYCLING  { gear=0, count, display_1, reset, count_2, display_2, gear_2, count_3, display_3, reset_2, count_4, wait  };

class BikeSystem {
public:
  BikeSystem();

  void start();
  
private:


#if defined(MBED_ALL_STATS_ENABLED)
  static const int MAX_THREAD_INFO = 10;
  mbed_stats_heap_t m_heap_info = {0};  
  mbed_stats_stack_t m_stack_info[MAX_THREAD_INFO] = {0};
  void getAndPrintStatistics();
  void printDiffs();
#endif


  void queueNoP();

  void updateCurrentGear();
  void updateWheelRotationCount();
  void checkAndPerformReset();
  //void updateDisplay(int subTaskIndex = -1);
  void updateDisplay();

  void controleTimeExecution(std::chrono::microseconds &startTime, Timer &timer, long long TimeTestMS, char* task );

  ResetDevice resetDevice;
  void setReset();
  volatile bool resetOn=false;
  Timer timer_reset;
  std::chrono::microseconds startReset;

  GearSystemDevice gearSystemDevice;
  void setGear();
  volatile bool gearOn=false;


  WheelCounterDevice wheelCounterDevice;
  
  LCDDisplay lcdDisplay;

  int m_currentGear = 0;
  int m_currentWheelCounter = 0;

  STATE_CYCLING stateCycling = gear;


  Timer m_timer;

  EventQueue m_eventQueue;

#if defined(USE_SECOND_THREAD)
  // used in ISRs for deferring event handling job to the queue
  EventQueue queue_periodic;
#endif
  

  
  
};


} // namespace