#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

template <typename T>
T min(const T a, const T b) {
  return a < b ? a : b;
}

template <typename T>
T max(const T a, const T b) {
  return a > b ? a : b;
}

template <typename T>
T clamp(const T x, const T min, const T max) {
  return x < min ? min : x > max ? max : x;
}

ZB_INLINE f32 normalizeAngle(f32 angle) {
  return fmod(angle, 2.0f * Pi);
}

ZB_NAMESPACE_END
