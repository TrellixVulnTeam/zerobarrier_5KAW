#pragma once

//
// See copyright notice in zbd.h.
//

#include "../zbd.h"

#include "internal/types.h"

#if defined(ZB_PLATFORM_WINDOWS)
typedef __int64 LONGLONG;
typedef LONGLONG TimeStamp;
#else
#error Timer class not implemented for this platform.
#endif

ZB_NAMESPACE_BEGIN

namespace zb_time {
  void Initialize(void);
  seconds TimeDifference(TimeStamp old, TimeStamp current);
  seconds TimeDifference(seconds old, seconds current);
  TimeStamp Time(void);
}

ZB_NAMESPACE_END