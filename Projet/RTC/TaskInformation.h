#pragma once

#include "mbed.h"

class TaskInformation {
  public:
    TaskInformation() :
      m_task(NULL),
      m_period(0),
      m_releaseTime(0),
      m_readyToRun(false) {
    }
    
    void execute() {
      if (m_task != NULL) {
        m_task();
      }
    }

    // the task to be executed (void method)
    mbed::Callback<void()> m_task;
    std::chrono::microseconds m_period;
    std::chrono::microseconds m_releaseTime;
    bool m_readyToRun;
};



