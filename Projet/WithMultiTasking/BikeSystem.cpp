#include "BikeSystem.h"
	
#if MBED_CONF_MBED_TRACE_ENABLE
#include "mbed_trace.h"
#define TRACE_GROUP "BikeSystem"
#endif // MBED_CONF_MBED_TRACE_ENABLE


namespace with_multitasking {
  
BikeSystem::BikeSystem() :
  m_processingThread(osPriorityNormal, OS_STACK_SIZE, nullptr, "ProcessingThread"),
  m_resetDevice(callback(this, &BikeSystem::setReset)),
  m_gearSystemDevice(callback(this, &BikeSystem::setNewGear)),
  m_wheelCounterDevice(m_countQueue),
  m_lcdDisplay(m_processedMail) {
}

void BikeSystem::setReset() {  
  m_resetTime = m_timer.elapsed_time();
  // defer the job to the event queue
  m_eventQueueForISRs.call(mbed::callback(this, &BikeSystem::performReset));
}

void BikeSystem::performReset() {
  std::chrono::microseconds currentTime = m_timer.elapsed_time();
  tr_debug("Reset task (event): response time is %d usecs", (int) (currentTime.count() - m_resetTime.count())); 
 
  core_util_atomic_store_u32(&m_totalRotationCount, 0);
}

void BikeSystem::setNewGear() {
  // defer the job to the event queue
  m_eventQueueForISRs.call(mbed::callback(this, &BikeSystem::updateCurrentGear));
}

void BikeSystem::updateCurrentGear() {
  // get the new gear 
  uint32_t gear = m_gearSystemDevice.getCurrentGear();

  core_util_atomic_store_u32(&m_currentGear, gear);
}

void BikeSystem::start() {
  // start the timer
  m_timer.start();

  // the wheel counter and lcd display must be started
  // the reset and gear system device are event-based

  // start the wheel counter device
  m_wheelCounterDevice.start();

  // start the lcd display
  m_lcdDisplay.start();
  
  // start the thread responsible for running deferred ISRs
  
  // start the processing thread
  m_processingThread.start(callback(this, &BikeSystem::processData));

  tr_debug("Bike system started\n");

  //m_eventQueue.call_every( 1s, this, &BikeSystem::getAndPrintStatistics);

  // dispatch event on the ISR queue forever
  m_eventQueueForISRs.dispatch_forever();
}

void BikeSystem::processData() {
  
  // get the start time before entering the loop
  std::chrono::microseconds startTime;
    
  // process data forever  
  while (true) {
    // get the rotation count
    int rotationCount = 0;
    m_countQueue.try_get_for(Kernel::wait_for_u32_forever, (int**) &rotationCount);

    // at first rotation (upon reset), get the start time and wait for next rotation
    if (m_totalRotationCount == 0) {
      startTime = m_timer.elapsed_time();

      // update the rotation count
      m_totalRotationCount += rotationCount;
    }
    else {
      // get the elapsed time since last rotation count
      std::chrono::microseconds currentTime = m_timer.elapsed_time();
      std::chrono::microseconds elapsedSinceLastUpdate = currentTime - startTime;

      // update the rotation count
      m_totalRotationCount += rotationCount;
    
      // compute distance in m
      long distance = (WHEEL_CIRCUMFERENCE * m_totalRotationCount) / 1000;
      // compute time in seconds
      long totalElapsedTimeInSecs = elapsedSinceLastUpdate.count() / 1000000;
      // compute average speed in km/h
      float averageSpeed = (float) (distance * 1000) / (float) (totalElapsedTimeInSecs * 3600);
      // printf("Bike Average speed is %d.%d\n", (int) averageSpeed, (int) ((averageSpeed - (int) averageSpeed) * 100000));
    
      // push the new processed data
      ProcessedData* pProcessedData = m_processedMail.try_alloc_for(Kernel::wait_for_u32_forever);
      pProcessedData->averageSpeed = averageSpeed;
      pProcessedData->averagePower = m_currentGear;
      m_processedMail.put(pProcessedData);
    }
  }
}



#if defined(MBED_ALL_STATS_ENABLED)

// main stack info      : mbed_app.json
// others stack info    : mbed-os/cmsis/device/rtos/mbed_lib.json
void BikeSystem::getAndPrintStatistics() {

    printDiffs();
    
    mbed_stats_heap_get(&m_heap_info);
    tr_debug("\tHEAP ! ");

    tr_debug("\tBytes allocated currently: %d ", m_heap_info.current_size);
    tr_debug("\tMax bytes allocated at a given time: %d ", m_heap_info.max_size);
    tr_debug("\tCumulative sum of bytes ever allocated: %d ", m_heap_info.total_size);
    tr_debug("\tCurrent number of bytes allocated for the heap: %d ", m_heap_info.reserved_size);
    tr_debug("\tCurrent number of allocations: %d ", m_heap_info.alloc_cnt);
    tr_debug("\tNumber of failed allocations: %d ", m_heap_info.alloc_fail_cnt);


    mbed_stats_stack_t global_stack_info = {0};
    mbed_stats_stack_get(&global_stack_info);
    tr_debug("\tSTACK ! ");
    tr_debug("\tid: %d ", global_stack_info.thread_id);
    tr_debug("\tMaximum number of bytes used on the stack: %d ", global_stack_info.max_size);
    tr_debug("\tCurrent number of bytes allocated for the stack: %d ", global_stack_info.reserved_size);
    tr_debug("\tNumber of stacks stats accumulated in the structure: %d ", global_stack_info.stack_cnt);

     mbed_stats_stack_get_each(m_stack_info, MAX_THREAD_INFO);
     tr_debug("\tThread Stack Info");

    tr_debug("Thread Stack Info:");
    for (int i = 0; i < MAX_THREAD_INFO; i++) {
      if (m_stack_info[i].thread_id != 0) {
        tr_debug("\tThread: %d", i);
        tr_debug("\t\tThread Id: 0x%08X with name %s", m_stack_info[i].thread_id, osThreadGetName((osThreadId_t) m_stack_info[i].thread_id));
        tr_debug("\t\tMaximum number of bytes used on the stack: %d",  m_stack_info[i].max_size);     
        tr_debug("\t\tCurrent number of bytes allocated for the stack: %d",  m_stack_info[i].reserved_size);    
        tr_debug("\t\tNumber of stacks stats accumulated in the structure: %d",  m_stack_info[i].stack_cnt);           
      }
    }

}


void BikeSystem::printDiffs() {
    {
    tr_debug("MemoryStats (Heap):");
    mbed_stats_heap_t heap_info = {0};
    mbed_stats_heap_get(&heap_info);
    int current_size_diff = heap_info.current_size - m_heap_info.current_size;
    if (current_size_diff > 0) {
          tr_debug("\tBytes allocated increased by %d to %d bytes", current_size_diff, heap_info.current_size);
    }
    int max_size_diff = heap_info.max_size - m_heap_info.max_size;
    if (max_size_diff > 0) {
      tr_debug("\tMax bytes allocated at a given time increased by %d to %d bytes (max heap size is %d bytes)", max_size_diff, heap_info.max_size, heap_info.reserved_size);
    }
    m_heap_info = heap_info;
  }
  {
    mbed_stats_stack_t global_stack_info = {0};
    mbed_stats_stack_get(&global_stack_info);
    tr_debug("Cumulative Stack Info:");
    int max_size_diff = global_stack_info.max_size - global_stack_info.max_size;
    if (max_size_diff > 0) {
      tr_debug("\tMaximum number of bytes used on the stack increased by %d to %d bytes (stack size is %d bytes)", max_size_diff, global_stack_info.max_size, global_stack_info.reserved_size);
    }
    uint32_t stack_cnt_diff = global_stack_info.stack_cnt - global_stack_info.stack_cnt;
    if (stack_cnt_diff > 0) {
      tr_debug("\tNumber of stacks stats accumulated increased by %d to %d", stack_cnt_diff, global_stack_info.stack_cnt);
    }
    global_stack_info = global_stack_info;
    
    mbed_stats_stack_t stack_info[MAX_THREAD_INFO] = {0};  
    mbed_stats_stack_get_each(stack_info, MAX_THREAD_INFO);
    tr_debug("Thread Stack Info:");
    for (int i = 0; i < MAX_THREAD_INFO; i++) {
      if (stack_info[i].thread_id != 0) {
        for (int j = 0; j < MAX_THREAD_INFO; j++) {
          if (stack_info[i].thread_id == m_stack_info[j].thread_id) {
            int max_size_diff = stack_info[i].max_size - m_stack_info[j].max_size;
            if (max_size_diff > 0) {
              tr_debug("\tThread: %d", j);
              tr_debug("\t\tThread Id: 0x%08X with name %s", m_stack_info[j].thread_id, osThreadGetName((osThreadId_t) m_stack_info[j].thread_id));
              tr_debug("\t\tMaximum number of bytes used on the stack increased by %d to %d bytes (stack size is %d bytes)", max_size_diff, stack_info[i].max_size, stack_info[i].reserved_size);
            }
            m_stack_info[j] = stack_info[i];
          }
        }            
      }
    }    
  }
}

#endif

} // namespace
