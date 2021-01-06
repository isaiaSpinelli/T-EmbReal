#pragma once

#include "mbed.h"

#include "config.h"
#include "SensorData.h"
#include "HTS221Sensor.h"
#include "LPS22HBSensor.h"

namespace with_multitasking {
  
class SensorHub {
public:
  SensorHub(Mail<SensorData, SENSOR_DATA_QUEUE_SIZE>& sensorMail);

  void start();
  //Shouldn't this be private ? 
  void GetMeasureThread();
  void GetMeasure();

private:
  // definition of task execution time
  static const std::chrono::microseconds PERIOD_MEASURE; 

  // data members 
  LowPowerTicker m_ticker;
  Thread m_thread;
  Mail<SensorData, SENSOR_DATA_QUEUE_SIZE>& m_sensorMail;
  float m_temp = 0.0;
  float m_pressure = 0.0;
  float m_humidity = 0.0;




  //int m_rotationCount = 0;
};

} // 