#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

struct ViewParameters {
  ViewParameters(void)
    : view(0)
    , width(-1)
    , height(-1)
    , windowed(false) {
      // Do nothing.
  }

  HWND view;
  i32 width;
  i32 height;
  bool windowed;
};

struct DefaultVertex {
  vector2d position;
  u32 color;
  vector2d tex0;
  vector2d tex1;
};

namespace PrimitiveType {
  enum PrimitiveType {
    Point,
    Line,
    Triangle,
    Quad,
  };
}

typedef i32 TextureHandle;

class RenderThread : public ThreadContext {
public:
  RenderThread()
  : renderCommandReader(0x0)
  , renderCommandWriter(0x0)
  , resourceCommandReader(0x0)
  , resourceCommandWriter(0x0)
  , vertexReader(0x0)
  , vertexWriter(0x0)
  , d3d(0x0)
  , device(0x0)
  , defaultVertexDeclaration(0x0)
  , defaultEffect(0x0)
  , hasCommands(false)
  , currentEffect(0x0)
  , currentEffectHasChanges(true)
  , nextTextureHandle(1)
  , currentTechniqueIndex(-1) {
      renderCommandReader = &commandBuffers[0];
      renderCommandWriter = &commandBuffers[1];
      resourceCommandReader = &resourceBuffers[0];
      resourceCommandWriter = &resourceBuffers[1];
      vertexReader = &vertexBuffers[0];
      vertexWriter = &vertexBuffers[1];
  }

  // All public functions are main-thread safe.
  void Initialize(const ViewParameters &vp);
  void Shutdown(void);

  void Clear(const aabb2d &region, const Color &color);
  void Clear(const Camera2d& camera, const Color& color);

  void SetCamera(const Camera2d &camera);
  void SetTexture(TextureHandle texture, i32 index);
  void SetTechnique(i32 techniqueIndex);

  void Draw(PrimitiveType::PrimitiveType primitiveType, const zbvector(DefaultVertex) &verts);
  void Draw(PrimitiveType::PrimitiveType primitiveType, const DefaultVertex *verts, i32 vertCount);

  void Present(void);

  i32 WindowWidth(void) const;
  i32 WindowHeight(void) const;

  TextureHandle LoadTexture(const void *data, i32 bytes);
  TextureHandle LoadTextureFromFile(const std::string &filename);
  void UnloadTexture(TextureHandle texture);

private:
  struct Texture {
    Texture(void)
    : texture(0x0)
    , width(0.0f)
    , height(0.0f) {
      // Do nothing.
    }

    IDirect3DTexture9 *texture;
    f32 width;
    f32 height;

    // This allows non power of two textures to render properly, even when they're padded to power of two textures on the card.
    vector2d virtualCoords;
  };

  u32 main(void);

  void initialize(void);
  void shutdown(void);
  void createDefaultResources(void);
  void setDefaultState(void);
  void processResourceCommands(void);
  void processRenderCommands(void);

  void lockThread(void);
  void unlockThread(void);

  void clear(const aabb2d &region, const Color &color);
  void setCamera(const Camera2d &camera);
  void setTexture(const Texture &texture, i32 index);
  void setTechnique(i32 techniqueIndex);
  void draw(PrimitiveType::PrimitiveType primitiveType, i32 count, i32 startVertex, i32 effectPasses);
  void flushBatch(PrimitiveType::PrimitiveType primitiveType, i32 *primitiveCountInOut, i32 *startVertexInOut, i32 effectPasses);
  void loadTexture(TextureHandle handle, const void *data, i32 bytes);
  void loadTextureFromFile(TextureHandle handle, const char *filename);
  void unloadTexture(TextureHandle texture);

  void addD3DTexture(IDirect3DTexture9 *texture, const D3DXIMAGE_INFO *info, TextureHandle handle);

  // Thread communication
  WordAlignedBuffer commandBuffers[2];
  WordAlignedBuffer *renderCommandReader;
  WordAlignedBuffer *renderCommandWriter;

  WordAlignedBuffer resourceBuffers[2];
  WordAlignedBuffer *resourceCommandReader;
  WordAlignedBuffer *resourceCommandWriter;

  zbvector(u8) vertexBuffers[2];
  zbvector(u8) *vertexReader;
  zbvector(u8) *vertexWriter;

  HANDLE mutex;
  bool hasCommands;

  // D3D
  IDirect3D9 *d3d;
  IDirect3DDevice9 *device;

  ViewParameters viewParameters;
  D3DPRESENT_PARAMETERS d3dParameters;

  IDirect3DVertexDeclaration9 *defaultVertexDeclaration;
  ID3DXEffect *defaultEffect;
  IDirect3DVertexBuffer9 *defaultVertexBuffer;

  // Graphics State
  Camera2d camera;

  ID3DXEffect *currentEffect;
  i32 currentTechniqueIndex;
  bool currentEffectHasChanges;

  // Textures
  TextureHandle nextTextureHandle;  // This variable is an exception! Accessed only on main thread.
  zbmap(TextureHandle, Texture) textures;
  Texture currentTextures[2];
};

extern RenderThread *RENDERTHREAD;

namespace Draw {
  void Point(const vector2d &a, const Color &color = Color::Black);
  void Line(const vector2d &a, const vector2d &b, const Color &color = Color::Black);
  void ThickLine(const vector2d &a, const vector2d &b, f32 thickness = 2.0f, const Color &color = Color::Black);
  void Arrow(const vector2d &start, const vector2d &end, const Color &color = Color::Black, f32 arrowLength = 8.0f, degrees arrowAngle = 30.0f);
  void Triangle(vector2d a, vector2d b, vector2d c, const Color &color = Color::Black, bool fill = false);
  void Rect(const aabb2d &rect, const Color &color = Color::Black, bool fill = false);
  void Ngon(const vector2d &center, f32 radius, i32 n, const Color &color = Color::Black, radians orientation = RadPi2, bool fill = false);
  void TexturedQuad(const transform2d &transform, const aabb2d &tx0, const Color &color = Color::White, const aabb2d &tx1 = aabb2d());
}

ZB_NAMESPACE_END
