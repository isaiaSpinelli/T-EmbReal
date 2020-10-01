#include "LCDDisplay.h"

namespace StaticSchedulingWithEvent {

#if defined(TARGET_DISCO_L475VG_IOT01A) 
    #define PUSH_BUTTON USER_BUTTON
    #define POLARITY_PRESSED 0
#elif defined(TARGET_EP_AGORA)
    #define PUSH_BUTTON PIN_NAME_PUSH_BUTTON
    #define POLARITY_PRESSED ACTIVE_LOW_POLARITY
#endif

const std::chrono::microseconds LCDDisplay::TASK_RUN_TIME = 300000us;


StaticSchedulingWithEvent::LCDDisplay::LCDDisplay(){

}

/*
void LCDDisplay::show(int currentGear, int currentRotationCount){

    printf("Current gear = %d\nCurrent rotation count = %d\n", currentGear,currentRotationCount);

}*/

void LCDDisplay::show(int currentGear, int currentRotationCount, int subTaskIndex){

    switch(subTaskIndex){
        case 1:
            wait_us( (TASK_RUN_TIME.count() / 3) );
            printf("LCD Display show :\n");
            break;
        case 2:
            wait_us( (TASK_RUN_TIME.count() / 3) );
            printf("\tCurrent gear = %d\n",currentGear);
            break;
        case 3:
            wait_us( (TASK_RUN_TIME.count() / 3) );
            printf("\tCurrent rotation count = %d\n",currentRotationCount);
            break ;


        default:
            wait_us(TASK_RUN_TIME.count());
            printf("\nError subTaskIndex = %d\n", subTaskIndex);
            break;
    }
}

} // namespace