#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

class aabb2d {
 public:
  ZB_INLINE aabb2d(void) {}
  ZB_INLINE aabb2d(const vector2d Center, const vector2d Extents) : center(Center), extents(Extents) {}
  ZB_INLINE aabb2d(const f32 cx, const f32 cy, const f32 ex, const f32 ey) : center(cx, cy), extents(ex, ey) {}

  ZB_INLINE aabb2d join(const aabb2d rhs) const {
    const vector2d minA = center - extents;
    const vector2d maxA = center + extents;
    const vector2d minB = rhs.center - rhs.extents;
    const vector2d maxB = rhs.center + rhs.extents;

    const vector2d minFinal(min(minA.x, minB.x), min(minA.y, minB.y));
    const vector2d maxFinal(max(maxA.x, maxB.x), max(maxA.y, maxB.y));
    const vector2d extent = (maxFinal - minFinal) / 2.0f;
    
    return aabb2d(minFinal + extent, extent);
  }

  ZB_INLINE aabb2d joinUninitialized(const aabb2d rhs) const {
    if (center == vector2d::Zero && extents == vector2d::Zero) {
      return rhs;
    }
    else {
      return join(rhs);
    }
  }

  vector2d center, extents;
};

ZB_NAMESPACE_END
