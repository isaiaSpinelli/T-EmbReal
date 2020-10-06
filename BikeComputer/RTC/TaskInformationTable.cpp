#include "TaskInformationTable.h"

// for template method instantiation
#include "BikeSystem.h"


namespace rtc {

TaskInformationTable::TaskInformationTable() {
  m_ticker.attach(callback(this, &TaskInformationTable::updateTaskStatus), taskUpdateInterval);
      
  // start the timer
  m_timer.start();

  // store the last update time as the current time
  m_lastUpdateTime = m_timer.elapsed_time();
}
    
template <typename T>
void TaskInformationTable::addTask(T *obj, void(T::*method)()) {
  // call the other addTask method with zero period - non periodic task
  addTask(obj, method, std::chrono::microseconds::zero());
}
    
template <typename T>
void TaskInformationTable::addTask(T *obj, void(T::*method)(), std::chrono::microseconds period) {
  if (m_nbrOfTasks >= MAX_NBR_OF_TASKS) {
    return;
  }
  m_taskInformationArray[m_nbrOfTasks].m_task = mbed::callback(obj, method);
  m_taskInformationArray[m_nbrOfTasks].m_period = period;
  // set the release time to the period time
  m_taskInformationArray[m_nbrOfTasks].m_releaseTime = period;

  // increment the number of tasks
  m_nbrOfTasks++;
}
    
int TaskInformationTable::getNbrOfTasks() const {
  return m_nbrOfTasks;
}

bool TaskInformationTable::isReadyToRun(int taskIndex) {
  if (taskIndex >= m_nbrOfTasks) {
    return false;
  }
  return m_taskInformationArray[taskIndex].m_readyToRun;
}
    
void TaskInformationTable::makeTaskReadyToRun(int taskIndex) {
  // this method is executed in the ISR context
  if (taskIndex >= m_nbrOfTasks) {
    return;
  }
  m_taskInformationArray[taskIndex].m_readyToRun = true;
  int readyEventFlag = (1UL << taskIndex);
  m_readyEventFlags.set(readyEventFlag);
}

void TaskInformationTable::waitForTaskReadyToRun() {
  // wait for the ready event to be set for any of the tasks
  int readyEventFlag = 0xFF & ((1UL << m_nbrOfTasks) - 1);
  // printf("Waiting for event flag: 0x%08lx\r\n", (unsigned long) readyEventFlag);
  uint32_t flags_read = m_readyEventFlags.wait_any(readyEventFlag);
  // printf("Got: 0x%08lx\r\n", (unsigned long) flags_read);
  // it can be that several flags have been set, meaning
  // that several tasks are ready to run
  // we need to set the flags again for tasks except the first one
  int nbrOfEventsSet = 0;
  for (int taskIndex = 0; taskIndex < m_nbrOfTasks; taskIndex++) {
    int readyEventFlag = (1UL << taskIndex);
    if (flags_read & readyEventFlag) {
      nbrOfEventsSet++;
      if (nbrOfEventsSet > 1) {
        m_readyEventFlags.set(readyEventFlag);
      }
    }
  }
}

void TaskInformationTable::execute(int taskIndex) {
  if (taskIndex >= m_nbrOfTasks) {
    return;
  }
  if (! m_taskInformationArray[taskIndex].m_readyToRun) {
    return;
  }
  // printf("Executing task with index %d\n", taskIndex);
  m_taskInformationArray[taskIndex].execute();

  // reset the readyToRun flag
  m_taskInformationArray[taskIndex].m_readyToRun = false;
}

// get task period for a giver task
  std::chrono::microseconds TaskInformationTable::getPeriodTask_us(int taskIndex){
    if (taskIndex >= m_nbrOfTasks) {
        return std::chrono::microseconds::zero();
    }

    return m_taskInformationArray[taskIndex].m_period;

  }

// this method is attached to the ticker and it executed within the ISR context
void TaskInformationTable::updateTaskStatus() {
  // get current time and compute elapsed time since last task update
  std::chrono::microseconds currentTime = m_timer.elapsed_time();
  std::chrono::microseconds elapsedTime = currentTime - m_lastUpdateTime;
  m_lastUpdateTime = currentTime; 

  // perform the update
  for (int taskIndex = 0; taskIndex < m_nbrOfTasks; taskIndex++) {
    // do not update the status of tasks that are not periodic
    if (m_taskInformationArray[taskIndex].m_period != std::chrono::microseconds::zero()) {
      if ((m_taskInformationArray[taskIndex].m_releaseTime - elapsedTime) <= std::chrono::microseconds::zero()) {
        // the task is ready to run
        int readyEventFlag = (1UL << taskIndex);
        m_readyEventFlags.set(readyEventFlag);
        m_taskInformationArray[taskIndex].m_readyToRun = true;
        //printf("Task %d is now ready to run\n", taskIndex);
        // reset the release time to the period time
        m_taskInformationArray[taskIndex].m_releaseTime = m_taskInformationArray[taskIndex].m_period;            
      }
      else {
        // decrease release time           
        m_taskInformationArray[taskIndex].m_releaseTime -= elapsedTime;
        // printf("Decreasing release time of task %d to %d\n", taskIndex, (int) m_taskInformationArray[taskIndex].m_releaseTime.count());
      }      
    }
  }
}

// instantiate the template methods for the BikeSystem class
template void TaskInformationTable::addTask(BikeSystem *obj, void(BikeSystem::*method)());
template void TaskInformationTable::addTask(BikeSystem *obj, void(BikeSystem::*method)(), std::chrono::microseconds period);

} // namespace