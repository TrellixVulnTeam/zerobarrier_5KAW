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

  struct RawSample {
    TimeStamp time;
    const char *name;
    const char *function;
    const char *file;
    i32 line;
  };

  struct Sample {
    Sample(const RawSample &raw)
    : rawSample(raw)
    , lastFrameTime(0)
    , inclusiveFrameTotal(0.0f)
    , exclusiveFrameTotal(0.0f)
    , inclusiveHistoryTotal(0.0f)
    , exclusiveHistoryTotal(0.0f) {
      memset(inclusiveFrameHistory, 0, sizeof(inclusiveFrameHistory));
    }

    RawSample rawSample;

    TimeStamp lastFrameTime;
    f32 inclusiveFrameTotal;
    f32 exclusiveFrameTotal;
    f32 inclusiveHistoryTotal;
    f32 exclusiveHistoryTotal;

    f32 inclusiveFrameHistory[ZB_PROFILE_FRAME_HISTORY_LENGTH];
    zbset(Sample*) children;
  };

  struct AsciiStringCompare {
    bool operator()(char const *a, char const *b) {
      return zbstrcmp(a, b) < 0;
    }
  };

  struct RawSampleCompare {
    bool operator()(const RawSample &a, const RawSample &b) {
      if (a.line == b.line) {
        if (a.name == b.name) {
          if (a.function == b.function) {
            return a.file < b.file;
          }
          return a.function < b.function;
        }
        return a.name < b.name;
      }
      return a.line < b.line;
    }
  };

  typedef zbmap(RawSample, Sample*, RawSampleCompare) SampleMap;
  typedef zbset(const char*, AsciiStringCompare) StringStorage;

  TimeStamp frameStart;
  u32 frameNumber;
  f32 totalHistoryTime;

  zbvector(zb_profile_data::RawSample) frameRawSamples;

  SampleMap samples;
  zbvector(RawSample) rawSampleHistory[ZB_PROFILE_FRAME_HISTORY_LENGTH];
  StringStorage strings;
};

void zb_profile_frame_start(void);
void zb_profile_start(const char *name, const char *function, const char *file, i32 line);
void zb_profile_start_store_strings(const char *name, const char *function, const char *file, i32 line);
void zb_profile_stop(void);
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
  zb_profile_start_store_strings(name, ZB_FUNCTION_NAME, ZB_FILE_NAME, ZB_LINE_NUMBER); \
ZB_MULTI_LINE_MACRO_END

#define ProfileStop() ZB_MULTI_LINE_MACRO_BEGIN \
  zb_profile_stop(); \
ZB_MULTI_LINE_MACRO_END

#define ProfileScope(name) const zb_profile_scope zbProfileScope(name, ZB_FUNCTION_NAME, ZB_FILE_NAME, ZB_LINE_NUMBER)

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
  banish(name);
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
