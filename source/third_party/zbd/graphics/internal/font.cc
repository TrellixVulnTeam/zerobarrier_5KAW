
namespace Font {
  struct FixedWidthFont {
    FixedWidthFont(void)
    : texture(0) {
      // Do nothing.
    }

    TextureHandle texture;
    vector2d textureDimensions;
    vector2d glyphDimensions;
    vector2d glyphSpacing;
  };

  FontHandle nextFontHandle = 1;
  zbmap(FontHandle, FixedWidthFont) fonts;
  FontHandle DefaultFont = 0;

  void Initialize(void) {
    DefaultFont = Create(RENDERTHREAD->LoadTextureFromFile("textures/small.png"), 128.0f, 128.0f, 8.0f, 13.0f, 1.0f, 1.0f);
  }

  void Shutdown(void) {
    RENDERTHREAD->UnloadTexture(fonts[DefaultFont].texture);
    DefaultFont = 0;
  }

  FontHandle Create(TextureHandle texture, f32 textureWidth, f32 textureHeight, f32 glyphWidth, f32 glyphHeight, f32 glyphSpacingX, f32 glyphSpacingY) {
    FontHandle handle = nextFontHandle++;

    FixedWidthFont font;
    font.texture = texture;
    font.textureDimensions = vector2d(textureWidth, textureHeight);
    font.glyphDimensions = vector2d(glyphWidth, glyphHeight);
    font.glyphSpacing = vector2d(glyphSpacingX, glyphSpacingY);
    MapInsertUnique(fonts, handle, font);

    return handle;
  }
}

namespace Draw {
  void Text(const zbstring &text, const vector2d &topLeft, const Font::FixedWidthFont &font, const Color &color) {
    ProfileStart("GFX::Text");
    RENDERTHREAD->SetTexture(font.texture, 0);

    transform2d transform;
    transform.Scale(font.glyphDimensions);
    transform.Translate(topLeft);

    const vector2d glyphTexCoords = componentwise_divide(font.glyphDimensions + font.glyphSpacing, font.textureDimensions);
    const vector2d glyphHalfTexCoords = componentwise_divide(font.glyphDimensions, font.textureDimensions) / 2.0f;
    const f32 glyphsPerRow = floorf(font.textureDimensions.x / (font.glyphDimensions.x + font.glyphSpacing.x));

    for (u32 i = 0; i < text.size(); ++i) {
      const char c = text[i] - 32;
      f32 yOffset = floorf(c / glyphsPerRow);
      f32 xOffset = floorf(c - yOffset * glyphsPerRow);

      aabb2d coords(componentwise_multiply(vector2d(xOffset, yOffset), glyphTexCoords) + glyphHalfTexCoords, glyphHalfTexCoords);
      TexturedQuad(transform, coords, color);
      transform.Translate(font.glyphDimensions.x, 0.0f);
    }
    ProfileStop();
  }

  void TextWorld(const zbstring &text, vector2d topLeft, FontHandle handle, const Color &color) {
    const Font::FixedWidthFont &font = MapFindGuaranteed(Font::fonts, handle);
    Text(text, topLeft + vector2d(font.glyphDimensions.x / 2.0f, -font.glyphDimensions.y / 2.0f), font, color);
  }

  void TextUI(const zbstring &text, vector2d topLeft, FontHandle handle, const Color &color) {
    const Font::FixedWidthFont &font = MapFindGuaranteed(Font::fonts, handle);
    Text(text, topLeft + vector2d(font.glyphDimensions.x / 2.0f, font.glyphDimensions.y / 2.0f), font, color);
  }
}
