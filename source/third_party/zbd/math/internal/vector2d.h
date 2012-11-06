#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

class vector3d;
class complex;

class vector2d {
 public:
  static const vector2d Zero;

  ZB_INLINE vector2d(void) { 
    x = 0.0f; 
    y = 0.0f;
  }

  ZB_INLINE vector2d(f32 X, f32 Y) { 
    x = X; 
    y = Y;
  }

  ZB_INLINE vector2d(const vector3d source);

  ZB_INLINE bool operator==(const vector2d rhs) const { 
    return x == rhs.x && y == rhs.y;
  }

  ZB_INLINE bool operator!=(const vector2d rhs) const { 
    return !(*this == rhs);
  }

  ZB_INLINE vector2d operator-(void) const { 
    return *this * -1.0f;
  }

  ZB_INLINE vector2d operator-(const vector2d rhs) const { 
    return vector2d(x-rhs.x, y-rhs.y);
  }

  ZB_INLINE vector2d operator+(const vector2d rhs) const { 
    return vector2d(x+rhs.x, y+rhs.y);
  }

  ZB_INLINE vector2d operator*(const f32 rhs) const { 
    return vector2d(x*rhs, y*rhs);
  }

  ZB_INLINE vector2d operator/(const f32 rhs) const { 
    defend (rhs != 0.0f); 
    return vector2d(x/rhs, y/rhs);
  }

  ZB_INLINE vector2d operator*(const complex rhs) const {
    return vector2d(x*rhs.real - y*rhs.imaginary, x*rhs.imaginary + y*rhs.real);
  }

  ZB_INLINE void operator+=(const vector2d rhs) { 
    x += rhs.x; 
    y += rhs.y;
  }

  ZB_INLINE void operator-=(const vector2d rhs) { 
    x -= rhs.x; 
    y -= rhs.y; 
  }

  ZB_INLINE void operator*=(const f32 rhs) { 
    x *= rhs; 
    y *= rhs;
  }

  ZB_INLINE void operator/=(const f32 rhs) { 
    defend (rhs != 0.0f); 
    x /= rhs; 
    y /= rhs;
  }

  ZB_INLINE void operator*=(const complex rhs) {
    const f32 old_x = x;
    x = old_x*rhs.real - y*rhs.imaginary;
    y = old_x*rhs.imaginary + y*rhs.real;
  }

  ZB_INLINE void Set(f32 X, f32 Y) {
    x = X;
    y = Y;
  }

  ZB_INLINE f32 Length(void) const { 
    return sqrt(x*x + y*y);
  }

  ZB_INLINE f32 LengthSq(void) const {
    return x*x + y*y;
  }

  ZB_INLINE f32 Normalize(void) { 
    const f32 length = Length(); 
    defend (length != 0.0f); 
    *this /= length;
    return length; 
  }

  ZB_INLINE f32 SafeNormalize(vector2d default) {
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

  ZB_INLINE vector2d Normal(void) const {
    const f32 length = Length();
    defend (length != 0.0f);

    return vector2d(x/length, y/length);
  }

  f32 x, y;
};

ZB_INLINE f32 dot(const vector2d lhs, const vector2d rhs) { 
  return lhs.x*rhs.x + lhs.y*rhs.y;
}

ZB_INLINE f32 wedge(const vector2d lhs, const vector2d rhs) { 
  return lhs.x*rhs.y - lhs.y*rhs.x;
}

ZB_INLINE vector2d operator*(const f32 lhs, const vector2d rhs) { 
  return rhs * lhs;
}

ZB_INLINE vector2d componentwise_multiply(const vector2d lhs, const vector2d rhs) {
  return vector2d(lhs.x*rhs.x, lhs.y*rhs.y);
}

ZB_INLINE vector2d componentwise_divide(const vector2d lhs, const vector2d rhs) {
  defend (rhs.x != 0.0f);
  defend (rhs.y != 0.0f);
  return vector2d(lhs.x/rhs.x, lhs.y/rhs.y);
}

ZB_INLINE f32 angle_between(const vector2d lhs, const vector2d rhs) {
  return acos(clamp(dot(lhs.Normal(), rhs.Normal()), -1.0f, 1.0f));
}

ZB_NAMESPACE_END
