#include "BikeSystem.h"

#include <string>

#include "mbed_events.h"

#define USE_SECOND_THREAD 



namespace EventQueueNs {

BikeSystem::BikeSystem() :
    resetDevice(callback(this, &BikeSystem::setReset)), gearSystemDevice(callback(this, &BikeSystem::setGear)) {

}

void BikeSystem::setReset(){
    this->resetOn=true;
    // Start timer reset 
    timer_reset.start();
    startReset = timer_reset.elapsed_time();
    m_taskInformationTable.makeTaskReadyToRun(0);
}

void BikeSystem::setGear(){
    this->gearOn=true;
    m_taskInformationTable.makeTaskReadyToRun(1);
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

    tr_info("Starting with EventQueue scheduler\n");

    #ifdef USE_SECOND_THREAD

        events::EventQueue queue_periodic;

        Thread thread_no_P(osPriorityAboveNormal5) ; // osPriorityBelowNormal

        thread_no_P.start(callback(this, &BikeSystem::queueNoP ));
        // thread_no_P.set_priority(osPriorityBelowNormal2);

        queue_periodic.call_every( 100ms, this, &BikeSystem::updateWheelRotationCount );
        queue_periodic.call_every( 250ms, this, &BikeSystem::updateDisplay);

        queue_periodic.dispatch();


    #else

        std::chrono::microseconds startTime;

        // creates a queue with the default size
        events::EventQueue queue;

        // start the timer
        m_timer.start();
        startTime = m_timer.elapsed_time();


        queue.call_every( 100ms, this, &BikeSystem::updateWheelRotationCount );
        queue.call_every( 250ms, this, &BikeSystem::updateDisplay);

        //queue.event(this, &BikeSystem::updateCurrentGear);
        //queue.event(this, &BikeSystem::checkAndPerformReset);

        queue.call_every( 100ms, this, &BikeSystem::updateCurrentGear );
        queue.call_every( 100ms, this, &BikeSystem::checkAndPerformReset );
        

        queue.dispatch();
        
    #endif



   

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


} // namespace
