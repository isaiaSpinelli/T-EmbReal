#include "ButtonDevice.h"

#if defined(TARGET_DISCO_L475VG_IOT01A) 
#define PUSH_BUTTON USER_BUTTON
#define POLARITY_PRESSED 0
#elif defined(TARGET_EP_AGORA)
#define PUSH_BUTTON PIN_NAME_PUSH_BUTTON
#define POLARITY_PRESSED ACTIVE_LOW_POLARITY
#endif

namespace with_multitasking {
  
ButtonDevice::ButtonDevice(mbed::Callback<void()> cb_fall, mbed::Callback<void()> cb_rise) :
  m_resetButton(PUSH_BUTTON) {
  m_resetButton.fall(cb_fall);
  m_resetButton.rise(cb_rise);
}

} // namespace
