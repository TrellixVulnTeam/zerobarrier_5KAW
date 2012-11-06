//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

namespace {
  const f32 MIN_ZOOM = 0.001f;
  const f32 MAX_ZOOM = 100.0f;
}

Camera::Camera(void)
: orientation(1.0f, 0.0f)
, width(0.0f)
, height(0.0f)
, min_depth(0.0f)
, max_depth(0.0f)
, zoom(1.0f)
, view_dirty(true)
, projection_dirty(true) {
  D3DXMatrixIdentity(&View);
  D3DXMatrixIdentity(&Projection);
}

void Camera::Initialize(f32 width, f32 height, f32 minDepth, f32 maxDepth) {
  defend (width > 0.0f);
  defend (height > 0.0f);
  defend (minDepth <= maxDepth);

  this->width = width;
  this->height = height;
  min_depth = minDepth;
  max_depth = maxDepth;
  FinalizeMatrices();
}

void Camera::OrthoProjection(void) {
  const f32 half_width = zoom * width / 2.0f;
  const f32 half_height = zoom * height / 2.0f;
  D3DXMatrixOrthoOffCenterRH(&Projection, -half_width, half_width, -half_height, half_height, min_depth, max_depth);
}

void Camera::FinalizeMatrices(void) {
  if (view_dirty) {
    const D3DXVECTOR3 eye(location.x, location.y, location.z);
    const D3DXVECTOR3 at(eye - D3DXVECTOR3(0.0f, 0.0f, 1.0f));

    vector2d upVector(0.0f, 1.0f);
    upVector = upVector * orientation;
    const D3DXVECTOR3 up(upVector.x, upVector.y, 0.0f);

    D3DXMatrixLookAtRH(&View, &eye, &at, &up);
    D3DXMatrixInverse(&ViewInverse, NULL, &View);
    view_dirty = false;
  }

  if (projection_dirty) {
    OrthoProjection();
    D3DXMatrixInverse(&ProjectionInverse, NULL, &Projection);
    projection_dirty = false;
  }
}

vector3d Camera::GetLocation(void) const {
  return location;
}

void Camera::SetLocation(const vector3d &location) {
  view_dirty = true;
  this->location = location;
}

void Camera::SetLocation(const vector2d &location) {
  const vector3d newLocation(location.x, location.y, this->location.z);
  SetLocation(newLocation);
}

void Camera::SetOrientation(const complex &orientation) {
  view_dirty = true;
  this->orientation = orientation;
}

void Camera::SetZoom(const f32 zoom) {
  projection_dirty = true;
  this->zoom = clamp(zoom, MIN_ZOOM, MAX_ZOOM);
}

void Camera::Move(const vector3d &delta) {
  view_dirty = true;
  location += delta;
}

void Camera::Rotate(const complex &rotation) {
  view_dirty = true;
  orientation = rotation * orientation;
}

void Camera::Zoom(const f32 zoomDelta) {
  projection_dirty = true;
  zoom = clamp(zoom + zoomDelta, MIN_ZOOM, MAX_ZOOM);
}

f32 Camera::GetWidth(void) const {
  return width;
}

f32 Camera::GetHeight(void) const {
  return height;
}

f32 Camera::GetZoom(void) const {
  return zoom;
}

ZB_NAMESPACE_END
