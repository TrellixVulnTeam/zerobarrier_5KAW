#pragma once

ZB_NAMESPACE_BEGIN

#ifndef ZB_DISABLE_PROFILER
const i32 ZB_PROFILE_FRAME_HISTORY_LENGTH = 300;

struct zb_profile_data {
  zb_profile_data(void)
  : frameStart(0)
  , frameNumber(0)
  , totalHistoryTime(0.0f) {
    // Do nothing.
  }

  struct Sample {
    TimeStamp time;
    const char *name;
    const char *function;
    const char *file;
    i32 line;
  };

  struct Statistics {
    Statistics(void)
    : lastFrameTime(0)
    , frameTotal(0.0f)
    , historyTotal(0.0f)
    , historyPercentage(0.0f) {
      memset(frameHistory, 0, sizeof(frameHistory));
    }

    TimeStamp lastFrameTime;
    f32 frameTotal;
    f32 historyTotal;
    f32 historyPercentage;

    // Cumulative frame time, based off of 'name'.
    f32 frameHistory[ZB_PROFILE_FRAME_HISTORY_LENGTH];
  };

  struct AsciiStringCompare {
     bool operator()(char const *a, char const *b) {
        return zbstrcmp(a, b) < 0;
     }
  };
  typedef zbmap(const char*, Statistics*, AsciiStringCompare) StatisticsMap;
  typedef zbset(const char*, AsciiStringCompare) StringStorage;

  TimeStamp frameStart;
  u32 frameNumber;
  f32 totalHistoryTime;
  zbvector(Sample) frameSamples;
  StatisticsMap statistics;
  zbvector(Sample) sampleHistory[ZB_PROFILE_FRAME_HISTORY_LENGTH];
  StringStorage strings;
};

void zb_profile_frame_start(void);
ZB_INLINE void zb_profile_start(const char *name, const char *function, const char *file, i32 line);
ZB_INLINE void zb_profile_start_store_strings(const char *name);
ZB_INLINE void zb_profile_stop(void);
void zb_profile_shutdown(void);

// Automatically profiles the current scope.
struct zb_profile_scope {
  zb_profile_scope(const char *name, const char *function, const char *file, i32 line) { zb_profile_start(name, function, file, line); }
  ~zb_profile_scope(void) { zb_profile_stop(); }
};

#define ProfileFrameStart() ZB_MULTI_LINE_MACRO_BEGIN \
  zb_profile_frame_start(); \
ZB_MULTI_LINE_MACRO_END

#define ProfileStart(name) ZB_MULTI_LINE_MACRO_BEGIN \
  zb_profile_start(name, ZB_FUNCTION_NAME, ZB_FILE_NAME, ZB_LINE_NUMBER); \
ZB_MULTI_LINE_MACRO_END

#define ProfileStartStoreStrings(name) ZB_MULTI_LINE_MACRO_BEGIN \
  zb_profile_start_store_strings(name); \
ZB_MULTI_LINE_MACRO_END

#define ProfileStop() ZB_MULTI_LINE_MACRO_BEGIN \
  zb_profile_stop(); \
ZB_MULTI_LINE_MACRO_END

#define ProfileScope(name) const zb_profile_scope zbProfileScope##name(name, ZB_FUNCTION_NAME, ZB_FILE_NAME, ZB_LINE_NUMBER)

#define ProfileShutdown() ZB_MULTI_LINE_MACRO_BEGIN \
  zb_profile_shutdown(); \
ZB_MULTI_LINE_MACRO_END

#else

#define ProfileFrameStart() ZB_MULTI_LINE_MACRO_BEGIN \
ZB_MULTI_LINE_MACRO_END

#define ProfileStart(name) ZB_MULTI_LINE_MACRO_BEGIN \
  banish(name);
ZB_MULTI_LINE_MACRO_END

#define ProfileStartStoreStrings(name) ZB_MULTI_LINE_MACRO_BEGIN \
ZB_MULTI_LINE_MACRO_END

#define ProfileStop() ZB_MULTI_LINE_MACRO_BEGIN \
ZB_MULTI_LINE_MACRO_END

#define ProfileScope(name) ZB_MULTI_LINE_MACRO_BEGIN \
  banish(name);
ZB_MULTI_LINE_MACRO_END

#define ProfileShutdown() ZB_MULTI_LINE_MACRO_BEGIN \
ZB_MULTI_LINE_MACRO_END

#endif

ZB_NAMESPACE_END
