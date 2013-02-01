#pragma once

ZB_NAMESPACE_BEGIN
#ifndef ZB_DISABLE_PROFILER
void zb_profile_visualizer_draw(const aabb2d &viewport);

#define ProfileVisualizerDraw(viewport) ZB_MULTI_LINE_MACRO_BEGIN \
  zb_profile_visualizer_draw(viewport); \
ZB_MULTI_LINE_MACRO_END

#else

#define ProfileVisualizerDraw(viewport) ZB_MULTI_LINE_MACRO_BEGIN \
  banish(viewport); \
ZB_MULTI_LINE_MACRO_END

#endif

ZB_NAMESPACE_END
