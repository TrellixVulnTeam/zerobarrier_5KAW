#pragma once

//
// See copyright notice in zbd.h.
//

#include "../zbd.h"

#include "internal/types.h"

#if defined(ZB_PLATFORM_WINDOWS)
#include "internal/win32/timer_win32.h"
#else
#error Timer class not implemented for this platform.
#endif
