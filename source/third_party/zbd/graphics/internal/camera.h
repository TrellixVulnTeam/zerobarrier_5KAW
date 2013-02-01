#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

class Camera2d {
public:
  Camera2d(void);

  void Initialize(const aabb2d &viewport, f32 minDepth, f32 maxDepth);

  void SetLocation(const vector3d &location);
  void SetLocation(const vector2d &location);
  void SetOrientation(const complex &orientation);
  void SetViewport(const aabb2d &viewport);
  void SetZoom(const f32 zoom);

  void Move(const vector3d &delta);
  void Rotate(const complex &rotation);
  void Zoom(const f32 zoom_delta);

  vector3d GetLocation(void) const;
  f32 GetZoom(void) const;
  const aabb2d& GetViewport(void) const;
  aabb2d GetViewportWorldBounds(void);
  vector2d ViewportToWorld(vector2d position);

  void OrthoProjection(void);
  void FinalizeMatrices(void);

  D3DXMATRIX View;
  D3DXMATRIX Projection;
  D3DXMATRIX ViewInverse;
  D3DXMATRIX ProjectionInverse;

private:
  vector3d location;
  complex orientation;
  aabb2d viewport;
  f32 minDepth;
  f32 maxDepth;
  f32 zoom;
  bool viewDirty;
  bool projectionDirty;
};

ZB_NAMESPACE_END
