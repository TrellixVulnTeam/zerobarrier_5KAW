//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

#include <windows.h>

namespace zb_time {
  seconds frequency = 0;

  void Initialize(void) {
    LARGE_INTEGER largeFrequency;
    QueryPerformanceFrequency(&largeFrequency);
    frequency = (seconds)largeFrequency.QuadPart;
  }

  seconds TimeDifference(TimeStamp old, TimeStamp current) {
    return (seconds)(current - old) / frequency;
  }

  seconds TimeDifference(seconds old, seconds current) {
    return (current - old) / frequency;
  }

  TimeStamp Time(void) {
    LARGE_INTEGER current;
    QueryPerformanceCounter(&current);
    return current.QuadPart;
  }
}

ZB_NAMESPACE_END
