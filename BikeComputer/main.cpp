#include "mbed.h"

#include "mbed_trace.h"


#include "StaticScheduling/BikeSystem.h"
#include "StaticSchedulingWithEvent/BikeSystem.h"

int main() {
  mbed_trace_init();
  
  tr_info("Bike computer program started\n");

  StaticSchedulingWithEvent::BikeSystem bikeSystem;
  bikeSystem.start();
}