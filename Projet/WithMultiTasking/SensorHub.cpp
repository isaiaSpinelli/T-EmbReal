#include "SensorHub.h"
#include "SensorData.h"
#include <chrono>


namespace with_multitasking {
  
const std::chrono::microseconds SensorHub::PERIOD_MEASURE = 1000ms;

SensorHub::SensorHub(Mail<SensorData, SENSOR_DATA_QUEUE_SIZE>& sensorMail) :
  m_thread(osPriorityNormal, OS_STACK_SIZE, nullptr, "SensorHub"),
  m_sensorMail(sensorMail) {
}

void SensorHub::start() {
  // initialize random seed
  //srand (time(NULL));

  // start a ticker for signaling a wheel rotation
  //m_ticker.attach(callback(this, &SensorHub::GetMeasure), PERIOD_MEASURE);

  // start a thread that will get atomspheric datas 
  m_thread.start(callback(this, &SensorHub::GetMeasureThread));

  printf("SensorHub started\n");
}

void SensorHub::GetMeasureThread(){
     printf("SensorHub thread started\n");
     m_ticker.attach(callback(this, &SensorHub::GetMeasure), PERIOD_MEASURE);
     while(true)
     {}
}

void SensorHub::GetMeasure(){
    m_temp = 3.;
    m_pressure = 3.;
    m_humidity = 5.1;
    SensorData* pSensorData = m_sensorMail.try_alloc();//_for(Kernel::wait_for_u32_forever);
    pSensorData->temp = m_temp;
    pSensorData->pressure = m_pressure;
    pSensorData->humidity = m_humidity;
    m_sensorMail.put(pSensorData);
};

} // namespace