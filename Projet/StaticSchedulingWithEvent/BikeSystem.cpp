#include "BikeSystem.h"

#include "mbed_trace.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeSystem"
#endif // MBED_CONF_MBED_TRACE_ENABLE

namespace static_scheduling_with_event {

// initialization of static variables
const char* BikeSystem::TASK_DESCRIPTORS[] = { "Reset", "Gear", "Count", "Display"};
BikeSystem::BikeSystem() :
  m_resetDevice(callback(this, &BikeSystem::setReset)),
  m_gearSystemDevice(callback(this, &BikeSystem::setNewGear)) 
{
}

void BikeSystem::start() {
  tr_info("Starting Super-Loop with event");

  // start the timer
  m_timer.start();

  while (true) {
    // register the time at the beginning of the cyclic schedule period
    std::chrono::microseconds startTime = m_timer.elapsed_time();

    // perform tasks as documented in the timetable
    updateCurrentGear();
    updateWheelRotationCount();
    updateDisplay(0);
    checkAndPerformReset();
    updateWheelRotationCount();
    updateDisplay(1);
    updateCurrentGear();
    updateWheelRotationCount();
    updateDisplay(2);
    checkAndPerformReset();
    updateWheelRotationCount();
    
    ThisThread::sleep_for(std::chrono::milliseconds(100));
    
    // register the time at the end of the cyclic schedule period and print the elapsed time for the period
    std::chrono::microseconds endTime = m_timer.elapsed_time();
    tr_debug("Elapsed time is %d milliseconds", (int) (endTime.count() - startTime.count())/1000);
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
  
void BikeSystem::updateDisplay(int subTaskIndex) {
  std::chrono::microseconds taskStartTime = m_timer.elapsed_time();
  
  m_lcdDisplay.show(m_gear, m_wheelRotationCount, subTaskIndex);

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
}

void BikeSystem::setNewGear() {
  m_newGear = true;
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
