#include "ResetDevice.h"

#if defined(TARGET_DISCO_L475VG_IOT01A) 
#define PUSH_BUTTON USER_BUTTON
#define POLARITY_PRESSED 0
#elif defined(TARGET_EP_AGORA)
#define PUSH_BUTTON PIN_NAME_PUSH_BUTTON
#define POLARITY_PRESSED ACTIVE_LOW_POLARITY
#endif

namespace with_multitasking {
  
ResetDevice::ResetDevice(mbed::Callback<void()> cb) :
  m_resetButton(PUSH_BUTTON) {
  m_resetButton.fall(cb);
}

} // namespace
