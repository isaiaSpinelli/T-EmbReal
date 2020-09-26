#include "mbed.h"

#include "mbed_trace.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "main"
#endif // MBED_CONF_MBED_TRACE_ENABLE

#include "StaticScheduling/BikeSystem.h"

int main() {
  mbed_trace_init();
  
  tr_info("Bike computer program started\n");

  static_scheduling::BikeSystem bikeSystem;
  bikeSystem.start();
}