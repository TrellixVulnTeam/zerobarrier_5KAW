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
    if (center == vector2d::Zero && extents == vector2d::Zero) {
      return rhs;
    }

    const vector2d minA = min();
    const vector2d maxA = max();
    const vector2d minB = rhs.min();
    const vector2d maxB = rhs.max();

    const vector2d minFinal(::min(minA.x, minB.x), ::min(minA.y, minB.y));
    const vector2d maxFinal(::max(maxA.x, maxB.x), ::max(maxA.y, maxB.y));
    const vector2d extent = (maxFinal - minFinal) / 2.0f;

    return aabb2d(minFinal + extent, extent);
  }

  ZB_INLINE f32 width(void) const { return extents.x * 2.0f; }
  ZB_INLINE f32 height(void) const { return extents.y * 2.0f; }

  ZB_INLINE vector2d min(void) const { return center - extents; }
  ZB_INLINE vector2d max(void) const { return center + extents; }

  // These assume a right-handed y-up coordinate system, not sure what else to name them...
  ZB_INLINE vector2d tl(void) const { return center + vector2d(-extents.x, extents.y); }
  ZB_INLINE vector2d br(void) const { return center + vector2d(extents.x, -extents.y); }
  ZB_INLINE f32 left(void) const { return center.x - extents.x; }
  ZB_INLINE f32 right(void) const { return center.x + extents.x; }
  ZB_INLINE f32 top(void) const { return center.y + extents.y; }
  ZB_INLINE f32 bottom(void) const { return center.y - extents.y; }

  vector2d center, extents;
};

ZB_NAMESPACE_END
