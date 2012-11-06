//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

DebugRenderer *ZB_DEBUG_RENDERER = 0x0;
bool DebugRenderer::enabled = true;

namespace {
  struct DebugVertex {
    vector2d position;
    D3DCOLOR color;
  };

  const char *DebugEffectSource =
    "float4x4 ViewProjection;"
    ""
    "struct vertex_in"
    "{"
    "  float2 position : POSITION;"
    "  float4 color    : COLOR0;"
    "};"
    ""
    "struct vertex_out"
    "{"
    "  float4 position	: POSITION;"
    "  float4 color     : COLOR0;"
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
    ""
    "  output.color = input.color;"
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
    "technique DrawLines"
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

void DebugRenderer::Tick(void) {
  const seconds dt = realtimer.Elapsed();

  for (i32 i = debug_lines.size() - 1; i >= 0; --i) {
    DebugLine &line = debug_lines[i];
    // Test before decrementing so that everything gets drawn at least once.
    if (line.remaining < 0.0f) {
      debug_lines[i] = debug_lines.back();
      debug_lines.pop_back();
    }

    line.remaining -= dt;
  }
}

void DebugRenderer::ToggleDrawing(void) {
  enabled = !enabled;
}

void DebugRenderer::Render(void) {
  if (debug_lines.size() + debug_lines_draw_once.size() == 0 || !enabled) {
    return;
  }

  UpdateBuffers();

  device->SetVertexDeclaration(debug_vertex_declaration);
  device->SetStreamSource(0, debug_line_buffer, 0, sizeof(DebugVertex));

  D3DXMATRIX view_projection;
  D3DXMatrixMultiply(&view_projection, &view, &projection);
  debug_effect->SetMatrix("ViewProjection", &view_projection);

  u32 passes = 0;
  debug_effect->Begin(&passes, 0);
  for (u32 iPass = 0; iPass < passes; ++iPass) {
    debug_effect->BeginPass(iPass);
    device->DrawPrimitive(D3DPT_LINELIST, 0, debug_lines.size() + debug_lines_draw_once.size());
    debug_effect->EndPass();
  }
  debug_effect->End();

  device->SetVertexDeclaration(0x0);
  device->SetStreamSource(0, 0x0, 0, 0);

  debug_lines_draw_once.resize(0);
}

void DebugRenderer::UpdateBuffers(void) {
  u32 lineCount = debug_lines.size() + debug_lines_draw_once.size();

  if (debug_line_buffer_count < lineCount) {
    if (debug_line_buffer != 0x0) {
      debug_line_buffer->Release();
      debug_line_buffer = 0x0;
    }

    device->CreateVertexBuffer(sizeof(DebugVertex)*2*lineCount, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &debug_line_buffer, 0x0);
    debug_line_buffer_count = lineCount;
  }

  if (debug_line_buffer != 0x0) {
    DebugVertex *vertices = 0x0;
    debug_line_buffer->Lock(0, 0, (void**)&vertices, D3DLOCK_DISCARD);

    u32 lineIndex = 0;
    DebugLineList* lists[2] = {&debug_lines, &debug_lines_draw_once};
    
    for (i32 i = 0; i < 2; ++i) {
      const DebugLineList &lines = *lists[i];
      const DebugLineList::const_iterator start = lines.begin();
      const DebugLineList::const_iterator end = lines.end();

      for (DebugLineList::const_iterator iLine = start; iLine != end; ++iLine) {
        const DebugLine &line = *iLine;
        const D3DCOLOR color = D3DXCOLOR(line.color.r, line.color.g, line.color.b, line.color.a);
        vertices[lineIndex + 0].position = line.start;
        vertices[lineIndex + 0].color = color;
        vertices[lineIndex + 1].position = line.end;
        vertices[lineIndex + 1].color = color;

        lineIndex += 2;
      }
    }
    
    debug_line_buffer->Unlock();
  }
}

void DebugRenderer::Line(const vector2d &start, const vector2d &end, const Color &color, seconds time) {
  if (ZB_DEBUG_RENDERER == 0x0) {
    return;
  }

  DebugLine newLine;
  newLine.start = start;
  newLine.end = end;
  newLine.color = color;
  newLine.remaining = time;

  if (time == 0.0f) {
    ZB_DEBUG_RENDERER->debug_lines_draw_once.push_back(newLine);
  }
  else {
    ZB_DEBUG_RENDERER->debug_lines.push_back(newLine);
  }
}

void DebugRenderer::Arrow(const vector2d &start, const vector2d &end, const Color &color, seconds time, f32 length, degrees arrowAngle) {
  if (ZB_DEBUG_RENDERER == 0x0) {
    return;
  }

  const vector2d base = (end - start).Normal();
  const complex baseRotation(base.x, base.y);
  const complex arrowRotation = ComplexFromRadians((180.0f - arrowAngle) * ToRadians);
  const complex finalRotation = baseRotation * arrowRotation;
  const vector2d firstTip = vector2d(length, 0.0f) * finalRotation;
  const vector2d firstTipPoint = end + firstTip;
  const vector2d secondTipPoint = firstTipPoint + (dot(firstTip, base)*base - firstTip) * 2.0f;

  Line(start, end, color, time);
  Line(end, firstTipPoint, color, time);
  Line(end, secondTipPoint, color, time);
}

void DebugRenderer::Box(const aabb2d &aabb, const Color &color, seconds time) {
  if (ZB_DEBUG_RENDERER == 0x0) {
    return;
  }

  const vector2d tr(aabb.center.x + aabb.extents.x, aabb.center.y + aabb.extents.y);
  const vector2d bl(aabb.center.x - aabb.extents.x, aabb.center.y - aabb.extents.y);
  const vector2d tl(aabb.center.x - aabb.extents.x, aabb.center.y + aabb.extents.y);
  const vector2d br(aabb.center.x + aabb.extents.x, aabb.center.y - aabb.extents.y);

  Box(tl, tr, bl, br, color, time);
}

void DebugRenderer::Box(const vector2d &tl, const vector2d &tr, const vector2d &bl, const vector2d &br, const Color &color, seconds time) {
  if (ZB_DEBUG_RENDERER == 0x0) {
    return;
  }

  Line(tl, tr, color, time);
  Line(tr, br, color, time);
  Line(br, bl, color, time);
  Line(bl, tl, color, time);
}

void DebugRenderer::Triangle(const vector2d &a, const vector2d &b, const vector2d &c, const Color &color, seconds time) {
  if (ZB_DEBUG_RENDERER == 0x0) {
    return;
  }

  Line(a, b, color, time);
  Line(b, c, color, time);
  Line(c, a, color, time);
}

void DebugRenderer::Ngon(const vector2d &center, f32 radius, u32 n, const Color &color,  seconds time, radians orientation) {
  defend (n > 2);

  if (ZB_DEBUG_RENDERER == 0x0) {
    return;
  }

  const complex toOrientation = ComplexFromRadians(orientation);
  const f32 toNextPoint = TwoPi / static_cast<f32>(n);
  vector2d pointOffset(radius, 0.0f);
  vector2d start = pointOffset * toOrientation + center;
  for (u32 i = 1; i < n + 1; ++i) {
    const complex final = ComplexFromRadians(toNextPoint * i + orientation);
    const vector2d end = pointOffset * final + center;
    Line(start, end, color, time);
    start = end;
  }
}


DebugRenderer::DebugRenderer(void) 
: device(0x0)
, debug_vertex_declaration(0x0)
, debug_line_buffer(0x0)
, debug_line_buffer_count(0)
, debug_effect(0x0) {
  D3DXMatrixIdentity(&view);
  D3DXMatrixIdentity(&projection);
}

bool DebugRenderer::Initialize(IDirect3DDevice9 *device) {
  defend (ZB_DEBUG_RENDERER == 0x0);
  defend (device != 0x0);

  this->device = device;

  D3DVERTEXELEMENT9 debugVertexElements[] = 
  {
    {0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
    {0, 8, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
    D3DDECL_END()
  };

  if (FAILED(device->CreateVertexDeclaration(debugVertexElements, &debug_vertex_declaration))) {
    return false;
  }

  ID3DXBuffer *errors = 0x0;
  D3DXCreateEffect(device, DebugEffectSource, strlen(DebugEffectSource), 0x0, 0x0, 0, 0x0, &debug_effect, &errors);
  if (debug_effect == 0x0) {
    if (errors != 0x0) {
      //char *message = (char*)errors->GetBufferPointer();
      //defend_msg (errors == 0x0, message);
    }

    return false;
  }

  realtimer.Elapsed();

  ZB_DEBUG_RENDERER = this;
  return true;
}

void DebugRenderer::Shutdown(void) {
  debug_lines.clear();
  
  if (debug_effect) {
    debug_effect->Release();
  }

  if (debug_vertex_declaration) {
    debug_vertex_declaration->Release();
  }
}

void DebugRenderer::SetCamera(const D3DXMATRIX &view, const D3DXMATRIX &projection) {
  this->view = view;
  this->projection = projection;
}

ZB_NAMESPACE_END
