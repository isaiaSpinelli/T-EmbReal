#pragma once

#include "mbed.h"

#include "TaskInformation.h"

namespace rtc {
  
class TaskInformationTable {
public: 
  TaskInformationTable();
   
  // called by the scheduler
  // tasks are added by priority order, highest priority first
  // tasks have a period when they are started at a fixed interval
  // tasks without period are made ready to run upon specific interrupt
  template <typename T>
  void addTask(T *obj, void(T::*method)());    
  template <typename T>
  void addTask(T *obj, void(T::*method)(), std::chrono::microseconds period);

  // called by the scheduler for getting the number of tasks
  int getNbrOfTasks() const;
  // called by the scheduler for querying task status
  bool isReadyToRun(int taskIndex);
  
  // called upon event such as button press
  // this method is executed in the ISR context
  void makeTaskReadyToRun(int taskIndex);

  // called by the scheduler for waiting for a ready to run task
  void waitForTaskReadyToRun();
  
  // called by the scheduler for executing a given task
  void execute(int taskIndex);

  // get task period for a giver task
  std::chrono::microseconds getPeriodTask_us(int taskIndex);
    
private:
  // private methods
  void updateTaskStatus();

  // data members
  int m_nbrOfTasks = 0;
  static const int MAX_NBR_OF_TASKS = 4;
  TaskInformation m_taskInformationArray[MAX_NBR_OF_TASKS];
  // thread and event queue used for updating the task status
  std::chrono::microseconds m_lastUpdateTime;
  LowPowerTimer m_timer;
  EventFlags m_readyEventFlags;
  const std::chrono::microseconds taskUpdateInterval = 10000us;
  LowPowerTicker m_ticker;
};

} // namespace