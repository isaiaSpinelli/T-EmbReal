#include "ResetDevice.h"

namespace static_scheduling {

#if defined(TARGET_DISCO_L475VG_IOT01A) 
    #define PUSH_BUTTON USER_BUTTON
    #define POLARITY_PRESSED 0
#elif defined(TARGET_EP_AGORA)
    #define PUSH_BUTTON PIN_NAME_PUSH_BUTTON
    #define POLARITY_PRESSED ACTIVE_LOW_POLARITY
#endif

const std::chrono::microseconds ResetDevice::TASK_RUN_TIME = 100000us;

DigitalIn ResetDevice::buttonReset(PUSH_BUTTON);


ResetDevice::ResetDevice(){

}

ResetDevice::ResetDevice(Callback<void()> callback) :
    callback(callback) {

    
}

bool ResetDevice::checkReset(){

    // simulate task computation by waiting for the required task run time
    wait_us(TASK_RUN_TIME.count());
    return (buttonReset == POLARITY_PRESSED) ;
}



} // namespace