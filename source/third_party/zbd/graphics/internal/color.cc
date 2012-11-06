//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

const Color Color::White(1.0f, 1.0f, 1.0f, 1.0f);
const Color Color::Black(0.0f, 0.0f, 0.0f, 1.0f);
const Color Color::Red(1.0f, 0.0f, 0.0f, 1.0f);
const Color Color::Green(0.0f, 1.0f, 0.0f, 1.0f);
const Color Color::Blue(0.0f, 0.0f, 1.0f, 1.0f);
const Color Color::Teal(0.0f, 0.5f, 0.5f, 1.0f);
const Color Color::Magenta(1.0f, 0.0f, 1.0f, 1.0f);
const Color Color::Invisible(0.0f, 0.0f, 0.0f, 0.0f);

Color::Color(void) 
: r(0.0f)
, g(0.0f)
, b(0.0f)
, a(1.0f) {
  // Do nothing.
}

Color::Color(f32 R, f32 G, f32 B, f32 A)
: r(R)
, g(G)
, b(B)
, a(A) {
  // Do nothing.
}

Color::Color(f32 R, f32 G, f32 B)
: r(R)
, g(G)
, b(B)
, a(1.0) {
  // Do nothing.
}

bool Color::operator==(const Color &rhs) const {
  return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
}

bool Color::operator!=(const Color &rhs) const {
  return !(*this == rhs);
}

ZB_NAMESPACE_END
