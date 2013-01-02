#pragma once

ZB_NAMESPACE_BEGIN
#ifndef ZB_DISABLE_PROFILER
void zb_profile_visualizer_draw(const Camera2d &camera);

#define ProfileVisualizerDraw(camera) ZB_MULTI_LINE_MACRO_BEGIN \
  zb_profile_visualizer_draw(camera); \
ZB_MULTI_LINE_MACRO_END

#else

#define ProfileVisualizerDraw(camera) ZB_MULTI_LINE_MACRO_BEGIN \
  banish(camera); \
ZB_MULTI_LINE_MACRO_END

#endif

ZB_NAMESPACE_END
