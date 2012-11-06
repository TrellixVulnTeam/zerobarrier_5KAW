#pragma once

//
// See copyright notice in zbd.h.
//

class matrix3x3 {
public:
  static const matrix3x3 Identity;

  ZB_INLINE matrix3x3(void) 
  : m00(1.0f), m01(0.0f), m02(0.0f)
  , m10(0.0f), m11(1.0f), m12(0.0f)
  , m20(0.0f), m21(0.0f), m22(1.0f) {
    // Do nothing.
  }

  ZB_INLINE matrix3x3(f32 M00, f32 M01, f32 M02, f32 M10, f32 M11, f32 M12, f32 M20, f32 M21, f32 M22)
  : m00(M00), m01(M01), m02(M02)
  , m10(M10), m11(M11), m12(M12)
  , m20(M20), m21(M21), m22(M22) {
    // Do nothing.
  }

  ZB_INLINE vector3d TransformVector(const vector3d v) const {
    return vector3d(m00 * v.x + m01 * v.y + m02 * v.z, m10 * v.x + m11 * v.y + m12 * v.z, m20 * v.x + m21 * v.y + m22 * v.z);
  }

  ZB_INLINE f32 Determinant(void) const {
    return m00 * m11 * m22 + m01 * m12 * m20 + m02 * m10 * m21 - m02 * m11 * m20 - m01 * m10 * m22 - m00 * m12 * m21;
  }

  ZB_INLINE matrix3x3 Inverse(void) const {
    const f32 d = Determinant();
    defend (d != 0.0f);
    
    return matrix3x3((m11*m22-m21*m12)/d, (m02*m21-m22*m01)/d, (m01*m12-m11*m02)/d,
                     (m12*m20-m22*m10)/d, (m00*m22-m20*m02)/d, (m02*m10-m12*m00)/d,
                     (m10*m21-m20*m11)/d, (m01*m20-m21*m00)/d, (m00*m11-m10*m01)/d);
  }

  f32 m00, m01, m02;
  f32 m10, m11, m12;
  f32 m20, m21, m22;
};
