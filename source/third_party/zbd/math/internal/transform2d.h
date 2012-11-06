#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

class transform2d {
 public:
  static const transform2d Identity;

  ZB_INLINE transform2d(void) {
    translation[0] = 0.0f;
    translation[1] = 0.0f;
    matrix[0] = 1.0f;
    matrix[1] = 0.0f;
    matrix[2] = 0.0f;
    matrix[3] = 1.0f;
  }

  ZB_INLINE transform2d(const vector2d T, const vector2d S, const complex R) {
    translation[0] = T.x;
    translation[1] = T.y;

    matrix[0] = S.x*R.real;
    matrix[1] = -S.y*R.imaginary;
    matrix[2] = S.x*R.imaginary;
    matrix[3] = S.y*R.real;
  }

  ZB_INLINE transform2d(const f32 tx, const f32 ty, const f32 m00, const f32 m01, const f32 m10, const f32 m11) {
    translation[0] = tx;
    translation[1] = ty;
    
    matrix[0] = m00;
    matrix[1] = m01;
    matrix[2] = m10;
    matrix[3] = m11;
  }

  ZB_INLINE void Scale(const f32 sx, const f32 sy) {
    matrix[0] *= sx;
    matrix[1] *= sx;
    matrix[2] *= sy;
    matrix[3] *= sy;
  }

  ZB_INLINE void Scale(const vector2d S) {
    Scale(S.x, S.y);
  }

  ZB_INLINE void Rotate(const f32 real, const f32 imaginary) {
    const f32 m00 = matrix[0];
    const f32 m01 = matrix[1];
    const f32 m10 = matrix[2];
    const f32 m11 = matrix[3];

    matrix[0] = real*m00 - imaginary*m10;
    matrix[1] = real*m01 - imaginary*m11;
    matrix[2] = real*m10 + imaginary*m00;
    matrix[3] = real*m11 + imaginary*m01;
  }

  ZB_INLINE void Rotate(const complex R) {
    Rotate(R.real, R.imaginary);
  }

  ZB_INLINE void Rotate(const f32 r) {
    Rotate(ComplexFromRadians(r));
  }

  ZB_INLINE void Orient(const complex R) {
    const f32 scaleX = vector2d(matrix[0], matrix[1]).Length();
    const f32 scaleY = vector2d(matrix[2], matrix[3]).Length();
    
    matrix[0] = R.real * scaleX;
    matrix[1] = -R.imaginary * scaleY;
    matrix[2] = R.imaginary * scaleX;
    matrix[3] = R.real * scaleY;
  }

  ZB_INLINE void Orient(const f32 r) {
    Orient(ComplexFromRadians(r));
  }

  ZB_INLINE void Move(const f32 x, const f32 y) {
    translation[0] += x;
    translation[1] += y;
  }

  ZB_INLINE void Move(const vector2d T) {
    Move(T.x, T.y);
  }

  ZB_INLINE void Place(const vector2d T) {
    translation[0] = T.x;
    translation[1] = T.y;
  }

  ZB_INLINE vector2d GetTranslation(void) const {
    return vector2d(translation[0], translation[1]);
  }

  ZB_INLINE vector2d GetForward(void) const {
    return vector2d(matrix[0], matrix[1]);
  }

  ZB_INLINE vector2d GetUp(void) const {
    return vector2d(matrix[2], matrix[3]);
  }

  ZB_INLINE vector2d GetScale(void) {
    const f32 scaleX = vector2d(matrix[0], matrix[1]).Length();
    const f32 scaleY = vector2d(matrix[2], matrix[3]).Length();
    return vector2d(scaleX, scaleY);
  }

  ZB_INLINE complex GetOrientation(void) {
    const f32 scaleX = vector2d(matrix[0], matrix[1]).Length();
    return complex(matrix[0] / scaleX, matrix[1] / scaleX);
  }

  ZB_INLINE transform2d operator*(const transform2d rhs) const {
    const f32 t0 = matrix[0]*rhs.translation[0] + matrix[1]*rhs.translation[1] + translation[0];
    const f32 t1 = matrix[2]*rhs.translation[0] + matrix[3]*rhs.translation[1] + translation[1];
    const f32 m00 = matrix[0]*rhs.matrix[0] + matrix[1]*rhs.matrix[2];
    const f32 m01 = matrix[0]*rhs.matrix[1] + matrix[1]*rhs.matrix[3];
    const f32 m10 = matrix[2]*rhs.matrix[0] + matrix[3]*rhs.matrix[2];
    const f32 m11 = matrix[2]*rhs.matrix[1] + matrix[3]*rhs.matrix[3];
    
    return transform2d(t0, t1, m00, m01, m10, m11);
  }

  ZB_INLINE vector2d TransformPoint(const vector2d point) const {
    return vector2d(point.x*matrix[0] + point.y*matrix[1] + translation[0], point.x*matrix[2] + point.y*matrix[3] + translation[1]);
  }

  ZB_INLINE vector2d TransformVector(const vector2d vector) const {
    return vector2d(vector.x*matrix[0] + vector.y*matrix[1], vector.x*matrix[2] + vector.y*matrix[3]);
  }

  ZB_INLINE transform2d Inverse(void) const {
    const f32 det = matrix[0]*matrix[3] - matrix[1]*matrix[2];
    defend (det != 0.0f);

    const f32 t0 = (matrix[1]*translation[1] - matrix[3]*translation[0]) / det;
    const f32 t1 = (matrix[2]*translation[0] - matrix[0]*translation[1]) / det;
    return transform2d(t0, t1, matrix[3] / det, -matrix[1] / det, -matrix[2] / det, matrix[0] / det);
  }

  ZB_INLINE transform2d NormalTransform(void) const {
    const f32 det = matrix[0]*matrix[3] - matrix[1]*matrix[2];
    defend (det != 0.0f);

    return transform2d(0.0f, 0.0f, matrix[3] / det, -matrix[2] / det, -matrix[1] / det, matrix[0] / det);
  }

  f32 translation[2], matrix[4];
};

ZB_NAMESPACE_END
