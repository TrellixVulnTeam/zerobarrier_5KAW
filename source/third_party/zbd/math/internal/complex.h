#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

f32 abs(const class complex c);

class complex {
 public:
  ZB_INLINE complex(void) : real(0.0f), imaginary(0.0f) {}
  ZB_INLINE complex(f32 Real, f32 Imaginary) : real(Real), imaginary(Imaginary) {}

  ZB_INLINE bool operator==(const complex rhs) { 
    return real == rhs.real && imaginary == rhs.imaginary;
  }

  ZB_INLINE bool operator!=(const complex rhs) { 
    return !(*this == rhs);
  }

  ZB_INLINE complex operator-(void) const { 
    return complex(-real, -imaginary);
  }

  ZB_INLINE complex operator+(const complex rhs) const { 
    return complex(real + rhs.real, imaginary + rhs.imaginary);
  }

  ZB_INLINE complex operator-(const complex rhs) const { 
    return complex(real - rhs.real, imaginary - rhs.imaginary);
  }

  ZB_INLINE complex operator*(const complex rhs) const { 
    return complex(real*rhs.real - imaginary*rhs.imaginary, real*rhs.imaginary + imaginary*rhs.real);
  }

  ZB_INLINE complex operator/(const complex rhs) const {
    const f32 denominator = rhs.real*rhs.real + rhs.imaginary*rhs.imaginary;
    defend (denominator != 0.0f);
    return complex((real*rhs.real + imaginary*rhs.imaginary)/denominator, (real*rhs.imaginary - imaginary*rhs.real)/denominator);
  }

  ZB_INLINE void operator+=(const complex rhs) { 
    real += rhs.real; 
    imaginary += rhs.imaginary; 
  }

  ZB_INLINE void operator-=(const complex rhs) { 
    real -= rhs.real; 
    imaginary -= rhs.imaginary; 
  }

  ZB_INLINE void operator*=(const complex rhs) {
    const f32 old_real = real;
    real = real*rhs.real - imaginary*rhs.imaginary; 
    imaginary = old_real*rhs.imaginary + imaginary*rhs.real; 
  }

  ZB_INLINE void operator/=(const complex rhs) {
    const f32 old_real = real;
    const f32 denominator = rhs.real*rhs.real + rhs.imaginary*rhs.imaginary;
    defend (denominator != 0.0f);
    real = (real*rhs.real + imaginary*rhs.imaginary) / denominator;
    imaginary = (old_real*rhs.imaginary - imaginary*rhs.real) / denominator;
  }

  ZB_INLINE f32 Normalize(void) { 
    const f32 mag = abs(*this); 
    defend (mag != 0.0f); 
    real /= mag; 
    imaginary /= mag;
    return mag;
  }

  f32 real, imaginary;
};

ZB_INLINE f32 abs(const complex c) { 
  return sqrt(c.real*c.real + c.imaginary*c.imaginary);
}

ZB_INLINE complex conjugate(const complex c) { 
  return complex(c.real, -c.imaginary);
}

ZB_INLINE f32 ComplexToRadians(const complex c) { 
  defend (c.real != 0.0f || c.imaginary != 0.0f); 
  return clamp(atan2(c.imaginary, c.real), 0.0f, 2.0f*Pi);
}

ZB_INLINE complex ComplexFromRadians(radians theta) { 
  return complex(cos(theta), sin(theta));
}

ZB_NAMESPACE_END
