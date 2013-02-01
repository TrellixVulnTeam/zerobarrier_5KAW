//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

RenderThread *RENDERTHREAD;

namespace RenderCommand {
  enum RenderCommand {
    Unknown,
    Clear,
    SetCamera,
    SetTexture,
    SetTechnique,
    Draw,
    Count,
  };
}

namespace ResourceCommand {
  enum ResourceCommand {
    Unknown,
    LoadTexture,
    LoadTextureFromFile,
    UnloadTexture
  };
}

void RenderThread::Initialize(const ViewParameters &viewParams) {
  viewParameters = viewParams;

  D3DPRESENT_PARAMETERS presentParameters = {0};
  presentParameters.BackBufferWidth = viewParameters.width;
  presentParameters.BackBufferHeight = viewParameters.height;
  presentParameters.BackBufferCount = 1;
  presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
  presentParameters.hDeviceWindow = viewParameters.view;
  presentParameters.EnableAutoDepthStencil = FALSE;
  presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

  if (viewParameters.windowed) {
    presentParameters.Windowed = TRUE;
    presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
  }
  else {
    presentParameters.Windowed = FALSE;
    presentParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
    presentParameters.FullScreen_RefreshRateInHz = 60;
  }

  d3dParameters = presentParameters;
  initialize();

  mutex = CreateMutex(0x0, FALSE, 0x0);

  // Jumpstart things a bit.
  vertexWriter->reserve(1000*sizeof(DefaultVertex));
  vertexReader->reserve(1000*sizeof(DefaultVertex));

  RENDERTHREAD = this;
}

void RenderThread::Shutdown(void) {
  RENDERTHREAD = 0x0;
  CloseHandle(mutex);
  shutdown();
}

void RenderThread::Clear(const aabb2d &region, const Color &color) {
  renderCommandWriter->write(RenderCommand::Clear);
  renderCommandWriter->write(region);
  renderCommandWriter->write(color);
}

void RenderThread::Clear(const Camera2d& camera, const Color& color) {
  Clear(camera.GetViewport(), color);
}

void RenderThread::SetCamera(const Camera2d &camera) {
  renderCommandWriter->write(RenderCommand::SetCamera);
  renderCommandWriter->write(camera);
}

void RenderThread::SetTexture(TextureHandle texture, i32 index) {
  renderCommandWriter->write(RenderCommand::SetTexture);
  renderCommandWriter->write(texture);
  renderCommandWriter->write(index);
}

void RenderThread::SetTechnique(i32 techniqueIndex) {
  renderCommandWriter->write(RenderCommand::SetTechnique);
  renderCommandWriter->write(techniqueIndex);
}

void RenderThread::Draw(PrimitiveType::PrimitiveType primitiveType, const zbvector(DefaultVertex) &verts) {
  Draw(primitiveType, &verts[0], verts.size());
}

void RenderThread::Draw(PrimitiveType::PrimitiveType primitiveType, const DefaultVertex *verts, i32 vertCount) {
  renderCommandWriter->write(RenderCommand::Draw);
  renderCommandWriter->write(primitiveType);

  i32 primitiveCount = 0;
  switch (primitiveType) {
    case PrimitiveType::Point:    primitiveCount = vertCount; break;
    case PrimitiveType::Line:     primitiveCount = vertCount / 2; break;
    case PrimitiveType::Triangle: primitiveCount = vertCount / 3; break;
    default: break;
  }

  renderCommandWriter->write(primitiveCount);

  u32 writeIndex = vertexWriter->size();
  if (vertexWriter->capacity() - writeIndex < vertCount * sizeof(DefaultVertex)) {
    vertexWriter->resize(vertexWriter->capacity() * 2);
  }

  vertexWriter->insert(vertexWriter->end(), vertCount * sizeof(DefaultVertex), 0);
  memcpy(&(*vertexWriter)[writeIndex], verts, vertCount * sizeof(DefaultVertex));
}

void RenderThread::Present(void) {
  ProfileStart("GFX::VSYNC");
  // Wait for commands from the previous frame to finish processing.
  lockThread();
  while (hasCommands) {
    unlockThread();
    Sleep(0);
    lockThread();
  }
  ProfileStop();

  // Swap in the new commands.
  defend (!hasCommands);
  zbswap(renderCommandReader, renderCommandWriter);
  zbswap(resourceCommandReader, resourceCommandWriter);
  zbswap(vertexReader, vertexWriter);

  // Reset the write state for this thread.
  renderCommandWriter->reset();
  resourceCommandWriter->freeMemory();  // Free memory because this will likely be largish, and infrequently used.
  vertexWriter->clear();

  hasCommands = true;
  unlockThread();
}

i32 RenderThread::WindowWidth(void) const {
  return viewParameters.width;
}

i32 RenderThread::WindowHeight(void) const {
  return viewParameters.height;
}

TextureHandle RenderThread::LoadTexture(const void *data, i32 bytes) {
  TextureHandle handle = nextTextureHandle++;

  resourceCommandWriter->write(ResourceCommand::LoadTexture);
  resourceCommandWriter->write(handle);
  resourceCommandWriter->writeArray(data, bytes);

  return handle;
}

TextureHandle RenderThread::LoadTextureFromFile(const std::string &filename) {
  TextureHandle handle = nextTextureHandle++;

  resourceCommandWriter->write(ResourceCommand::LoadTextureFromFile);
  resourceCommandWriter->write(handle);
  resourceCommandWriter->writeString(filename.c_str());

  return handle;
}

void RenderThread::UnloadTexture(TextureHandle texture) {
  resourceCommandWriter->write(ResourceCommand::UnloadTexture);
  resourceCommandWriter->write(texture);
}

void RenderThread::initialize(void) {
  if (d3d == 0x0) {
    d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (d3d != 0x0) {
      HRESULT result = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, viewParameters.view, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &d3dParameters, &device);
      if (FAILED(result)) {
        result = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, viewParameters.view, D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &d3dParameters, &device);
        if (FAILED(result)) {
          result = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, viewParameters.view, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &d3dParameters, &device);
        }
      }

      defend (!FAILED(result));
    }
  }

  setDefaultState();
  createDefaultResources();
}

void RenderThread::setDefaultState(void) {
  //device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
  device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  // TODO: Pre-multiplied alpha.
  device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
  device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
  device->SetRenderState(D3DRS_ZENABLE, FALSE);
}

void RenderThread::createDefaultResources(void) {
  // TODO: Resource commands for effects and vertex declarations.
  D3DVERTEXELEMENT9 defaultVertexElements[] = {
    {0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
    {0, 8, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
    {0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
    {0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
    D3DDECL_END()
  };

  device->CreateVertexDeclaration(defaultVertexElements, &defaultVertexDeclaration);

  ID3DXBuffer *errors = 0x0;
  D3DXCreateEffect(device, DefaultResources::DefaultEffect, strlen(DefaultResources::DefaultEffect), 0x0, 0x0, 0, 0x0, &defaultEffect, &errors);
  if (defaultEffect == 0x0) {
    if (errors != 0x0) {
      char *message = (char*)errors->GetBufferPointer();
      defend_msg (errors == 0x0, message);
    }
  }

  currentEffect = defaultEffect;
}

u32 RenderThread::main(void) {
  while (ThreadContext::main()) {
    lockThread();
    bool sleep = !hasCommands; // Have to copy this variable here since it's shared.
    if (hasCommands) {
      processResourceCommands();
      processRenderCommands();
      hasCommands = false;
    }
    unlockThread();

    // If there's nothing to draw, sleep.
    // This may need to be ripped out in the future, but it keeps
    // the application from burning through the CPU.
    if (sleep) {
      Sleep(1);
    }
  }

  return 0;
}

void RenderThread::shutdown(void) {
  // Bit of a hack, swap buffers and process all
  // commands to make sure all the texture unloads
  // etc. happen.
  Present();
  processResourceCommands();
  processRenderCommands();

  defend (textures.size() == 0);

  if (defaultVertexDeclaration) {
    defaultVertexDeclaration->Release();
    defaultVertexDeclaration = 0x0;
  }

  if (device) {
    device->Release();
    device = 0x0;
  }

  if (d3d) {
    d3d->Release();
    d3d = 0x0;
  }
}

void RenderThread::processResourceCommands(void) {
  while (!resourceCommandReader->reachedEnd()) {
    const ResourceCommand::ResourceCommand command = *resourceCommandReader->read<ResourceCommand::ResourceCommand>();

    switch (command) {
      case ResourceCommand::LoadTexture:
      {
        const TextureHandle handle = *resourceCommandReader->read<TextureHandle>();
        i32 size = 0;
        const void *data = resourceCommandReader->readArray(&size);
        loadTexture(handle, data, size);
        break;
      }

      case ResourceCommand::LoadTextureFromFile:
      {
        const TextureHandle handle = *resourceCommandReader->read<TextureHandle>();
        const char *filename = resourceCommandReader->readString();
        loadTextureFromFile(handle, filename);
        break;
      }

      case ResourceCommand::UnloadTexture:
      {
        unloadTexture(*resourceCommandReader->read<TextureHandle>());
        break;
      }

      default: sentinel(); break;
    }
  }
}

void RenderThread::processRenderCommands(void) {
  if (!renderCommandReader->reachedEnd()) {
    IDirect3DVertexBuffer9 *vertices = 0x0;
    if (!vertexReader->empty()) {
      device->CreateVertexBuffer(vertexReader->size(), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &vertices, 0x0);

      void *vertBuffer = 0x0;
      vertices->Lock(0, 0, &vertBuffer, D3DLOCK_DISCARD);
      memcpy(vertBuffer, &(*vertexReader)[0], vertexReader->size());
      vertices->Unlock();

      device->SetVertexDeclaration(defaultVertexDeclaration);
      device->SetStreamSource(0, vertices, 0, sizeof(DefaultVertex));
    }

    device->BeginScene();

    u32 effectPasses = 0;
    currentEffect->Begin(&effectPasses, D3DXFX_DONOTSAVESTATE);
    currentEffect->BeginPass(0);

    i32 startVertex = 0;
    PrimitiveType::PrimitiveType primitiveType = PrimitiveType::Point;
    i32 primitiveCount = 0;

    while (!renderCommandReader->reachedEnd()) {
      const RenderCommand::RenderCommand command = *renderCommandReader->read<RenderCommand::RenderCommand>();

      switch (command) {
        case RenderCommand::Clear:
        {
          flushBatch(primitiveType, &primitiveCount, &startVertex, effectPasses);

          const aabb2d region = *renderCommandReader->read<aabb2d>();
          const Color color = *renderCommandReader->read<Color>();
          clear(region, color);
          break;
        }

        case RenderCommand::SetCamera:
        {
          flushBatch(primitiveType, &primitiveCount, &startVertex, effectPasses);
          setCamera(*renderCommandReader->read<Camera2d>());
          break;
        }

        case RenderCommand::SetTexture:
        {
          TextureHandle handle = *renderCommandReader->read<TextureHandle>();
          i32 index = *renderCommandReader->read<i32>();
          defend (index < sizeof(currentTextures) / sizeof(currentTextures[0]));

          zbmap(TextureHandle, Texture)::iterator iTexture = textures.find(handle);
          if (iTexture != textures.end()) {
            if (currentTextures[index].texture != iTexture->second.texture) {
              flushBatch(primitiveType, &primitiveCount, &startVertex, effectPasses);
              setTexture(iTexture->second, index);
            }
          }

          break;
        }

        case RenderCommand::SetTechnique:
        {
          i32 techniqueIndex = *renderCommandReader->read<i32>();
          if (currentTechniqueIndex != techniqueIndex) {
            flushBatch(primitiveType, &primitiveCount, &startVertex, effectPasses);

            currentEffect->EndPass();
            currentEffect->End();
            setTechnique(techniqueIndex);
            currentEffect->Begin(&effectPasses, D3DXFX_DONOTSAVESTATE);
            currentEffect->BeginPass(0);
          }

          break;
        }

        case RenderCommand::Draw:
        {
          PrimitiveType::PrimitiveType newPrimitiveType = *renderCommandReader->read<PrimitiveType::PrimitiveType>();
          const i32 count = *renderCommandReader->read<i32>();

          if (newPrimitiveType != primitiveType) {
            flushBatch(primitiveType, &primitiveCount, &startVertex, effectPasses);
          }

          primitiveType = newPrimitiveType;
          primitiveCount += count;
          break;
        }

        default: sentinel(); break;
      }
    }

    flushBatch(primitiveType, &primitiveCount, &startVertex, effectPasses);

    currentEffect->EndPass();
    currentEffect->End();

    device->SetVertexDeclaration(0x0);
    device->SetStreamSource(0, 0x0, 0, 0);

    device->EndScene();
    device->Present(0x0, 0x0, 0, 0x0);

    if (vertices) {
      vertices->Release();
      vertices = 0x0;
    }
  }
}

void RenderThread::flushBatch(PrimitiveType::PrimitiveType primitiveType, i32 *primitiveCountInOut, i32 *startVertexInOut, i32 effectPasses) {
  defend(primitiveCountInOut != startVertexInOut);
  const i32 primitiveCount = *primitiveCountInOut;

  if (primitiveCount > 0) {
    i32 startVertex = *startVertexInOut;

    draw(primitiveType, primitiveCount, startVertex, effectPasses);
    switch (primitiveType) {
        case PrimitiveType::Point:    startVertex += primitiveCount * 1; break;
        case PrimitiveType::Line:     startVertex += primitiveCount * 2; break;
        case PrimitiveType::Triangle: startVertex += primitiveCount * 3; break;
    }

    *primitiveCountInOut = 0;
    *startVertexInOut = startVertex;
  }
}

void RenderThread::lockThread(void) {
  WaitForSingleObject(mutex, INFINITE);
}

void RenderThread::unlockThread(void) {
  ReleaseMutex(mutex);
}

void RenderThread::clear(const aabb2d &region, const Color &color) {
   const vector2d viewMin = region.min();
   const vector2d viewMax = region.max();
   D3DRECT rect = { (long)viewMin.x, (long)viewMin.y, (long)viewMax.x, (long)viewMax.y };
   device->Clear(1, &rect, D3DCLEAR_TARGET, D3DXCOLOR(color.r, color.g, color.b, color.a), 0.0f, 0);
}

void RenderThread::setCamera(const Camera2d &camera) {
  this->camera = camera;
  this->camera.FinalizeMatrices();

  D3DXMATRIX viewProjection;
  D3DXMatrixMultiply(&viewProjection, &this->camera.View, &this->camera.Projection);
  currentEffect->SetMatrix("ViewProjection", &viewProjection);

  vector2d halfPixelOffset = componentwise_divide(vector2d(-1.0f, 1.0f), camera.GetViewport().extents * 2.0f);
  currentEffect->SetFloatArray("HalfPixelOffset", (FLOAT*)&halfPixelOffset, 2);

  aabb2d viewport = camera.GetViewport();
  D3DVIEWPORT9 d3dViewport;
  d3dViewport.X = (DWORD)viewport.min().x;
  d3dViewport.Y = (DWORD)viewport.min().y;
  d3dViewport.Width = (DWORD)viewport.width();
  d3dViewport.Height = (DWORD)viewport.height();
  device->SetViewport(&d3dViewport);

  currentEffectHasChanges = true;
}

void RenderThread::setTexture(const Texture &texture, i32 index) {
    const char *texName = "";
    const char *virtualCoordsName = "";
    const char *texelOffsetName = "";
    switch (index) {
      case 0: texName = "Tex0"; texelOffsetName = "HalfTexelOffset0"; virtualCoordsName = "Tex0Scalar"; break;
      case 1: texName = "Tex1"; texelOffsetName = "HalfTexelOffset1"; virtualCoordsName = "Tex1Scalar"; break;
      default: sentinel(); break;
    }
    currentEffect->SetTexture(texName, texture.texture);

    D3DSURFACE_DESC desc;
    texture.texture->GetLevelDesc(0, &desc);
    vector2d halfTexelOffset = componentwise_divide(vector2d(0.5f, 0.5f), vector2d((f32)desc.Width, (f32)desc.Height));
    currentEffect->SetFloatArray(texelOffsetName, (FLOAT*)&halfTexelOffset, 2);

    currentEffect->SetFloatArray(virtualCoordsName, (FLOAT*)&texture.virtualCoords, 2);
    currentTextures[index] = texture;

    currentEffectHasChanges = true;
}

void RenderThread::setTechnique(i32 techniqueIndex) {
  currentEffect->SetTechnique(currentEffect->GetTechnique(techniqueIndex));
  currentTechniqueIndex = techniqueIndex;
  currentEffectHasChanges = true;
}

void RenderThread::draw(PrimitiveType::PrimitiveType primitiveType, i32 count, i32 startVertex, i32 passes) {
  D3DPRIMITIVETYPE d3dPrimitiveType = D3DPT_POINTLIST;
  switch (primitiveType) {
    case PrimitiveType::Point:        d3dPrimitiveType = D3DPT_POINTLIST; break;
    case PrimitiveType::Line:         d3dPrimitiveType = D3DPT_LINELIST; break;
    case PrimitiveType::Triangle:     d3dPrimitiveType = D3DPT_TRIANGLELIST; break;
  }

  if (currentEffectHasChanges) {
    currentEffect->CommitChanges();
    currentEffectHasChanges = false;
  }

  for (i32 iPass = 0; iPass < passes; ++iPass) {
    // Since most effects only have 1 pass, we set that upfront when the effect is initially set
    // instead of every time a batch is drawn.
    // Otherwise, this loop will catch it, do the rest of the passes, and reset it to pass 0.
    // This helps prevent unnecessary state changes.
    if (iPass) {
      currentEffect->EndPass();
      currentEffect->BeginPass(iPass);
    }

    device->DrawPrimitive(d3dPrimitiveType, startVertex, count);
  }

  if (passes > 1) {
    currentEffect->EndPass();
    currentEffect->BeginPass(0);
  }
}

void RenderThread::loadTexture(TextureHandle handle, const void *data, i32 bytes) {
  IDirect3DTexture9 *d3dTexture = 0x0;
  D3DXIMAGE_INFO info;
  D3DXCreateTextureFromFileInMemoryEx(device, data, bytes, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, &info, 0x0, &d3dTexture);
  if (d3dTexture) {
    addD3DTexture(d3dTexture, &info, handle);
  }
}

void RenderThread::loadTextureFromFile(TextureHandle handle, const char *filename) {
  IDirect3DTexture9 *d3dTexture = 0x0;
  D3DXIMAGE_INFO info;
  D3DXCreateTextureFromFileEx(device, zb_widen(filename).c_str(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, &info, 0x0, &d3dTexture);
  if (d3dTexture) {
    addD3DTexture(d3dTexture, &info, handle);
  }
}

void RenderThread::addD3DTexture(IDirect3DTexture9 *d3dTexture, const D3DXIMAGE_INFO *info, TextureHandle handle) {
  defend(d3dTexture);

  D3DSURFACE_DESC desc;
  d3dTexture->GetLevelDesc(0, &desc);

  Texture texture;
  texture.texture = d3dTexture;
  texture.width = (f32)info->Width;
  texture.height = (f32)info->Height;
  texture.virtualCoords = vector2d(texture.width / desc.Width, texture.height / desc.Height);

  MapInsertUnique(textures, handle, texture);
}

void RenderThread::unloadTexture(TextureHandle handle) {
  zbmap(TextureHandle, Texture)::iterator iTexture = textures.find(handle);
  if (iTexture != textures.end()) {
    iTexture->second.texture->Release();
    textures.erase(iTexture);
  }
}


namespace Draw {
  void Point(const vector2d &a, const Color &color) {
    DefaultVertex vert;

    vert.position = a;
    vert.color = D3DXCOLOR(color.r, color.g, color.b, color.a);

    RENDERTHREAD->Draw(PrimitiveType::Point, &vert, 1);
  }

  void Line(const vector2d &a, const vector2d &b, const Color &color) {
    DefaultVertex verts[2];

    verts[0].position = a;
    verts[0].color = D3DXCOLOR(color.r, color.g, color.b, color.a);

    verts[1].position = b;
    verts[1].color = D3DXCOLOR(color.r, color.g, color.b, color.a);

    RENDERTHREAD->Draw(PrimitiveType::Line, verts, 2);
  }

  void ThickLine(const vector2d &a, const vector2d &b, f32 thickness, const Color &color) {
    vector2d points[4];

    vector2d direction = a - b;
    vector2d push = vector2d(-direction.y, direction.x).Normal() * thickness / 2.0f;

    points[0] = a + push;
    points[1] = a - push;
    points[2] = b + push;
    points[3] = b - push;

    Triangle(points[0], points[1], points[2], color, true);
    Triangle(points[1], points[2], points[3], color, true);
  }

  void Arrow(const vector2d &start, const vector2d &end, const Color &color, f32 arrowLength, degrees arrowAngle) {
    const vector2d base = (end - start).Normal();
    const complex baseRotation(base.x, base.y);
    const complex arrowRotation = ComplexFromRadians((180.0f - arrowAngle) * ToRadians);
    const complex finalRotation = baseRotation * arrowRotation;
    const vector2d firstTip = vector2d(arrowLength, 0.0f) * finalRotation;
    const vector2d firstTipPoint = end + firstTip;
    const vector2d secondTipPoint = firstTipPoint + (dot(firstTip, base)*base - firstTip) * 2.0f;

    Line(start, end, color);
    Line(end, firstTipPoint, color);
    Line(end, secondTipPoint, color);
  }

  void Triangle(vector2d a, vector2d b, vector2d c, const Color &color, bool fill) {
    if (fill) {
      DefaultVertex verts[3];

      if (wedge(b-a, c-a) > 0.0f) {
        zbswap(a, b);
      }

      verts[0].position = a;
      verts[0].color = D3DXCOLOR(color.r, color.g, color.b, color.a);

      verts[1].position = b;
      verts[1].color = D3DXCOLOR(color.r, color.g, color.b, color.a);

      verts[2].position = c;
      verts[2].color = D3DXCOLOR(color.r, color.g, color.b, color.a);
      RENDERTHREAD->Draw(PrimitiveType::Triangle, verts, 3);
    }
    else {
      Line(a, b, color);
      Line(a, c, color);
      Line(b, c, color);
    }
  }

  void Rect(const aabb2d &rect, const Color &color, bool fill) {
    const vector2d a = rect.min();
    const vector2d b = a + vector2d(rect.width(), 0.0f);
    const vector2d c = a + vector2d(0.0f, rect.height());
    const vector2d d = rect.max();

    if (fill) {
      Triangle(a, b, c, color, true);
      Triangle(b, d, c, color, true);
    }
    else {
      Line(a, b, color);
      Line(a, c, color);
      Line(b, d, color);
      Line(c, d, color);
    }
  }

  void Ngon(const vector2d &center, f32 radius, i32 n, const Color &color, radians orientation, bool fill) {
    defend (n > 2);

    ProfileStart("GFX::Ngon");
    const complex toOrientation = ComplexFromRadians(orientation);
    const f32 toNextPoint = TwoPi / f32(n);
    vector2d pointOffset(radius, 0.0f);
    vector2d start = pointOffset * toOrientation + center;
    for (i32 i = 1; i < n + 1; ++i) {
      const complex final = ComplexFromRadians(toNextPoint * i + orientation);
      const vector2d end = pointOffset * final + center;
      if (fill) {
        Triangle(center, start, end, color, true);
      }
      else {
        Line(start, end, color);
      }

      start = end;
    }
    ProfileStop();
  }

  void TexturedQuad(const transform2d &transform, const aabb2d &tex0, const Color &color, const aabb2d &tex1) {
    ProfileStart("GFX::TexturedQuad");
    const u32 d3dColor = D3DXCOLOR(color.r, color.g, color.b, color.a);

    DefaultVertex verts[6];
    verts[0].position = transform.TransformPoint(vector2d(-0.5f, 0.5f));
    verts[0].color = d3dColor;
    verts[0].tex0 = tex0.min();
    verts[0].tex1 = tex1.min();

    verts[1].position = transform.TransformPoint(vector2d(0.5f, 0.5f));
    verts[1].color = d3dColor;
    verts[1].tex0 = verts[0].tex0 + vector2d(tex0.width(), 0.0f);
    verts[1].tex1 = verts[0].tex1 + vector2d(tex1.width(), 0.0f);

    verts[2].position = transform.TransformPoint(vector2d(-0.5f, -0.5f));
    verts[2].color = d3dColor;
    verts[2].tex0 = verts[0].tex0 + vector2d(0.0f, tex0.height());
    verts[2].tex1 = verts[0].tex1 + vector2d(0.0f, tex1.height());

    verts[3] = verts[1];

    verts[4].position = transform.TransformPoint(vector2d(0.5f, -0.5f));
    verts[4].color = d3dColor;
    verts[4].tex0 = tex0.max();
    verts[4].tex1 = tex1.max();

    verts[5] = verts[2];

    RENDERTHREAD->Draw(PrimitiveType::Triangle, verts, 6);
    ProfileStop();
  }
}

ZB_NAMESPACE_END
