#include "BikeSystem.h"
#include <string>

namespace static_scheduling {


BikeSystem::BikeSystem(){

}

// Controle le temps d'exécution
void BikeSystem::controleTimeExecution(std::chrono::microseconds &startTime, Timer &timer, long long TimeTestMS, char* task ){

    long long timeExecuteMS = (timer.elapsed_time() - startTime).count()/1000;

    if (timeExecuteMS != TimeTestMS){
        tr_error("Error : Time execute of %s = %lld", task, timeExecuteMS );
    }

    
}


void BikeSystem::start(){
    Timer m_timer;
    std::chrono::microseconds startTime,taskEndTime,executionTime ;
    int timeExecuteMS=0;
    
    //mbed_trace_init();
    tr_info("Starting Super-Loop with no event");

    // start the timer
    m_timer.start();

    // Boucle infinie
    while(true){


        tr_debug("STATE CYCLING  = %d\n", stateCycling);

        // Recupère le temps avant le lancement de la tâche
        startTime = m_timer.elapsed_time();

        // Execute la tâche en fonction de l'état présent
        switch(stateCycling){

            case gear:
            case gear_2:
                updateCurrentGear();
                controleTimeExecution(startTime, m_timer, 100, "gear");

                break;

            case count:
            case count_2:
            case count_3:
            case count_4:
                updateWheelRotationCount();
                controleTimeExecution(startTime, m_timer, 200, "count");

                break;
            
            case display_1:
                updateDisplay(1);
                controleTimeExecution(startTime, m_timer, 100, "display_1");

                break;

            case display_2:
                updateDisplay(2);
                controleTimeExecution(startTime, m_timer, 100, "display_2");
                break;

            case display_3:
                updateDisplay(3);
                controleTimeExecution(startTime, m_timer, 100, "display_3");

                break;

            case reset:
            case reset_2:
                checkAndPerformReset();
                controleTimeExecution(startTime, m_timer, 100, "reset");
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

}

void BikeSystem::updateCurrentGear(){
    m_currentGear = gearSystemDevice.getCurrentGear();
}

void BikeSystem::updateWheelRotationCount(){
    m_currentWheelCounter = wheelCounterDevice.getCurrentRotationCount();
}

void BikeSystem::checkAndPerformReset(){
    if ( resetDevice.checkReset() ){
        tr_info("Reset system !\n");
        wheelCounterDevice.reset();
    }
}

void BikeSystem::updateDisplay(int subTaskIndex){

    lcdDisplay.show( m_currentGear, m_currentWheelCounter, subTaskIndex);
}





} // namespace