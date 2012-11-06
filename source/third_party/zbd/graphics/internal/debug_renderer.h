#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

class DebugRenderer {
public:
  DebugRenderer(void);

  bool Initialize(IDirect3DDevice9 *device);
  void Shutdown(void);

  void SetCamera(const D3DXMATRIX &view, const D3DXMATRIX &projection);

  void Tick(void);
  void Render(void);

  static void Line(const vector2d &start, const vector2d &end, const Color &color = Color::Black, seconds time = 0.0f);
  static void Arrow(const vector2d &start, const vector2d &end, const Color &color = Color::Black, seconds time = 0.0f, f32 length = 8.0f, degrees angle = 30.0f);
  static void Box(const aabb2d &aabb, const Color &color = Color::White, seconds time = 0.0f);
  static void Box(const vector2d &tl, const vector2d &tr, const vector2d &bl, const vector2d &br, const Color &color = Color::Black, seconds time = 0.0f);
  static void Triangle(const vector2d &a, const vector2d &b, const vector2d &c, const Color &color = Color::Black, seconds time = 0.0f);
  static void Ngon(const vector2d &center, f32 radius, u32 n, const Color &color = Color::Black, seconds time = 0.0f, radians orientation = RadPi2);

  static void ToggleDrawing(void);

private:
  static bool enabled;

  struct DebugLine {
    vector2d start;
    vector2d end;
    Color color;
    seconds remaining;
  };
  typedef zbvector(DebugLine) DebugLineList;

private:
  void UpdateBuffers(void);

  RealTimeElapsed realtimer;
  IDirect3DDevice9 *device;

  IDirect3DVertexDeclaration9 *debug_vertex_declaration;
  IDirect3DVertexBuffer9 *debug_line_buffer;
  size_t debug_line_buffer_count;
  ID3DXEffect *debug_effect;
  D3DXHANDLE view_projection_handle;

  DebugLineList debug_lines;
  DebugLineList debug_lines_draw_once;
  
  D3DXMATRIX view;
  D3DXMATRIX projection;
};

extern DebugRenderer *ZB_DEBUG_RENDERER;

ZB_NAMESPACE_END
