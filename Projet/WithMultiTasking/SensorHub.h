#pragma once

#include "DevI2C.h"
#include "mbed.h"

#include "config.h"
#include "SensorData.h"
#include "HTS221Sensor.h"
#include "LPS22HBSensor.h"

namespace with_multitasking {

//static DevI2C devI2c(PB_11,PB_10);
//static LPS22HBSensor press_temp(&devI2c);
//static HTS221Sensor hum_temp(&devI2c);

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

  bool newData;

  DevI2C* devI2c = new DevI2C(PB_11,PB_10);
  LPS22HBSensor* press_temp = new LPS22HBSensor(devI2c);
  HTS221Sensor* hum_temp = new HTS221Sensor(devI2c);
  


  //int m_rotationCount = 0;
};

} // 