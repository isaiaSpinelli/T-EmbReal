#include "mbed.h"

#include "mbed_trace.h"

#include "StaticScheduling/BikeSystem.h"
#include "StaticSchedulingWithEvent/BikeSystem.h"
#include "RTC/BikeSystem.h" 
#include "WithEventQueue/BikeSystem.h"
#include "WithMultiTasking/BikeSystem.h"

#include "UpdateClient/USBSerialUC.h"


#define TRACE_GROUP "main"

int main() {
    mbed_trace_init();

    // Lancement du thread (màj depuis USB) en arrière plan 
    update_client::USBSerialUC usbSerialUC;
    Thread m_thread ;
    m_thread.start(callback(&usbSerialUC, &update_client::USBSerialUC::start))  ;

  

    // Lancement de l'application principal
    tr_info("\r\n----- Bike computer program started -----\r\n");

    with_multitasking::BikeSystem bikeSystem;
    bikeSystem.start();
}