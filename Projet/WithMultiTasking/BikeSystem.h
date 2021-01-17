#pragma once

#include "GearSystemDevice.h"
#include "SensorData.h"
#include "WheelCounterDevice.h"
#include "ButtonDevice.h"
#include "LCDDisplay.h"
#include "ProcessedData.h"
#include "SensorHub.h"
#include "config.h"

namespace with_multitasking {

class BikeSystem {
public:
  BikeSystem();

  void start();

private:
  // private methods
  void buttonFall();
  void buttonRise();
  void displayTimeButton();

  void setReset();
  void performReset();
  void setNewGear();
  void updateCurrentGear();

  // data member that represents the device for manipulating the gear
  GearSystemDevice m_gearSystemDevice;
  // data member that represents the device for counting wheel rotations  
  WheelCounterDevice m_wheelCounterDevice;
  // data member that represents the device used for resetting
  ButtonDevice m_buttonDevice;
  // data member that represents the device display
  LCDDisplay m_lcdDisplay;
  // data member that represent the Sensor Hub
  SensorHub m_sensorHub;
  
  // used for performance measurements
  // wheel circumference in mm
  static const int WHEEL_CIRCUMFERENCE = 2200;
  Queue<int, COUNT_QUEUE_SIZE> m_countQueue;
  Mail<ProcessedData, PROCESSED_DATA_QUEUE_SIZE> m_processedMail;
  Mutex m_externNewData;
  Mail<SensorData,SENSOR_DATA_QUEUE_SIZE> m_sensorMail;
  volatile uint32_t m_totalRotationCount = 0;
  volatile uint32_t m_currentGear = 0;
  Thread m_processingThread;
  EventQueue m_eventQueueForISRs;
  void processData();
  Timer m_timer;
  std::chrono::microseconds m_timeButtonFall;
  std::chrono::microseconds m_timeButtonRise;

  SensorData pSensorDataAverage = {0,0,0};
  bool firstTimeEnvironementalValues = true;

#if defined(MBED_ALL_STATS_ENABLED)
  static const int MAX_THREAD_INFO = 10;
  mbed_stats_heap_t m_heap_info = {0};  
  mbed_stats_stack_t m_stack_info[MAX_THREAD_INFO] = {0};
  void getAndPrintStatistics();
  void printDiffs();
  void printRuntimeMemoryMap();
#endif

};

} // namespace
