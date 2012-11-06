//
// See copyright notice in zbd.h.
//

#include "zb_time.h"

#if defined(ZB_PLATFORM_WINDOWS)
#include "internal/win32/timer_win32.cc"
#else
#error Timer class not implemented for this platform.
#endif
