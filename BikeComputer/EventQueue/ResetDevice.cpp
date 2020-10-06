#include "ResetDevice.h"

namespace EventQueueNs {

#if defined(TARGET_DISCO_L475VG_IOT01A) 
    #define PUSH_BUTTON USER_BUTTON
    #define POLARITY_PRESSED 0
#elif defined(TARGET_EP_AGORA)
    #define PUSH_BUTTON PIN_NAME_PUSH_BUTTON
    #define POLARITY_PRESSED ACTIVE_LOW_POLARITY
#endif

const std::chrono::microseconds ResetDevice::TASK_RUN_TIME = 100000us;

//DigitalIn ResetDevice::buttonReset(PUSH_BUTTON);


ResetDevice::ResetDevice() :
    _interrupt(PUSH_BUTTON), cb(NULL){

    // rise ou fall
    _interrupt.rise(callback(this, &ResetDevice::setReset));
}

ResetDevice::ResetDevice(Callback<void()> callback) :
     _interrupt(PUSH_BUTTON), cb(callback) {

    // rise ou fall
    _interrupt.rise(cb);
}

void ResetDevice::setReset(){
    this->resetWithoutCallback = true;
}


bool ResetDevice::checkReset(){
    bool resetON=false;
    // simulate task computation by waiting for the required task run time
    wait_us(TASK_RUN_TIME.count());
    // return (buttonReset == POLARITY_PRESSED) ;

    // Si aucune fonction de callback a été fourni
    if (cb==NULL) {
        resetON = this->resetWithoutCallback;
        this->resetWithoutCallback = false;
    // Si un callback gère déjà la détéction
    } else {
        resetON = false;
    }
    return resetON ;
}

} // namespace