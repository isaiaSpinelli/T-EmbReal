#include "mbed.h"

#include "mbed_trace.h"


#include "StaticScheduling/BikeSystem.h"
#include "StaticSchedulingWithEvent/BikeSystem.h"
#include "RTC/BikeSystem.h" 
#include "EventQueue/BikeSystem.h" 

int main() {
  mbed_trace_init();
  
  tr_info("Bike computer program started\n");

  EventQueueNs::BikeSystem bikeSystem;
  bikeSystem.start();
}