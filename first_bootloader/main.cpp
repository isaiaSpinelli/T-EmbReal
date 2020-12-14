#include "mbed.h"

#include "mbed_trace.h"

#include "UpdateClient/FlashUpdater.h"
#include "UpdateClient/MbedApplication.h"
#include "UpdateClient/CandidateApplications.h"

#define TRACE_GROUP "main"

#if MBED_CONF_MBED_TRACE_ENABLE
static UnbufferedSerial g_uart(STDIO_UART_TX, STDIO_UART_RX);

// Function that directly outputs to an unbuffered serial port in blocking mode.
void boot_debug(const char *s) {
  size_t len = strlen(s);
  g_uart.write(s, len);
}
#endif

int main() {
#if MBED_CONF_MBED_TRACE_ENABLE
  mbed_trace_init();
  mbed_trace_print_function_set(boot_debug);
#endif // MBED_CONF_MBED_TRACE_ENABLE

  tr_debug("BikeComputer bootloader 5\r\n");

     
    //----- GET ACTIVE APP -----
  update_client::FlashUpdater flashUpdater; //= update_client::FlashUpdater();
  flashUpdater.init();

  update_client::MbedApplication mbedApplicationActive(
      flashUpdater, HEADER_ADDR, POST_APPLICATION_ADDR);

   mbedApplicationActive.checkApplication();   


    //----- GET CANDIDATE APP -----
    const uint32_t headerSize = APPLICATION_ADDR - HEADER_ADDR;
    update_client::CandidateApplications candidateApplications(
      flashUpdater, MBED_CONF_UPDATE_CLIENT_STORAGE_ADDRESS, 
                    MBED_CONF_UPDATE_CLIENT_STORAGE_SIZE, 
                    headerSize, // HEADER_SIZE, 
                    MBED_CONF_UPDATE_CLIENT_STORAGE_LOCATIONS);

    uint32_t slotIndex ;
    bool ret = candidateApplications.hasValidNewerApplication(mbedApplicationActive, slotIndex );

    tr_debug("Valid new app = %d\r\n", ret);

    // a firmware candidate with a newer version is availabl
    if (ret){
        uint32_t applicationAddress;
        uint32_t slotSize;
        candidateApplications.getApplicationAddress( slotIndex, applicationAddress,slotSize );
        candidateApplications.installApplication(slotIndex, POST_APPLICATION_ADDR );

    }


    // if valide, lauch active application
  if (mbedApplicationActive.isValid()){
     tr_debug(" Active application is valid\r\n");

    // at this stage we directly branch to the main application
    void *sp = *((void **)POST_APPLICATION_ADDR + 0);
    void *pc = *((void **)POST_APPLICATION_ADDR + 1);

    tr_debug("Starting application at address 0x%08x (sp 0x%08x, pc 0x%08x)\r\n",
            POST_APPLICATION_ADDR, (uint32_t)sp, (uint32_t)pc);

    mbed_start_application(POST_APPLICATION_ADDR);


    //  if the active application is not valid
  } else {
      // if a firmware candidate is stored and valid
        // install it
  }
  
   tr_debug(" Active application isn't valid\r\n");


  
}
