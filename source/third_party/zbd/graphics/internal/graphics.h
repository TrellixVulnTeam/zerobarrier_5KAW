#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

struct ViewParameters {
  ViewParameters(void)
    : view(0)
    , width(-1)
    , height(-1)
    , windowed(false) {
      // Do nothing.
  }

  HWND view;
  i32 width;
  i32 height;
  bool windowed;
};

class Graphics {
public:
  Graphics(void);

  bool Initialize(ViewParameters vp);
  void Shutdown(void);

  void Tick(void);

  vector2d WindowToWorld(vector2d position);

  IDirect3D9 *d3d;
  IDirect3DDevice9 *device;

  Camera camera;

#if !defined(ZB_DISABLE_DEBUG_RENDERER)
  DebugRenderer debug_renderer;
#endif

  RealTimeElapsed renderTimer;
  f32 half_width;
  f32 half_height;
};

ZB_NAMESPACE_END
