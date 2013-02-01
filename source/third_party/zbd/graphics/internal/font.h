#pragma once

//
// See copyright notice in zbd.h.
//

typedef i32 FontHandle;

namespace Font {
  void Initialize(void);
  void Shutdown(void);

  FontHandle Create(TextureHandle texture, f32 textureWidth, f32 textureHeight, f32 glyphWidth, f32 glyphHeight, f32 glyphSpacingX, f32 glyphSpacingY);
  extern FontHandle DefaultFont;
}

namespace Draw {
  void Text(const zbstring &text, vector2d topLeft, FontHandle font = Font::DefaultFont, const Color &color = Color::Black);
}
