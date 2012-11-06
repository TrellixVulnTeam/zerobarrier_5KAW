//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

Graphics::Graphics(void) 
: d3d(0x0)
, device(0x0) {
  // Do nothing.
}

bool Graphics::Initialize(ViewParameters vp) {
  if (d3d != 0x0) {
    return false;
  }

  d3d = Direct3DCreate9(D3D_SDK_VERSION);
  if (d3d == 0x0) {
    return false;
  }

  D3DPRESENT_PARAMETERS present_parameters = {0};
  present_parameters.BackBufferWidth = vp.width;
  present_parameters.BackBufferHeight = vp.height;
  present_parameters.BackBufferCount = 1;
  present_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
  present_parameters.hDeviceWindow = vp.view;
  present_parameters.EnableAutoDepthStencil = FALSE;
  present_parameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

  if (vp.windowed) {
    present_parameters.Windowed = TRUE;
    present_parameters.BackBufferFormat = D3DFMT_UNKNOWN;
  }
  else {
    present_parameters.Windowed = FALSE;
    present_parameters.BackBufferFormat = D3DFMT_X8R8G8B8;
  }

  HRESULT result = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, vp.view, D3DCREATE_HARDWARE_VERTEXPROCESSING, &present_parameters, &device);
  if (FAILED(result)) {
    result = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, vp.view, D3DCREATE_MIXED_VERTEXPROCESSING, &present_parameters, &device);
    if (FAILED(result)) {
      result = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, vp.view, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &present_parameters, &device);
      if (FAILED(result)) {
        return false;
      }
    }
  }

  //if (!scene_renderer.Initialize(device)) {
  //  return false;
  //}

#if !defined(ZB_DISABLE_DEBUG_RENDERER)
  debug_renderer.Initialize(device);
#endif

  camera.Initialize(static_cast<f32>(vp.width), static_cast<f32>(vp.height), 0.0f, 32.0f);
  camera.SetLocation(vector3d(0.0f, 0.0f, 32.0f));

  half_width = vp.width * 0.5f;
  half_height = vp.height * 0.5f;

  return true;
}

void Graphics::Shutdown(void) {
#if !defined(ZB_DISABLE_DEBUG_RENDERER)
  debug_renderer.Shutdown();
#endif
}

void Graphics::Tick(void) {
  // Only need this if vsync is turned off.
  //f32 framerate = 1.0f / 60.0f;
  //if (renderTimer.PeekElapsed() < framerate) {
  //  return;
  //}
  
  device->Clear(0, 0x0, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 135, 206, 250), 0.0f, 0);

  camera.FinalizeMatrices();

  device->BeginScene();
#if !defined(ZB_DISABLE_DEBUG_RENDERER)
  debug_renderer.Tick();
  debug_renderer.SetCamera(camera.View, camera.Projection);
  debug_renderer.Render();
#endif
  device->EndScene();

  device->Present(0x0, 0x0, 0x0, 0x0);
  renderTimer.Elapsed();
}

vector2d Graphics::WindowToWorld(vector2d position) {
  D3DXVECTOR4 d3dPosition(position.x / half_width - 1.0f, position.y / -half_height + 1.0f, 0.0f, 1.0f);
  D3DXVec4Transform(&d3dPosition, &d3dPosition, &camera.ProjectionInverse);
  D3DXVec4Transform(&d3dPosition, &d3dPosition, &camera.ViewInverse);

  return vector2d(d3dPosition.x, d3dPosition.y);
}

ZB_NAMESPACE_END
