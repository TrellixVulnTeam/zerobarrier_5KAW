#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

class line2d {
public:
  ZB_INLINE line2d(void) {}
  ZB_INLINE line2d(const vector2d point, const vector2d direction) : p(point), v(direction) {}
  ZB_INLINE line2d(f32 px, f32 py, f32 vx, f32 vy) : p(px, py), v(vx, vy) {}

  vector2d p;
  vector2d v;
};

class segment2d {
public:
  ZB_INLINE segment2d(void) {}
  ZB_INLINE segment2d(const vector2d A, const vector2d B) : a(A), b(B) {}
  ZB_INLINE segment2d(f32 ax, f32 ay, f32 bx, f32 by) : a(ax, ay), b(bx, by) {}
 
  ZB_INLINE vector2d midpoint(void) {
    return a + (b - a) * 0.5f;
  }
  vector2d a;
  vector2d b;
};

ZB_NAMESPACE_END
