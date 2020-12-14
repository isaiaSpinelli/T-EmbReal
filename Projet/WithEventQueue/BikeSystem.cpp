#include "BikeSystem.h"

#include "mbed_trace.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeSystem"
#endif // MBED_CONF_MBED_TRACE_ENABLE

namespace with_event_queue {
  
// initialization of static variables
const char* BikeSystem::TASK_DESCRIPTORS[] = { "Reset", "Gear", "Count", "Display"};

BikeSystem::BikeSystem() :
  m_resetDevice(callback(this, &BikeSystem::setReset)),
  m_gearSystemDevice(callback(this, &BikeSystem::setNewGear)) {
}

void BikeSystem::start() {
#if defined(USE_SECOND_THREAD)
  tr_info("Starting with EventQueue and additional thread");
#else
  tr_info("Starting with EventQueue");
#endif

  // start the timer
  m_timer.start();

  // The EventQueue has no concept of event priority. 
  // If you schedule events to run at the same time, the order in which the events run relative to one another is undefined. 
  // The EventQueue only schedules events based on time.
  // If you want to separate your events into different priorities, you must instantiate an EventQueue for each priority. 
  // You must appropriately set the priority of the thread dispatching each EventQueue instance.
  // This version does not explicetly create threads and uses a single queue.
#if defined(USE_SECOND_THREAD)
  // create a thread with higher priority for hanlding aperiodic events
  Thread aperiodicEventThread(osPriorityAboveNormal);
  aperiodicEventThread.start(callback(&m_eventQueueForISRs, &EventQueue::dispatch_forever));
#endif

  // schedule periodic events
  m_eventQueue.call_every(WheelCounterDevice::TASK_PERIOD, callback(this, &BikeSystem::updateWheelRotationCount));
  m_eventQueue.call_every(LCDDisplay::TASK_PERIOD, callback(this, &BikeSystem::updateDisplayInOneTask));

  // dispatch events in the queue forever
  m_eventQueue.dispatch_forever();
}

void BikeSystem::updateCurrentGear() {
  std::chrono::microseconds taskStartTime = m_timer.elapsed_time();
  
  // get the new gear 
  m_gear = m_gearSystemDevice.getCurrentGear();

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

void BikeSystem::updateDisplayInOneTask() { 
  updateDisplay(-1);
}

void BikeSystem::checkAndPerformReset() {
  std::chrono::microseconds taskStartTime = m_timer.elapsed_time();
  
  // reset the wheel counter device
  tr_debug("Reset task (event): response time is %d usecs", (int) (m_timer.elapsed_time().count() - m_resetTime.count())); 
  m_wheelCounterDevice.reset();

  logPeriodAndExecutionTime(RESET_TASK_INDEX, taskStartTime);
}

void BikeSystem::setReset() {
  m_resetTime = m_timer.elapsed_time();
  
#if defined(USE_SECOND_THREAD)
  // defer the job to the event queue
  m_eventQueueForISRs.call(mbed::callback(this, &BikeSystem::checkAndPerformReset));
#else
  m_eventQueue.call(mbed::callback(this, &BikeSystem::checkAndPerformReset));
#endif
}

void BikeSystem::setNewGear() {
#if defined(USE_SECOND_THREAD)
  // defer the job to the event queue if ap
  m_eventQueueForISRs.call(mbed::callback(this, &BikeSystem::updateCurrentGear));
#else
  m_eventQueue.call(mbed::callback(this, &BikeSystem::updateCurrentGear));
#endif
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
