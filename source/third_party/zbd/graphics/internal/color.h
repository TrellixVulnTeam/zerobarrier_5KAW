#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

class Color {
public:
  Color(void);
  Color(f32 red, f32 green, f32 blue, f32 alpha);
  Color(f32 red, f32 green, f32 blue);

  bool operator==(const Color &rhs) const;
  bool operator!=(const Color &rhs) const;

  f32 r, g, b, a;

  const static Color White;
  const static Color Black;
  const static Color Red;
  const static Color Green;
  const static Color Blue;
  const static Color Teal;
  const static Color Magenta;
  const static Color Invisible;
};

ZB_NAMESPACE_END
