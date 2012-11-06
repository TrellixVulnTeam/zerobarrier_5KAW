#pragma once

//
// See copyright notice in zbd.h.
//

typedef __int64 LONGLONG;
typedef LONGLONG TimeStamp;

ZB_NAMESPACE_BEGIN

class RealTimeElapsed {
public:
  static void Initialize(void);
  static seconds StampDiff(TimeStamp old, TimeStamp current);

  RealTimeElapsed(void);

  seconds Elapsed(void);
  seconds PeekElapsed(void);

  TimeStamp CurrentStamp(void);
  
private:
  static LONGLONG preciseFrequency;
  static seconds frequency;

  LONGLONG last;
};

ZB_NAMESPACE_END
