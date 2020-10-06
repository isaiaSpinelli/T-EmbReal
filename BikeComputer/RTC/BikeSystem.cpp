#include "BikeSystem.h"

#include <string>


namespace rtc {

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

    tr_info("Starting with RTC scheduler\n");
    std::chrono::microseconds startTime;

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
        // Recupère le temps avant le lancement de la tâche
        startTime = m_timer.elapsed_time();
        m_taskInformationTable.execute(taskIndex);
        // note that the task ready to run status is reset after execution
        char buffer [50];
        sprintf (buffer, "task %d",taskIndex);
        controleTimeExecution(startTime, m_timer, m_taskInformationTable.getPeriodTask_us(taskIndex).count()/1000, (char*)buffer);
        break;
      }
    }
  }

    /*
    
    
    Timer m_timer;
    std::chrono::microseconds startTime,taskEndTime,executionTime ;
    int timeExecuteMS=0;
    
    //mbed_trace_init();
    tr_info("Starting Super-Loop with no event");

    // start the timer
    m_timer.start();

    // Boucle infinie
    while(true){


        // Recupère le temps avant le lancement de la tâche
        startTime = m_timer.elapsed_time();

        // Execute la tâche en fonction de l'état présent
        switch(stateCycling){

            case gear:
            case gear_2:
                updateCurrentGear();
                controleTimeExecution(startTime, m_timer, 100, (char*)"gear");

                break;

            case count:
            case count_2:
            case count_3:
            case count_4:
                updateWheelRotationCount();
                controleTimeExecution(startTime, m_timer, 200, (char*)"count");

                break;
            
            case display_1:
                updateDisplay(1);
                controleTimeExecution(startTime, m_timer, 100, (char*)"display_1");

                break;

            case display_2:
                updateDisplay(2);
                controleTimeExecution(startTime, m_timer, 100, (char*)"display_2");
                break;

            case display_3:
                updateDisplay(3);
                controleTimeExecution(startTime, m_timer, 100, (char*)"display_3");

                break;

            case reset:
            case reset_2:
                checkAndPerformReset();
                controleTimeExecution(startTime, m_timer, 100, (char*)"reset");
                break;

            case wait:
                wait_us(100000);
                break;

        }

        // Passe à l'état suivant 
        int statePres = (1+(int)stateCycling) ;
        if (statePres > wait){
            statePres = 0;
        }

        stateCycling =  (STATE_CYCLING)statePres;

        
    }
    */

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



void BikeSystem::updateDisplay(int subTaskIndex){

    lcdDisplay.show( m_currentGear, m_currentWheelCounter, subTaskIndex);
}

void BikeSystem::updateDisplay(){

    lcdDisplay.show( m_currentGear, m_currentWheelCounter);
}



} // namespace
