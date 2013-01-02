
ZB_NAMESPACE_BEGIN

#ifndef ZB_DISABLE_PROFILER
// TODO: Make this thread-safe, TLS perhaps?
zb_profile_data zbProfileData;

void zb_profile_frame_start(void) {
  TimeStamp frameStart = zbProfileData.frameStart;

  // Time the profiler itself.
  TimeStamp profilerBeginTime = zb_time::Time();

  // Run through the samples and tally up frame time.
  zbvector(zb_profile_data::Sample) &frameSamples = zbProfileData.frameSamples;
  zbvector(const zb_profile_data::Sample*) activeSamples;

  for (size_t i = 0; i < frameSamples.size(); ++i) {
    const zb_profile_data::Sample &sample = frameSamples[i];
    if (sample.name != 0x0) {
      // This is the start of a sample, push it onto the sample stack.
      activeSamples.push_back(&sample);
    }
    else {
      // This is the end of a sample, compute the dt and update statistics.
      defend_msg (!activeSamples.empty(), "Profile start/stop mismatch.");
      const zb_profile_data::Sample &start = *activeSamples.back();
      activeSamples.pop_back();

      defend_msg (start.name != 0x0, "Profile start/stop mismatch.");
      zb_profile_data::Statistics *&stats = zbProfileData.statistics[start.name];

      if (stats == 0x0) {
        stats = znew("zb_profile_data::Statistics") zb_profile_data::Statistics();
      }

      // Reset the counter if this is a new frame.
      if (stats->lastFrameTime != frameStart) {
        stats->lastFrameTime = frameStart;
        stats->frameTotal = 0.0f;
      }

      stats->frameTotal += (f32)zb_time::TimeDifference(start.time, sample.time);
    }
  }
  defend_msg (activeSamples.empty(), "Profile start/stop mismatch.");

  // Update histories.
  const i32 frameIndex = (i32)(zbProfileData.frameNumber % ZB_PROFILE_FRAME_HISTORY_LENGTH);
  const i32 validEntryCount = (i32)(zbProfileData.frameNumber < ZB_PROFILE_FRAME_HISTORY_LENGTH ? zbProfileData.frameNumber + 1 : ZB_PROFILE_FRAME_HISTORY_LENGTH);
  ++zbProfileData.frameNumber;

  f32 statisticsTotal = 0.0f;
  zb_profile_data::StatisticsMap::iterator iStart = zbProfileData.statistics.begin();
  zb_profile_data::StatisticsMap::iterator iStop = zbProfileData.statistics.end();
  for (zb_profile_data::StatisticsMap::iterator iStats = iStart; iStats != iStop; ++iStats) {
    zb_profile_data::Statistics *&stats = iStats->second;
    f32 *history = stats->frameHistory;

    // Reset the counter if this is a new frame.
    if (stats->lastFrameTime != frameStart) {
      stats->frameTotal = 0.0f;
    }

    history[frameIndex] = stats->frameTotal;

    f32 historyTotal = 0.0f;
    for (i32 i = 0; i < validEntryCount; ++i) {
      historyTotal += history[i];
    }

    stats->historyTotal = historyTotal;
    statisticsTotal += historyTotal;
  }

  zbProfileData.sampleHistory[frameIndex].swap(zbProfileData.frameSamples);
  zbProfileData.totalHistoryTime = statisticsTotal;
  zbProfileData.frameSamples.clear();
  zbProfileData.frameStart = zb_time::Time();

  // Inject the profiler's time at the beginning of the frame.
  zb_profile_start("ZBProfile", ZB_FUNCTION_NAME, ZB_FILE_NAME, ZB_LINE_NUMBER);
  zbProfileData.frameSamples[0].time = profilerBeginTime;
  zb_profile_stop();
}

ZB_INLINE void zb_profile_start(const char *name, const char *function, const char *file, i32 line) {
  defend (name != 0x0);

  // Keep the possible allocation out of the timing.
  zbProfileData.frameSamples.push_back(zb_profile_data::Sample());
  zb_profile_data::Sample &s = zbProfileData.frameSamples.back();
  s.time = zb_time::Time();
  s.name = name;
  s.function = function;
  s.file = file;
  s.line = line;
}

ZB_INLINE void zb_profile_start_store_strings(const char *name) {
  defend (name != 0x0);

  zb_profile_data::StringStorage::iterator iString = zbProfileData.strings.find(name);
  if (iString == zbProfileData.strings.end()) {
    i32 length = strlen(name);
    char *storedName = znew("zb_profile_data::string") char[length + 1];
    memcpy(storedName, name, length);
    storedName[length] = 0x0;

    zbProfileData.strings.insert(storedName);
    iString = zbProfileData.strings.find(name);
  }

  zb_profile_start(*iString, 0x0, 0x0, -1);
}

ZB_INLINE void zb_profile_stop(void) {
  zb_profile_data::Sample s;
  s.time = zb_time::Time();
  s.name = 0x0;
  s.function = 0x0;
  s.file = 0x0;
  s.line = -1;
  zbProfileData.frameSamples.push_back(s);
}

void zb_profile_shutdown(void) {
  zb_profile_data::StatisticsMap::iterator iStatsStart = zbProfileData.statistics.begin();
  zb_profile_data::StatisticsMap::iterator iStatsStop = zbProfileData.statistics.end();
  for (zb_profile_data::StatisticsMap::iterator iStats = iStatsStart; iStats != iStatsStop; ++iStats) {
    zdelete(iStats->second);
  }
  zbProfileData.statistics.clear();

  for (int i = 0; i < ZB_PROFILE_FRAME_HISTORY_LENGTH; ++i) {
    zbProfileData.sampleHistory[i].clear();
  }

  zb_profile_data::StringStorage::iterator iStringsStart = zbProfileData.strings.begin();
  zb_profile_data::StringStorage::iterator iStringsStop = zbProfileData.strings.end();
  for (zb_profile_data::StringStorage::iterator iStrings = iStringsStart; iStrings != iStringsStop; ++iStrings) {
    zarray_delete((char*)*iStrings);  // Cast hacks around the fact that the pointer is being stored as const.
  }
}

#endif

ZB_NAMESPACE_END
