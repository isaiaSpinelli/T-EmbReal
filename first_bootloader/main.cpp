#include "mbed.h"

#include "mbed_trace.h"

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

 tr_debug("BikeComputer bootloader\r\n");

 
 
  // at this stage we directly branch to the main application  
  void* sp = *((void **) POST_APPLICATION_ADDR + 0);
  void* pc = *((void **) POST_APPLICATION_ADDR + 1);

  tr_debug("Starting application at address 0x%08x (sp 0x%08x, pc 0x%08x)\r\n", POST_APPLICATION_ADDR, (uint32_t) sp, (uint32_t) pc);
 
  mbed_start_application(POST_APPLICATION_ADDR);
}

