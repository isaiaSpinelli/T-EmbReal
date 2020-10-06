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



} // namespace
