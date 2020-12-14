#include "BikeSystem.h"

#include "mbed_trace.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeSystem"
#endif // MBED_CONF_MBED_TRACE_ENABLE

namespace rtc {

// initialization of static variables
const char* BikeSystem::TASK_DESCRIPTORS[] = { "Reset", "Gear", "Count", "Display"};
BikeSystem::BikeSystem() :
  m_resetDevice(callback(this, &BikeSystem::setReset)),
  m_gearSystemDevice(callback(this, &BikeSystem::setNewGear)) {
}

void BikeSystem::start() {
  tr_info("Starting with RTC scheduler\n");

  // start the timer
  m_timer.start();

  // Add the tasks by priority order.
  // The task with the highest priority is the push reset task.
  // This task is not periodic and made ready to run in the setResetTaskToReady() function.
  m_taskInformationTable.addTask(this, &BikeSystem::checkAndPerformReset);
  // The next task is the task for updating the gear.
  // This task is not periodic and made ready to run in the setUpdateGearTaskToReady() function.
  m_taskInformationTable.addTask(this, &BikeSystem::updateCurrentGear);
  // the task with the next priority is the one for updating the current rotation count (periodic task)
  m_taskInformationTable.addTask(this, &BikeSystem::updateWheelRotationCount, WheelCounterDevice::TASK_PERIOD);  
  // the task with the next priority is the one for updating the LCD display (period task)
  m_taskInformationTable.addTask(this, &BikeSystem::updateDisplay, LCDDisplay::TASK_PERIOD);
  
  int nbrOfTasks = m_taskInformationTable.getNbrOfTasks();
  printf("Nbr of tasks in scheduler is %d\n", nbrOfTasks);

  // run a RTC scheduling  
  while (true) {
    // wait for a task to be ready to run
    m_taskInformationTable.waitForTaskReadyToRun();
    
    // tasks are ordered by priority
    // run the first ready to run task by priority order
    for (int taskIndex = 0; taskIndex < nbrOfTasks; taskIndex++) {
      if (m_taskInformationTable.isReadyToRun(taskIndex)) {
        // run the task
        m_taskInformationTable.execute(taskIndex);
        // note that the task ready to run status is reset after execution
        break;
      }
    }
  }
}

void BikeSystem::updateCurrentGear() {
  std::chrono::microseconds taskStartTime = m_timer.elapsed_time();
  
  // in case of event handling, check for a new gear
  if (m_newGear) {
    m_gear = m_gearSystemDevice.getCurrentGear();
    m_newGear = false;
  }

  logPeriodAndExecutionTime(GEAR_TASK_INDEX, taskStartTime);
}

void BikeSystem::updateWheelRotationCount() {
  std::chrono::microseconds taskStartTime = m_timer.elapsed_time();
  
  m_wheelRotationCount = m_wheelCounterDevice.getCurrentRotationCount();

  logPeriodAndExecutionTime(COUNT_TASK_INDEX, taskStartTime);
}
  
void BikeSystem::updateDisplay() {
  std::chrono::microseconds taskStartTime = m_timer.elapsed_time();
  
  m_lcdDisplay.show(m_gear, m_wheelRotationCount);

  logPeriodAndExecutionTime(DISPLAY_TASK_INDEX, taskStartTime);
}

void BikeSystem::checkAndPerformReset() {
  std::chrono::microseconds taskStartTime = m_timer.elapsed_time();
  
  // switch the next two lines for a polling or event-based handling of reset
  if (m_reset) {
    tr_debug("Reset task (event): response time is %d usecs", (int) (m_timer.elapsed_time().count() - m_resetTime.count())); 
    m_wheelCounterDevice.reset();
    m_reset = false;
  }

  logPeriodAndExecutionTime(RESET_TASK_INDEX, taskStartTime);
}

void BikeSystem::setReset() {
  m_reset = true;
  m_resetTime = m_timer.elapsed_time();
  // set the task to be ready to run 
  m_taskInformationTable.makeTaskReadyToRun(RESET_TASK_INDEX);
}

void BikeSystem::setNewGear() {
  m_newGear = true;
  // set the task to be ready to run 
  m_taskInformationTable.makeTaskReadyToRun(GEAR_TASK_INDEX);
}

void BikeSystem::logPeriodAndExecutionTime(int taskIndex, const std::chrono::microseconds& taskStartTime) {
  std::chrono::microseconds periodTime = taskStartTime - m_taskStartTime[taskIndex];  
  m_taskStartTime[taskIndex] = taskStartTime;
  std::chrono::microseconds taskEndTime = m_timer.elapsed_time();
  std::chrono::microseconds executionTime = taskEndTime - m_taskStartTime[taskIndex];  
  tr_debug("%s task: period %d usecs execution time %d usecs start time %d usecs", 
           TASK_DESCRIPTORS[taskIndex], 
           (int) periodTime.count(),
           (int) executionTime.count(),
           (int) m_taskStartTime[taskIndex].count()); 
}

} // namespace