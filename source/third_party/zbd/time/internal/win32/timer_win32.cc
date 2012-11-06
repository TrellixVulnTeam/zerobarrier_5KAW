//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

#include <windows.h>

LONGLONG RealTimeElapsed::preciseFrequency = 0;
seconds RealTimeElapsed::frequency = 0;

RealTimeElapsed::RealTimeElapsed(void) {
  last = 0;
}

void RealTimeElapsed::Initialize(void) {
  LARGE_INTEGER largeFrequency;
  QueryPerformanceFrequency(&largeFrequency);
  preciseFrequency = largeFrequency.QuadPart;
  frequency = static_cast<seconds>(preciseFrequency);
}

seconds RealTimeElapsed::StampDiff(TimeStamp old, TimeStamp current) {
  return static_cast<seconds>(current - old) / frequency;
}

seconds RealTimeElapsed::Elapsed(void) {
  LARGE_INTEGER current;
  QueryPerformanceCounter(&current);

  seconds dt = StampDiff(last, current.QuadPart);
  last = current.QuadPart;

  return dt;
}

seconds RealTimeElapsed::PeekElapsed(void) {
  LARGE_INTEGER current;
  QueryPerformanceCounter(&current);

  return static_cast<seconds>(current.QuadPart - last) / frequency;
}

TimeStamp RealTimeElapsed::CurrentStamp(void) {
  return last;
}

ZB_NAMESPACE_END
