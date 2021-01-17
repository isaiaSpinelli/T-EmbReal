#include "SensorHub.h"
#include "DevI2C.h"
#include "LPS22HBSensor.h"
#include <cstdio>


namespace with_multitasking {
  
const std::chrono::microseconds SensorHub::PERIOD_MEASURE = 1000ms;

SensorHub::SensorHub(Mail<SensorData, SENSOR_DATA_QUEUE_SIZE>& sensorMail, Mutex& externNewDataMutex) :
  m_thread(osPriorityNormal, OS_STACK_SIZE, nullptr, "SensorHub"),
  m_sensorMail(sensorMail),
  newData(false),
  m_externNewDataMutex(externNewDataMutex){
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
     hum_temp->init(NULL);
     press_temp->init(NULL);
     hum_temp->enable();
     press_temp->enable();
     m_ticker.attach(callback(this, &SensorHub::GetMeasure), PERIOD_MEASURE);
     while(true)
     {
        m_externNewDataMutex.lock();
        if(newData || externNewData)
        {
            hum_temp->get_temperature(&m_temp);
            hum_temp->get_humidity(&m_humidity);
            press_temp->get_pressure(&m_pressure);
            SensorData* pSensorData = m_sensorMail.try_alloc();//_for(Kernel::wait_for_u32_forever);
            if (pSensorData != nullptr)
            {
                pSensorData->temp = m_temp;
                pSensorData->pressure = m_pressure;
                pSensorData->humidity = m_humidity;
                m_sensorMail.put(pSensorData);
            }
            newData = false;
            externNewData = false;
        }
        m_externNewDataMutex.unlock();
     }
}

void SensorHub::GetMeasure(){
    /*m_temp = 3.;
    m_pressure = 3.;
    m_humidity = 5.1;*/
    newData = true;
};

void SensorHub::GetMeasureTemp(){
    /*m_temp = 3.;
    m_pressure = 3.;
    m_humidity = 5.1;*/
    m_externNewDataMutex.lock();
    newData = true;
    m_externNewDataMutex.unlock();
};

} // namespace