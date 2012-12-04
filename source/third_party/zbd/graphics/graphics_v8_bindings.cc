
namespace {
  bool ReadAABB2d(const v8::Arguments &args, i32 index, aabb2d *aabbOut) {
    if (index + 3 < args.Length() && args[index]->IsNumber() && args[index+1]->IsNumber() && args[index+2]->IsNumber() && args[index+3]->IsNumber()) {
      *aabbOut = aabb2d((f32)args[index]->NumberValue(), (f32)args[index+1]->NumberValue(), (f32)args[index+2]->NumberValue(), (f32)args[index+3]->NumberValue());
      return true;
    }

    return false;
  }

  bool ReadVector2d(const v8::Arguments &args, i32 index, vector2d *vectorOut) {
    if (index + 1 < args.Length() && args[index]->IsNumber() && args[index+1]->IsNumber()) {
      *vectorOut = vector2d((f32)args[index]->NumberValue(), (f32)args[index+1]->NumberValue());
      return true;
    }

    return false;
  }

  bool ReadVector3d(const v8::Arguments &args, i32 index, vector3d *vectorOut) {
    if (index + 2 < args.Length() && args[index]->IsNumber() && args[index+1]->IsNumber() && args[index+2]->IsNumber()) {
      *vectorOut = vector3d((f32)args[index]->NumberValue(), (f32)args[index+1]->NumberValue(), (f32)args[index+2]->NumberValue());
      return true;
    }

    return false;
  }

  bool ReadColor(const v8::Arguments &args, i32 index, Color *colorOut) {
    if (index + 3 < args.Length() && args[index]->IsNumber() && args[index+1]->IsNumber() && args[index+2]->IsNumber() && args[index+3]->IsNumber()) {
      *colorOut = Color((f32)args[index]->NumberValue(), (f32)args[index+1]->NumberValue(), (f32)args[index+2]->NumberValue(), (f32)args[index+3]->NumberValue());
      return true;
    }

    return false;
  }
}

namespace GraphicsBindings {
  v8::Persistent<v8::ObjectTemplate> Camera2dTemplate;
  const string_hash Camera2dClassId = vmhashclass(Camera2d);

  v8::Handle<v8::Value> tick(const v8::Arguments &) {
    RENDERTHREAD->Present();
    return v8::Undefined();
  }

  v8::Handle<v8::Value> clear(const v8::Arguments &args) {
    defend_vm(args.Length() == 5);
    Camera2d *camera = VMGetObject<Camera2d>(args[0], Camera2dClassId);
    defend_vm(camera != 0x0);

    Color color;
    verify_vm(ReadColor(args, 1, &color));

    RENDERTHREAD->Clear(*camera, color);
    return v8::Undefined();
  }

  v8::Handle<v8::Value> point(const v8::Arguments &args) {
    defend_vm(args.Length() == 6);
    
    vector2d point;
    verify_vm(ReadVector2d(args, 0, &point));

    Color color;
    verify_vm(ReadColor(args, 2, &color));

    Draw::Point(point, color);
    return v8::Undefined();
  }

  v8::Handle<v8::Value> line(const v8::Arguments &args) {
    defend_vm(args.Length() == 8);
    
    vector2d start;
    verify_vm(ReadVector2d(args, 0, &start));

    vector2d end;
    verify_vm(ReadVector2d(args, 2, &end));

    Color color;
    verify_vm(ReadColor(args, 4, &color));

    Draw::Line(start, end, color);
    return v8::Undefined();
  }

  v8::Handle<v8::Value> arrow(const v8::Arguments &args) {
    defend_vm(args.Length() == 10);
    
    vector2d start;
    verify_vm(ReadVector2d(args, 0, &start));

    vector2d end;
    verify_vm(ReadVector2d(args, 2, &end));

    Color color;
    verify_vm(ReadColor(args, 4, &color));

    f32 arrowLength = 0.0f;
    verify_vm(ReadFloat(args, 8, &arrowLength));

    f32 arrowAngle = 0.0f;
    verify_vm(ReadFloat(args, 9, &arrowAngle));

    Draw::Arrow(start, end, color, arrowLength, arrowAngle);
    return v8::Undefined();
  }

  v8::Handle<v8::Value> triangle(const v8::Arguments &args) {
    defend_vm(args.Length() == 11);
    
    vector2d a;
    verify_vm(ReadVector2d(args, 0, &a));

    vector2d b;
    verify_vm(ReadVector2d(args, 2, &b));

    vector2d c;
    verify_vm(ReadVector2d(args, 4, &c));

    Color color;
    verify_vm(ReadColor(args, 6, &color));

    bool fill = false;
    verify_vm(ReadBool(args, 10, &fill));

    Draw::Triangle(a, b, c, color, fill);
    return v8::Undefined();
  }

  v8::Handle<v8::Value> rect(const v8::Arguments &args) {
    defend_vm(args.Length() == 9);
    
    aabb2d rect;
    verify_vm(ReadAABB2d(args, 0, &rect));

    Color color;
    verify_vm(ReadColor(args, 4, &color));

    bool fill = false;
    verify_vm(ReadBool(args, 8, &fill));

    Draw::Rect(rect, color, fill);
    return v8::Undefined();
  }

  v8::Handle<v8::Value> ngon(const v8::Arguments &args) {
    defend_vm(args.Length() == 10);
    
    vector2d center;
    verify_vm(ReadVector2d(args, 0, &center));

    f32 radius = 0.0f;
    verify_vm(ReadFloat(args, 2, &radius));

    i32 n = 0;
    verify_vm(ReadI32(args, 3, &n));

    Color color;
    verify_vm(ReadColor(args, 4, &color));

    f32 orientation = 0.0f;
    verify_vm(ReadFloat(args, 8, &orientation));

    bool fill = false;
    verify_vm(ReadBool(args, 9, &fill));

    Draw::Ngon(center, radius, n, color, orientation, fill);
    return v8::Undefined();
  }

  v8::Handle<v8::Value> windowWidth(const v8::Arguments &) {
    return v8::Int32::New(RENDERTHREAD->WindowWidth());
  }

  v8::Handle<v8::Value> windowHeight(const v8::Arguments &) {
    return v8::Int32::New(RENDERTHREAD->WindowHeight());
  }

  v8::Handle<v8::Value> setCamera(const v8::Arguments &args) {
    defend_vm(args.Length() == 1);
    Camera2d *camera = VMGetObject<Camera2d>(args[0], Camera2dClassId);
    defend_vm(camera != 0x0);
    RENDERTHREAD->SetCamera(*camera);
    return v8::Undefined();
  }

  // TODO: Use a JS constructor for this?
  v8::Handle<v8::Value> NewCamera(const v8::Arguments &) {
    return AllocateVMObject(Camera2dTemplate, vmnew("VM Camera") Camera2d());
  }


  // Camera2d
  v8::Handle<v8::Value> camera2d_initialize(const v8::Arguments &args) {
    defend_vm(args.Length() == 6);

    aabb2d viewport;
    verify_vm(ReadAABB2d(args, 0, &viewport));
    defend_vm(viewport.width() > 0);
    defend_vm(viewport.height() > 0);

    f32 minDepth = 0.0f;
    verify_vm(ReadFloat(args, 4, &minDepth));

    f32 maxDepth = 0.0f;
    verify_vm(ReadFloat(args, 5, &maxDepth));
    defend_vm(minDepth <= maxDepth);

    Camera2d *camera = VMGetObject<Camera2d>(args.This(), Camera2dClassId);
    defend_vm(camera != 0x0);

    camera->Initialize(viewport, minDepth, maxDepth);

    return v8::Undefined();
  }

  v8::Handle<v8::Value> camera2d_setLocation(const v8::Arguments &args) {
    defend_vm(args.Length() == 2 || args.Length() == 3);
    Camera2d *camera = VMGetObject<Camera2d>(args.This(), Camera2dClassId);
    defend_vm(camera != 0x0);

    if (args.Length() == 3) {
      vector3d location;
      verify_vm(ReadVector3d(args, 0, &location));
      camera->SetLocation(location);
    }

    if (args.Length() == 2) {
      vector2d location;
      verify_vm(ReadVector2d(args, 0, &location));
      camera->SetLocation(location);
    }

    return v8::Undefined();
  }

}

void AddGraphicsBindings(VM *vm) {
  VMScope scope(vm);
  v8::HandleScope handleScope;

  GraphicsBindings::Camera2dTemplate = VMCreateObjectTemplate(GraphicsBindings::Camera2dClassId);
  GraphicsBindings::Camera2dTemplate->Set(v8::String::New("initialize"), v8::FunctionTemplate::New(GraphicsBindings::camera2d_initialize));
  GraphicsBindings::Camera2dTemplate->Set(v8::String::New("setLocation"), v8::FunctionTemplate::New(GraphicsBindings::camera2d_setLocation));

  v8::Local<v8::ObjectTemplate> graphicsTemplate = v8::ObjectTemplate::New();
  graphicsTemplate->Set(v8::String::New("tick"), v8::FunctionTemplate::New(GraphicsBindings::tick));
  graphicsTemplate->Set(v8::String::New("clear"), v8::FunctionTemplate::New(GraphicsBindings::clear));
  graphicsTemplate->Set(v8::String::New("point"), v8::FunctionTemplate::New(GraphicsBindings::point));
  graphicsTemplate->Set(v8::String::New("line"), v8::FunctionTemplate::New(GraphicsBindings::line));
  graphicsTemplate->Set(v8::String::New("arrow"), v8::FunctionTemplate::New(GraphicsBindings::arrow));
  graphicsTemplate->Set(v8::String::New("triangle"), v8::FunctionTemplate::New(GraphicsBindings::triangle));
  graphicsTemplate->Set(v8::String::New("rect"), v8::FunctionTemplate::New(GraphicsBindings::rect));
  graphicsTemplate->Set(v8::String::New("ngon"), v8::FunctionTemplate::New(GraphicsBindings::ngon));
  graphicsTemplate->Set(v8::String::New("windowWidth"), v8::FunctionTemplate::New(GraphicsBindings::windowWidth));
  graphicsTemplate->Set(v8::String::New("windowHeight"), v8::FunctionTemplate::New(GraphicsBindings::windowHeight));
  graphicsTemplate->Set(v8::String::New("NewCamera"), v8::FunctionTemplate::New(GraphicsBindings::NewCamera));
  graphicsTemplate->Set(v8::String::New("setCamera"), v8::FunctionTemplate::New(GraphicsBindings::setCamera));
  v8::Local<v8::Object> graphicsObj = graphicsTemplate->NewInstance();
  vm->context->Global()->Set(v8::String::New("GFX"), graphicsObj);
}
