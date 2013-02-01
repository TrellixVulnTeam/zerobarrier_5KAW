
ZB_NAMESPACE_BEGIN

#ifndef ZB_DISABLE_PROFILER
// TODO: Make this thread-safe, TLS perhaps?
zb_profile_data zbProfileData;

void zb_profile_frame_start(void) {
  TimeStamp frameStart = zbProfileData.frameStart;

  // Time the profiler itself.
  TimeStamp profilerBeginTime = zb_time::Time();

  // Run through the samples and tally up frame time.
  zbvector(zb_profile_data::RawSample) &frameRawSamples = zbProfileData.frameRawSamples;
  zbvector(const zb_profile_data::RawSample*) activeRawSamples;
  zbvector(zb_profile_data::Sample*) activeSamples;

  for (size_t i = 0; i < frameRawSamples.size(); ++i) {
    const zb_profile_data::RawSample &rawSample = frameRawSamples[i];
    if (rawSample.name != 0x0) {
      // This is the start of a sample, push it onto the sample stack.
      activeRawSamples.push_back(&rawSample);

      zb_profile_data::Sample *&sample = zbProfileData.samples[rawSample];
      if (sample == 0x0) {
        sample = znew("zb_profile_data::Sample") zb_profile_data::Sample(rawSample);
      }

      if (!activeSamples.empty()) {
        activeSamples.back()->children.insert(sample);
        activeSamples.push_back(sample);
      }
      else {
        activeSamples.push_back(sample);
      }
    }
    else {
      // This is the end of a sample, compute the dt and update statistics.
      defend_msg (!activeRawSamples.empty(), "Profile start/stop mismatch.");
      const zb_profile_data::RawSample &start = *activeRawSamples.back();
      activeRawSamples.pop_back();

      zb_profile_data::Sample *sample = activeSamples.back();
      activeSamples.pop_back();

      defend_msg (start.name != 0x0, "Profile start/stop mismatch.");

      // Reset the counter if this is a new frame.
      if (sample->lastFrameTime != frameStart) {
        sample->lastFrameTime = frameStart;
        sample->inclusiveFrameTotal = 0.0f;
      }

      f32 length = (f32)zb_time::TimeDifference(start.time, rawSample.time);
      sample->inclusiveFrameTotal += length;
    }
  }
  defend_msg (activeRawSamples.empty(), "Profile start/stop mismatch.");

  // Update histories.
  const i32 frameIndex = (i32)(zbProfileData.frameNumber % ZB_PROFILE_FRAME_HISTORY_LENGTH);
  const i32 validEntryCount = (i32)(zbProfileData.frameNumber < ZB_PROFILE_FRAME_HISTORY_LENGTH ? zbProfileData.frameNumber + 1 : ZB_PROFILE_FRAME_HISTORY_LENGTH);
  ++zbProfileData.frameNumber;

  zb_profile_data::SampleMap::iterator iStart = zbProfileData.samples.begin();
  zb_profile_data::SampleMap::iterator iStop = zbProfileData.samples.end();
  for (zb_profile_data::SampleMap::iterator iStats = iStart; iStats != iStop; ++iStats) {
    zb_profile_data::Sample *&sample = iStats->second;
    f32 *history = sample->inclusiveFrameHistory;

    // Reset the counter if this is a new frame.
    if (sample->lastFrameTime != frameStart) {
      sample->inclusiveFrameTotal = 0.0f;
    }

    history[frameIndex] = sample->inclusiveFrameTotal;

    f32 historyTotal = 0.0f;
    for (i32 i = 0; i < validEntryCount; ++i) {
      historyTotal += history[i];
    }

    sample->inclusiveHistoryTotal = historyTotal;
  }

  f32 totalHistoryTime = 0.0f;
  for (zb_profile_data::SampleMap::iterator iStats = iStart; iStats != iStop; ++iStats) {
    zb_profile_data::Sample *&sample = iStats->second;
    sample->exclusiveFrameTotal = sample->inclusiveFrameTotal;
    sample->exclusiveHistoryTotal = sample->inclusiveHistoryTotal;

    zbset(zb_profile_data::Sample*)::iterator iChildStart = sample->children.begin();
    zbset(zb_profile_data::Sample*)::iterator iChildEnd = sample->children.end();
    for (zbset(zb_profile_data::Sample*)::iterator iChild = iChildStart; iChild != iChildEnd; ++iChild) {
      zb_profile_data::Sample *child = *iChild;
      sample->exclusiveFrameTotal -= child->inclusiveFrameTotal;
      sample->exclusiveHistoryTotal -= child->inclusiveHistoryTotal;
    }

    // Total time is the sum of all exclusive time.
    totalHistoryTime += sample->exclusiveHistoryTotal;
  }

  zbProfileData.rawSampleHistory[frameIndex].swap(zbProfileData.frameRawSamples);
  zbProfileData.totalHistoryTime = totalHistoryTime;
  zbProfileData.frameRawSamples.clear();
  zbProfileData.frameStart = zb_time::Time();

  // Inject the profiler's time at the beginning of the frame.
  zb_profile_start("ZBProfile", ZB_FUNCTION_NAME, ZB_FILE_NAME, ZB_LINE_NUMBER);
  zbProfileData.frameRawSamples[0].time = profilerBeginTime;
  zb_profile_stop();
}

void zb_profile_start(const char *name, const char *function, const char *file, i32 line) {
  defend (name != 0x0);

  // Keep the possible allocation out of the timing.
  zbProfileData.frameRawSamples.push_back(zb_profile_data::RawSample());
  zb_profile_data::RawSample &s = zbProfileData.frameRawSamples.back();
  s.time = zb_time::Time();
  s.name = name;
  s.function = function;
  s.file = file;
  s.line = line;
}

void zb_profile_start_store_strings(const char *name, const char *function, const char *file, i32 line) {
  defend (name != 0x0);

  zb_profile_data::StringStorage::iterator iString = zbProfileData.strings.find(name);
  if (iString == zbProfileData.strings.end()) {
    i32 length = strlen(name);
    char *storedName = znew("zb_profile_data::string") char[length + 1];
    memcpy(storedName, name, length);
    storedName[length] = 0x0;

    zbProfileData.strings.insert(storedName);
    name = storedName;
  }
  else {
    name = *iString;
  }

  iString = zbProfileData.strings.find(function);
  if (iString == zbProfileData.strings.end()) {
    i32 length = strlen(function);
    char *storedName = znew("zb_profile_data::string") char[length + 1];
    memcpy(storedName, function, length);
    storedName[length] = 0x0;

    zbProfileData.strings.insert(storedName);
    function = storedName;
  }
  else {
    function = *iString;
  }

  iString = zbProfileData.strings.find(file);
  if (iString == zbProfileData.strings.end()) {
    i32 length = strlen(file);
    char *storedName = znew("zb_profile_data::string") char[length + 1];
    memcpy(storedName, file, length);
    storedName[length] = 0x0;

    zbProfileData.strings.insert(storedName);
    file = storedName;
  }
  else {
    file = *iString;
  }

  zb_profile_start(name, function, file, line);
}

void zb_profile_stop(void) {
  zb_profile_data::RawSample s;
  s.time = zb_time::Time();
  s.name = 0x0;
  s.function = 0x0;
  s.file = 0x0;
  s.line = -1;
  zbProfileData.frameRawSamples.push_back(s);
}

void zb_profile_shutdown(void) {
  zbProfileData.frameRawSamples.clear();

  zb_profile_data::SampleMap::iterator iStatsStart = zbProfileData.samples.begin();
  zb_profile_data::SampleMap::iterator iStatsStop = zbProfileData.samples.end();
  for (zb_profile_data::SampleMap::iterator iStats = iStatsStart; iStats != iStatsStop; ++iStats) {
    zdelete(iStats->second);
  }
  zbProfileData.samples.clear();

  for (int i = 0; i < ZB_PROFILE_FRAME_HISTORY_LENGTH; ++i) {
    zbProfileData.rawSampleHistory[i].clear();
  }

  zb_profile_data::StringStorage::iterator iStringsStart = zbProfileData.strings.begin();
  zb_profile_data::StringStorage::iterator iStringsStop = zbProfileData.strings.end();
  for (zb_profile_data::StringStorage::iterator iStrings = iStringsStart; iStrings != iStringsStop; ++iStrings) {
    zarray_delete((char*)*iStrings);  // Cast hacks around the fact that the pointer is being stored as const.
  }
}

#endif

ZB_NAMESPACE_END
