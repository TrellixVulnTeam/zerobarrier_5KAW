#pragma once

//
// See copyright notice in zbd.h.
//

class matrix4x4 {
public:
  static const matrix4x4 Identity;

  ZB_INLINE matrix4x4(void) 
  : m00(1.0f), m01(0.0f), m02(0.0f), m03(0.0f)
  , m10(0.0f), m11(1.0f), m12(0.0f), m13(0.0f)
  , m20(0.0f), m21(0.0f), m22(1.0f), m23(0.0f)
  , m30(0.0f), m31(0.0f), m32(0.0f), m33(1.0f) {
      // Do nothing.
  }

  ZB_INLINE matrix4x4(f32 M00, f32 M01, f32 M02, f32 M03, f32 M10, f32 M11, f32 M12, f32 M13, f32 M20, f32 M21, f32 M22, f32 M23, f32 M30, f32 M31, f32 M32, f32 M33)
  : m00(M00), m01(M01), m02(M02), m03(M03)
  , m10(M10), m11(M11), m12(M12), m13(M13)
  , m20(M20), m21(M21), m22(M22), m23(M23)
  , m30(M30), m31(M31), m32(M32), m33(M33) {
      // Do nothing.
  }

  ZB_INLINE vector3d TransformVector(const vector3d v) const {
    return vector3d(m00 * v.x + m01 * v.y + m02 * v.z, m10 * v.x + m11 * v.y + m12 * v.z, m20 * v.x + m21 * v.y + m22 * v.z);
  }

  ZB_INLINE vector3d TransformPoint(const vector3d v) const {
    return vector3d(m00 * v.x + m01 * v.y + m02 * v.z + m03, m10 * v.x + m11 * v.y + m12 * v.z + m13, m20 * v.x + m21 * v.y + m22 * v.z + m23);
  }

  f32 m00, m01, m02, m03;
  f32 m10, m11, m12, m13;
  f32 m20, m21, m22, m23;
  f32 m30, m31, m32, m33;
};
