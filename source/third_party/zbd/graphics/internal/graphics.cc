//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

RenderThread *RENDERTHREAD;

namespace DefaultResources {
  const char *DefaultEffect =
  "float4x4 ViewProjection;"
  "float2 HalfPixelOffset;"
  ""
  "struct vertex_in"
  "{"
  "  float2 position : POSITION;"
  "  float4 color    : COLOR0;"
  "  float2 tex0     : TEXCOORD0;"
  "  float2 tex1     : TEXCOORD1;"
  "};"
  ""
  "struct vertex_out"
  "{"
  "  float4 position : POSITION;"
  "  float4 color    : COLOR0;"
  "  float2 tex0     : TEXCOORD0;"
  "  float2 tex1     : TEXCOORD1;"
  "};"
  ""
  "vertex_out process_vertex(vertex_in input)"
  "{"
  "  vertex_out output;"
  ""
  "  output.position.xy = input.position;"
  "  output.position.z = 0.0;"
  "  output.position.w = 1.0;"
  "  output.position = mul(output.position, ViewProjection);"
  "  output.position.xy += HalfPixelOffset;"
  ""
  "  output.color = input.color;"
  "  output.tex0 = input.tex0;"
  "  output.tex1 = input.tex1;"
  ""
  "  return output;"
  "}"
  ""
  "struct fragment_out"
  "{"
  "  float4 color : COLOR0;"
  "};"
  ""
  "fragment_out process_fragment(vertex_out input)"
  "{"
  "  fragment_out output;"
  "  output.color = input.color;"
  ""
  "  return output;"
  "}"
  ""
  "technique Default"
  "{"
  "  pass P0"
  "  {"
  "    AlphaBlendEnable = True;"
  "    DestBlend = InvSrcAlpha;"
  "    SrcBlend = SrcAlpha;"
  "    ZWriteEnable = False;"
  "    ZEnable = False;"
  "    VertexShader = compile vs_2_0 process_vertex();"
  "    PixelShader = compile ps_2_0 process_fragment();"
  "  }"
  "}";
}

namespace RenderCommand {
  enum RenderCommand {
    Unknown,
    Clear,
    SetCamera,
    Draw,
    Count,
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

  RENDERTHREAD = this;
}

void RenderThread::Shutdown(void) {
  RENDERTHREAD = 0x0;
  CloseHandle(mutex);
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

void RenderThread::Draw(PrimitiveType::PrimitiveType primitiveType, const zbvector(DefaultVertex) &verts) {
  Draw(primitiveType, &verts[0], verts.size());
}

void RenderThread::Draw(PrimitiveType::PrimitiveType primitiveType, const DefaultVertex *verts, i32 vertCount) {
  renderCommandWriter->write(RenderCommand::Draw);
  renderCommandWriter->write(primitiveType);

  u32 writeIndex = vertexWriter->size();
  vertexWriter->resize(writeIndex + vertCount * sizeof(DefaultVertex));
  memcpy(&(*vertexWriter)[writeIndex], verts, vertCount * sizeof(DefaultVertex));
}

void RenderThread::Present(void) {
  lockThread();
  while (!hasDrawn) {
    unlockThread();
    Sleep(0);
    lockThread();
  }

  defend (hasDrawn);
  zbswap(renderCommandReader, renderCommandWriter);
  zbswap(vertexReader, vertexWriter);
  
  renderCommandWriter->reset();
  vertexWriter->clear();

  hasDrawn = false;
  unlockThread();
}

i32 RenderThread::WindowWidth(void) const {
  return viewParameters.width;
}

i32 RenderThread::WindowHeight(void) const {
  return viewParameters.height;
}

void RenderThread::initialize(void) {
  if (d3d == 0x0) {
    d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (d3d != 0x0)
    {
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
}

void RenderThread::createDefaultResources(void) {
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
}

u32 RenderThread::main(void) {
  createDefaultResources();

  while (ThreadContext::main()) {
    bool sleep = false;
    lockThread();
    if (!hasDrawn) {
      processRenderCommands();
      hasDrawn = true;
      sleep = false;
    }
    else {
      sleep = true;
    }
    unlockThread();

    if (sleep) {
      Sleep(1);
    }
  }

  shutdown();
  return 0;
}

void RenderThread::shutdown(void) {
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

void RenderThread::processRenderCommands(void) {
  if (!renderCommandReader->reachedEnd() && !vertexReader->empty()) {
    IDirect3DVertexBuffer9 *vertices = 0x0;
    device->CreateVertexBuffer(vertexReader->size(), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &vertices, 0x0);
    
    void *vertBuffer = 0x0;
    vertices->Lock(0, 0, &vertBuffer, D3DLOCK_DISCARD);
    memcpy(vertBuffer, &(*vertexReader)[0], vertexReader->size());
    vertices->Unlock(); 

    device->SetVertexDeclaration(defaultVertexDeclaration);
    device->SetStreamSource(0, vertices, 0, sizeof(DefaultVertex));

    device->BeginScene();

    i32 startVertex = 0;
    PrimitiveType::PrimitiveType primitiveType = PrimitiveType::Point;
    i32 primitiveCount = 0;

    while (!renderCommandReader->reachedEnd()) {
      const RenderCommand::RenderCommand command = *renderCommandReader->read<RenderCommand::RenderCommand>();

      switch (command) {
        case RenderCommand::Clear:
        {
          flushBatch(primitiveType, &primitiveCount, &startVertex);

          const aabb2d region = *renderCommandReader->read<aabb2d>();
          const Color color = *renderCommandReader->read<Color>();
          clear(region, color);
          break;
        }

        case RenderCommand::SetCamera: 
        {
          flushBatch(primitiveType, &primitiveCount, &startVertex);
          setCamera(*renderCommandReader->read<Camera2d>());
          break;
        }
        
        case RenderCommand::Draw:
        {
          PrimitiveType::PrimitiveType newPrimitiveType = *renderCommandReader->read<PrimitiveType::PrimitiveType>();
          
          if (newPrimitiveType != primitiveType) {
            flushBatch(primitiveType, &primitiveCount, &startVertex);
          }
          
          primitiveType = newPrimitiveType;
          ++primitiveCount;
          break;
        }

        default: sentinel(); break;
      }
    }

    flushBatch(primitiveType, &primitiveCount, &startVertex);

    device->SetVertexDeclaration(0x0);
    device->SetStreamSource(0, 0x0, 0, 0);

    device->EndScene();
    device->Present(0x0, 0x0, 0, 0x0);

    vertices->Release();
    vertices = 0x0;
  }
}

void RenderThread::flushBatch(PrimitiveType::PrimitiveType primitiveType, i32 *primitiveCountInOut, i32 *startVertexInOut) {
  defend(primitiveCountInOut != startVertexInOut);
  const i32 primitiveCount = *primitiveCountInOut;

  if (primitiveCount > 0) {
    i32 startVertex = *startVertexInOut;
      
    draw(primitiveType, primitiveCount, startVertex);
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
  defaultEffect->SetMatrix("ViewProjection", &viewProjection);

  vector2d halfPixelOffset = componentwise_divide(vector2d(-1.0f, 1.0f), camera.GetViewport().extents * 2.0f);
  defaultEffect->SetFloatArray("HalfPixelOffset", (FLOAT*)&halfPixelOffset, 2);

  aabb2d viewport = camera.GetViewport();
  D3DVIEWPORT9 d3dViewport;
  d3dViewport.X = (DWORD)viewport.min().x;
  d3dViewport.Y = (DWORD)viewport.min().y;
  d3dViewport.Width = (DWORD)viewport.width();
  d3dViewport.Height = (DWORD)viewport.height();
  device->SetViewport(&d3dViewport);
}

void RenderThread::draw(PrimitiveType::PrimitiveType primitiveType, i32 count, i32 startVertex) {
  D3DPRIMITIVETYPE d3dPrimitiveType = D3DPT_POINTLIST;
  switch (primitiveType) {
    case PrimitiveType::Point:        d3dPrimitiveType = D3DPT_POINTLIST; break;
    case PrimitiveType::Line:         d3dPrimitiveType = D3DPT_LINELIST; break;
    case PrimitiveType::Triangle:     d3dPrimitiveType = D3DPT_TRIANGLELIST; break;
  }

  u32 passes = 0;
  defaultEffect->Begin(&passes, 0);
  for (u32 iPass = 0; iPass < passes; ++iPass) {
    defaultEffect->BeginPass(iPass);
    device->DrawPrimitive(d3dPrimitiveType, startVertex, count);
    defaultEffect->EndPass();
  }
  defaultEffect->End();
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
  }
}


ZB_NAMESPACE_END
