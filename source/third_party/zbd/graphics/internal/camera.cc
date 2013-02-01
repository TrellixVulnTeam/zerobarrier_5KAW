//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

namespace {
  const f32 MIN_ZOOM = 0.001f;
  const f32 MAX_ZOOM = 100.0f;
}

Camera2d::Camera2d(void)
: orientation(1.0f, 0.0f)
, viewport(0.0f, 0.0f, 0.0f, 0.0f)
, minDepth(0.0f)
, maxDepth(0.0f)
, zoom(1.0f)
, viewDirty(true)
, projectionDirty(true) {
  D3DXMatrixIdentity(&View);
  D3DXMatrixIdentity(&Projection);
}

void Camera2d::Initialize(const aabb2d& viewport, f32 minDepth, f32 maxDepth) {
  defend (viewport.width() > 0.0f);
  defend (viewport.height() > 0.0f);
  defend (minDepth <= maxDepth);

  this->viewport = viewport;
  this->minDepth = minDepth;
  this->maxDepth = maxDepth;
  FinalizeMatrices();
}

void Camera2d::OrthoProjection(void) {
  const f32 halfWidth = zoom * viewport.extents.x;
  const f32 halfHeight = zoom * viewport.extents.y;
  D3DXMatrixOrthoOffCenterRH(&Projection, -halfWidth, halfWidth, -halfHeight, halfHeight, minDepth, maxDepth);
}

void Camera2d::FinalizeMatrices(void) {
  if (viewDirty) {
    const D3DXVECTOR3 eye(location.x, location.y, location.z);
    const D3DXVECTOR3 at(eye - D3DXVECTOR3(0.0f, 0.0f, 1.0f));

    vector2d upVector(0.0f, 1.0f);
    upVector = upVector * orientation;
    const D3DXVECTOR3 up(upVector.x, upVector.y, 0.0f);

    D3DXMatrixLookAtRH(&View, &eye, &at, &up);
    D3DXMatrixInverse(&ViewInverse, NULL, &View);
    viewDirty = false;
  }

  if (projectionDirty) {
    OrthoProjection();
    D3DXMatrixInverse(&ProjectionInverse, NULL, &Projection);
    projectionDirty = false;
  }
}

vector3d Camera2d::GetLocation(void) const {
  return location;
}

void Camera2d::SetLocation(const vector3d &location) {
  viewDirty = true;
  this->location = location;
}

void Camera2d::SetLocation(const vector2d &location) {
  const vector3d newLocation(location.x, location.y, this->location.z);
  SetLocation(newLocation);
}

void Camera2d::SetOrientation(const complex &orientation) {
  viewDirty = true;
  this->orientation = orientation;
}

void Camera2d::SetViewport(const aabb2d &viewport) {
  projectionDirty = true;
  this->viewport = viewport;
}

void Camera2d::SetZoom(const f32 zoom) {
  projectionDirty = true;
  this->zoom = clamp(zoom, MIN_ZOOM, MAX_ZOOM);
}

void Camera2d::Move(const vector3d &delta) {
  viewDirty = true;
  location += delta;
}

void Camera2d::Rotate(const complex &rotation) {
  viewDirty = true;
  orientation = rotation * orientation;
}

void Camera2d::Zoom(const f32 zoomDelta) {
  projectionDirty = true;
  zoom = clamp(zoom + zoomDelta, MIN_ZOOM, MAX_ZOOM);
}

f32 Camera2d::GetZoom(void) const {
  return zoom;
}

const aabb2d &Camera2d::GetViewport(void) const {
  return viewport;
}

aabb2d Camera2d::GetViewportWorldBounds(void) {
  // Construct 2 aabb's out of the diagonals transformed to world space, then join them together.
  // This is so that a rotated camera will still have a correct bounding area in world space.
  const aabb2d a = ToAABB2d(segment2d(ViewportToWorld(viewport.min()),
                                      ViewportToWorld(viewport.max())));
  const aabb2d b = ToAABB2d(segment2d(ViewportToWorld(viewport.tl()),
                                      ViewportToWorld(viewport.br())));
  return a.join(b);
}

vector2d Camera2d::ViewportToWorld(vector2d position) {
  FinalizeMatrices();
  position = componentwise_divide(position, viewport.extents);

  D3DXVECTOR4 d3dPosition(position.x - 1.0f, -position.y + 1.0f, 0.0f, 1.0f);
  D3DXVec4Transform(&d3dPosition, &d3dPosition, &ProjectionInverse);
  D3DXVec4Transform(&d3dPosition, &d3dPosition, &ViewInverse);

  return vector2d(d3dPosition.x, d3dPosition.y);
}

ZB_NAMESPACE_END
