#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

class circle {
 public:
  ZB_INLINE circle(void) : radius(0.0f) {}
  ZB_INLINE circle(const vector2d Center, const f32 Radius) : center(Center), radius(Radius) {}

  vector2d center;
  f32 radius;
};

ZB_NAMESPACE_END
