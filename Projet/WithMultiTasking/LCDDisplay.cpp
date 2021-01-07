#include "LCDDisplay.h"


namespace with_multitasking {
  
LCDDisplay::LCDDisplay(Mail<ProcessedData, PROCESSED_DATA_QUEUE_SIZE>& processedMail) :
  m_thread(osPriorityNormal, OS_STACK_SIZE, nullptr, "LCDDisplay"),
  m_processedMail(processedMail) {
}

void LCDDisplay::start() {
  // start a thread that will display information
  m_thread.start(callback(this, &LCDDisplay::displayInfo));

  printf("LCDDisplay started\n");
}

void LCDDisplay::displayInfo() {
  printf("LCDDisplay thread started\n");
  while (true) {
    ProcessedData* pProcessedData = m_processedMail.try_get_for(Kernel::wait_for_u32_forever);
    float speed = pProcessedData->averageSpeed;
    float power = pProcessedData->averagePower;
    SensorData sensorData = pProcessedData->sensorData;
    //printf("Average speed is %d.%d\n", (int) speed, (int) ((speed - (int) speed) * 100000));
    printf("03 Average speed is %f with power %f\n", speed, power);
    printf("temp : %f, humidity : %f, pressure : %f\n\n",sensorData.temp, sensorData.humidity,sensorData.pressure);
    m_processedMail.free(pProcessedData);    
  }
}

} // namespace
