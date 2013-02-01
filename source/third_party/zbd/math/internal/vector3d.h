#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

class vector2d;
class complex;

class vector3d {
 public:
  static const vector3d Zero;

  ZB_INLINE vector3d(void) {
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
  }

  ZB_INLINE vector3d(f32 X, f32 Y, f32 Z) {
    x = X;
    y = Y;
    z = Z;
  }

  ZB_INLINE vector3d(const vector2d source, f32 z) {
    x = source.x;
    y = source.y;
    z = z;
  }

  ZB_INLINE vector3d(const vector2d source) {
    x = source.x;
    y = source.y;
    z = 0.0f;
  }

  ZB_INLINE bool operator==(const vector3d rhs) const {
    return x == rhs.x && y == rhs.y && z == rhs.z;
  }

  ZB_INLINE bool operator!=(const vector3d rhs) const {
    return !(*this == rhs);
  }

  ZB_INLINE vector3d operator-(void) const {
    return *this * -1.0f;
  }

  ZB_INLINE vector3d operator-(const vector3d rhs) const {
    return vector3d(x - rhs.x, y - rhs.y, z - rhs.z);
  }

  ZB_INLINE vector3d operator+(const vector3d rhs) const {
    return vector3d(x + rhs.x, y + rhs.y, z + rhs.z);
  }

  ZB_INLINE vector3d operator*(const f32 rhs) const {
    return vector3d(x*rhs, y*rhs, z*rhs);
  }

  ZB_INLINE vector3d operator/(const f32 rhs) const {
    defend (rhs != 0.0f);
    return vector3d(x/rhs, y/rhs, z/rhs);
  }

  ZB_INLINE vector3d operator*(const complex rhs) const {
    return vector3d(x*rhs.real - y*rhs.imaginary, x*rhs.imaginary + y*rhs.real, z);
  }

  ZB_INLINE void operator=(const vector2d rhs) {
    x = rhs.x;
    y = rhs.y;
    // leave z alone
  }

  ZB_INLINE void operator+=(const vector3d rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
  }

  ZB_INLINE void operator-=(const vector3d rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
  }

  ZB_INLINE void operator*=(const f32 rhs) {
    x *= rhs;
    y *= rhs;
    z *= rhs;
  }

  ZB_INLINE void operator/=(const f32 rhs) {
    defend (rhs != 0.0f);
    x /= rhs;
    y /= rhs;
    z /= rhs;
  }

  ZB_INLINE void operator*=(const complex rhs) {
    const f32 old_x = x;
    x = old_x*rhs.real - y*rhs.imaginary;
    y = old_x*rhs.imaginary + y*rhs.real;
  }

  ZB_INLINE f32 Length(void) const {
    return sqrt(x*x + y*y + z*z);
  }

  ZB_INLINE f32 Normalize(void) {
    const f32 length = Length();
    defend (length != 0.0f);
    *this /= length;
    return length;
  }

  ZB_INLINE f32 SafeNormalize(vector3d default) {
    const f32 length = Length();
    if (length > Epsilon_Medium) {
      *this /= length;
      return length;
    }
    else {
      *this = default;
      return 0.0f;
    }
  }

  ZB_INLINE vector3d Normal(void) const {
    const f32 length = Length();
    defend (length != 0.0f);

    return vector3d(x/length, y/length, z/length);
  }

  f32 x, y, z;
};

ZB_INLINE f32 dot(const vector3d lhs, const vector3d rhs) {
  return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
}

ZB_INLINE vector3d cross(const vector3d lhs, const vector3d rhs) {
  return vector3d(lhs.y*rhs.z - lhs.z*rhs.y,
    lhs.z*rhs.x - lhs.x*rhs.z,
    lhs.x*rhs.y - lhs.y*rhs.x);
}

ZB_INLINE vector3d operator*(const f32 lhs, const vector3d rhs) {
  return rhs * lhs;
}

ZB_INLINE vector3d componentwise_multiply(const vector3d lhs, const vector3d rhs) {
  return vector3d(lhs.x*rhs.x, lhs.y*rhs.y, lhs.z*rhs.z);
}

ZB_INLINE vector3d componentwise_divide(const vector3d lhs, const vector3d rhs) {
  defend (rhs.x != 0.0f);
  defend (rhs.y != 0.0f);
  defend (rhs.z != 0.0f);
  return vector3d(lhs.x/rhs.x, lhs.y/rhs.y, lhs.z/rhs.z);
}

// This exists in this file because of the vector2d-vector3d header dependency.
ZB_INLINE vector2d::vector2d(const vector3d source) {
  x = source.x;
  y = source.y;
}

ZB_NAMESPACE_END
