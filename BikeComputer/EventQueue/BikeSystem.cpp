#include "BikeSystem.h"

#include <string>

#include "mbed_events.h"





namespace EventQueueNs {

BikeSystem::BikeSystem() :
    resetDevice(callback(this, &BikeSystem::setReset)), gearSystemDevice(callback(this, &BikeSystem::setGear)) {

}

void BikeSystem::setReset(){
    this->resetOn=true;
    // Start timer reset 
    timer_reset.start();
    startReset = timer_reset.elapsed_time();

#if defined(USE_SECOND_THREAD)
    // defer the job to the event queue
    queue_periodic.call(mbed::callback(this, &BikeSystem::checkAndPerformReset));
#else
    m_eventQueue.call(mbed::callback(this, &BikeSystem::checkAndPerformReset));
#endif

}

void BikeSystem::setGear(){
    this->gearOn=true;

#if defined(USE_SECOND_THREAD)
  // defer the job to the event queue if ap
  queue_periodic.call(mbed::callback(this, &BikeSystem::updateCurrentGear));
#else
  m_eventQueue.call(mbed::callback(this, &BikeSystem::updateCurrentGear));
#endif

}

void BikeSystem::checkAndPerformReset(){
    // Si un reset a été détecté (sans callback)
    // OU avec le callback
    // resetDevice.checkReset() => oblige l'attente d'execution de la tache reset
    if ( /* resetDevice.checkReset() ||*/ this->resetOn==true ){
        this->resetOn = false;
        
        wheelCounterDevice.reset();

        // end timer reset
        long long timeExecuteMS = (timer_reset.elapsed_time() - startReset).count()/1000;
        tr_info("\n\nReset system ! (response time = %lld [ms] )\n\n", timeExecuteMS);
    }
}

// Controle le temps d'exécution
void BikeSystem::controleTimeExecution(std::chrono::microseconds &startTime, Timer &timer, long long TimeTestMS, char* task ){

    long long timeExecuteMS = (timer.elapsed_time() - startTime).count()/1000;

    if (timeExecuteMS != TimeTestMS){
        tr_error("Error : Time execute of %s = %lld [ms] instead of %lld [ms]", task, timeExecuteMS, TimeTestMS );
    } else {
        tr_debug("Time execute of %s = %lld [ms]", task, timeExecuteMS );
    }   
    
}


void BikeSystem::start(){

    tr_info("Starting with EventQueue scheduler ");

        std::chrono::microseconds startTime;

        // start the timer
        m_timer.start();
        startTime = m_timer.elapsed_time();

    #ifdef USE_SECOND_THREAD
        tr_info("Using 2 thread !");
        Thread thread_no_P(osPriorityAboveNormal5) ;

        thread_no_P.start(callback(&queue_periodic, &EventQueue::dispatch_forever));

    #endif
  

    m_eventQueue.call_every(WheelCounterDevice::TASK_PERIOD, callback(this, &BikeSystem::updateWheelRotationCount));
    //m_eventQueue.call_every( WheelCounterDevice::TASK_PERIOD, this, &BikeSystem::updateWheelRotationCount );
    m_eventQueue.call_every( LCDDisplay::TASK_PERIOD, this, &BikeSystem::updateDisplay);


    m_eventQueue.call_every( 1s, this, &BikeSystem::getAndPrintStatistics);


    m_eventQueue.dispatch_forever();

}

void BikeSystem::updateCurrentGear(){
     if ( this->gearOn==true ){
        this->gearOn=false;
        m_currentGear = gearSystemDevice.getCurrentGear();
     }
    
}

void BikeSystem::updateWheelRotationCount(){
    m_currentWheelCounter = wheelCounterDevice.getCurrentRotationCount();
}


/* 
void BikeSystem::updateDisplay(int subTaskIndex){

    lcdDisplay.show( m_currentGear, m_currentWheelCounter, subTaskIndex);
}*/

void BikeSystem::updateDisplay(){

    lcdDisplay.show( m_currentGear, m_currentWheelCounter);
}

void BikeSystem::queueNoP(){

    events::EventQueue queue_no_periodic;

    queue_no_periodic.call_every( 100ms, this, &BikeSystem::updateCurrentGear );
    queue_no_periodic.call_every( 100ms, this, &BikeSystem::checkAndPerformReset );

    queue_no_periodic.dispatch();
    
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
