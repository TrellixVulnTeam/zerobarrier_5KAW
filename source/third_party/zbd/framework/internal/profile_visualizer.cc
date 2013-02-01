
ZB_NAMESPACE_BEGIN
#ifndef ZB_DISABLE_PROFILER

namespace ProfileVisualizer {
  bool StatSort(const zb_profile_data::Sample *a, const zb_profile_data::Sample *b) {
    return a->exclusiveHistoryTotal > b->exclusiveHistoryTotal;
  }

  Color NameToColor(const char *name) {
    static Color palette[] = {
      0x002b36ff,
      0x073642ff,
      0x586e75ff,
      0x657b83ff,
      0x839496ff,
      0x93a1a1ff,
      0xeee8d5ff,
      0xfdf6e3ff,
      0xb58900ff,
      0xcb4b16ff,
      0xdc322fff,
      0xd33682ff,
      0x6c71c4ff,
      0x268bd2ff,
      0x2aa198ff,
      0x859900ff
    };

    return palette[zb_hash_string(name) % (sizeof(palette) / sizeof(palette[0]))];
  }
}

void zb_profile_visualizer_draw(const aabb2d &viewport) {
  const f32 width = viewport.width();
  const f32 height = viewport.height();

  Camera2d camera;
  camera.Initialize(viewport, 0.0f, 32.0f);
  camera.SetLocation(vector3d(width / 2.0f, height / 2.0f, 32));
  RENDERTHREAD->SetCamera(camera);

  const f32 profileBarWidth = width * 0.8f;
  const f32 profileBarHeight = height * 0.01f;

  extern zb_profile_data zbProfileData;
  zbvector(zb_profile_data::Sample*) stats;
  stats.reserve(zbProfileData.samples.size());
  for (zb_profile_data::SampleMap::iterator iStat = zbProfileData.samples.begin(); iStat != zbProfileData.samples.end(); ++iStat) {
    stats.push_back(iStat->second);
  }

  std::sort(stats.begin(), stats.end(), ProfileVisualizer::StatSort);

  RENDERTHREAD->SetTechnique(0);
  const f32 profileBarHalfHeight = profileBarHeight / 2.0f;
  const f32 profileBarTopPadding = profileBarHeight;
  vector2d profileBarOffset((width - profileBarWidth) / 2.0f, height - profileBarTopPadding - profileBarHalfHeight);
  for (size_t i = 0; i < stats.size(); ++i) {
    zb_profile_data::Sample *sample = stats[i];
    const f32 statPercentage = sample->exclusiveHistoryTotal / zbProfileData.totalHistoryTime;
    const f32 statHalfWidth = statPercentage * profileBarWidth / 2.0f;
    profileBarOffset.x += statHalfWidth;

    Color color = ProfileVisualizer::NameToColor(sample->rawSample.name);
    Draw::Rect(aabb2d(profileBarOffset, vector2d(statHalfWidth, profileBarHalfHeight)), color, true);
    Draw::Rect(aabb2d(profileBarOffset, vector2d(statHalfWidth, profileBarHalfHeight)), Color::Black, false);

    profileBarOffset.x += statHalfWidth;
  }
}

#endif
ZB_NAMESPACE_END
