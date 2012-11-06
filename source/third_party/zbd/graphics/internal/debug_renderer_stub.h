#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

class DebugRenderer {
public:
  ZB_INLINE static void Line(const vector2d &, const vector2d &, const Color &color = Color::Black, seconds time = 0.0f) { silence_unused(color); silence_unused(time); }
  ZB_INLINE static void Arrow(const vector2d &, const vector2d &, const Color &color = Color::Black, seconds time = 0.0f, f32 length = 8.0f, degrees angle = 30.0f) { silence_unused(color); silence_unused(time); silence_unused(length); silence_unused(angle); }
  ZB_INLINE static void Box(const aabb2d &, const Color &color = Color::White, seconds time = 0.0f) { silence_unused(color); silence_unused(time); }
  ZB_INLINE static void Box(const vector2d &, const vector2d &, const vector2d &, const vector2d &, const Color &color = Color::Black, seconds time = 0.0f) { silence_unused(color); silence_unused(time); }
  ZB_INLINE static void Triangle(const vector2d &, const vector2d &, const vector2d &, const Color &color = Color::Black, seconds time = 0.0f) { silence_unused(color); silence_unused(time); }
  ZB_INLINE static void Ngon(const vector2d &, f32, u32, const Color &color = Color::Black, seconds time = 0.0f, radians orientation = RadPi2) { silence_unused(color); silence_unused(time); silence_unused(orientation); }
};

ZB_NAMESPACE_END
