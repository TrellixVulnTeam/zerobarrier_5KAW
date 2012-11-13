#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

template <typename T>
ZB_INLINE T min(const T a, const T b) {
  return a < b ? a : b;
}

template <typename T>
ZB_INLINE T max(const T a, const T b) {
  return a > b ? a : b;
}

template <typename T>
ZB_INLINE T clamp(const T x, const T min, const T max) {
  return x < min ? min : x > max ? max : x;
}

template <typename T>
ZB_INLINE T normalizeAngle(T angle) {
  return fmod(angle, 2.0f * Pi);
}

ZB_NAMESPACE_END
