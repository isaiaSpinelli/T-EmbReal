#include "mbed.h"

#include "mbed_trace.h"


#include "StaticScheduling/BikeSystem.h"
#include "StaticSchedulingWithEvent/BikeSystem.h"
#include "RTC/BikeSystem.h" 
#include "EventQueue/BikeSystem.h" 


#include "UpdateClient/FlashUpdater.h"
#include "UpdateClient/MbedApplication.h"
#include "UpdateClient/USBSerialUC.h"

// "target.printf_lib": "minimal-printf", // .text -4k, -elf -9k, .bin -5k -> std 

//const char szMsg[] = "This is a test message"; // .text (main.o) 23 bytes
static const int size = 10; // rien ...
int randomArray[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; // .data (main.o) 40  bytes
int randomNumber ; // .bss (main.o) 4 bytes
const char* string12 = "hello world"; // .data (main.o) 4 bytes
const char* stringbss ; // .bss  (main.o) 4 bytes

int main() {
  mbed_trace_init();

/*
  update_client::FlashUpdater flashUpdater; //= update_client::FlashUpdater();
  flashUpdater.init();

  update_client::MbedApplication mbedApplicationActive(
      flashUpdater, HEADER_ADDR, APPLICATION_ADDR);

   mbedApplicationActive.checkApplication();   

  if (mbedApplicationActive.isValid()){
     tr_debug(" Active application is valid");
  } else {
     tr_debug(" Active application isn't valid");
  }*/


  tr_info(stringbss);
  for (int i = 0; i < size; i++) {
    //randomArray[i] = i*4;
    tr_info("This is a random number %d", randomArray[i]);
  }
  //randomNumber = rand();
  tr_info("This is a random number %d", randomNumber);

  update_client::USBSerialUC usbSerialUC;

    Thread m_thread ;
    // callback( usbSerialUC , &update_client::USBSerialUC::start
    m_thread.start(callback(&usbSerialUC, &update_client::USBSerialUC::start))  ;

  
  tr_info("Bike computer program started 7 \n");

  EventQueueNs::BikeSystem bikeSystem;
  bikeSystem.start();
}