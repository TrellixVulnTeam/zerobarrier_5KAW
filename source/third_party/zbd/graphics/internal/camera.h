#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

class Camera {
public:
  Camera(void);

  void Initialize(f32 width, f32 height, f32 minDepth, f32 maxDepth);

  vector3d GetLocation(void) const;

  void SetLocation(const vector3d &location);
  void SetLocation(const vector2d &location);
  void SetOrientation(const complex &orientation);
  void SetZoom(const f32 zoom);

  void Move(const vector3d &delta);
  void Rotate(const complex &rotation);
  void Zoom(const f32 zoom_delta);

  f32 GetWidth(void) const;
  f32 GetHeight(void) const;
  f32 GetZoom(void) const;

  void OrthoProjection(void);
  void FinalizeMatrices(void);

  D3DXMATRIX View;
  D3DXMATRIX Projection;
  D3DXMATRIX ViewInverse;
  D3DXMATRIX ProjectionInverse;

private:
  vector3d location;
  complex orientation;
  f32 width;
  f32 height;
  f32 min_depth;
  f32 max_depth;
  f32 zoom;
  bool view_dirty;
  bool projection_dirty;
};

ZB_NAMESPACE_END
