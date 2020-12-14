#pragma once

#include "mbed.h"

#include "config.h"
#include "ProcessedData.h"

namespace with_multitasking {

class LCDDisplay {
public:

  LCDDisplay(Mail<ProcessedData, PROCESSED_DATA_QUEUE_SIZE>& processedMail);

  void start();

  // definition of task period time
  static const std::chrono::milliseconds TASK_PERIOD;
  
private:
  // private methods
  void displayInfo();
  
  // data members
  Mail<ProcessedData, PROCESSED_DATA_QUEUE_SIZE>& m_processedMail;
  Thread m_thread;
};

} // namespace

