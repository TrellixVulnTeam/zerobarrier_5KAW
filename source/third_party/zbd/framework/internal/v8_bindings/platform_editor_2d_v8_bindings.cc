namespace PlatformPhysics2dBindings {
  v8::Persistent<v8::ObjectTemplate> PlatformEditor2dTemplate;
  const string_hash PlatformEditor2dClassId = vmhashclass(PlatformPhysics2dEditor);

  v8::Handle<v8::Value> tick(const v8::Arguments &args) {
    PlatformPhysics2dEditor *editor = VMGetObject<PlatformPhysics2dEditor>(args.This(), PlatformEditor2dClassId);
    defend_vm(editor != 0x0);

    editor->Tick();
    return v8::Undefined();
  }

  v8::Handle<v8::Value> draw(const v8::Arguments &args) {
    PlatformPhysics2dEditor *editor = VMGetObject<PlatformPhysics2dEditor>(args.This(), PlatformEditor2dClassId);
    defend_vm(editor != 0x0);

    editor->Draw();
    return v8::Undefined();
  }

  v8::Handle<v8::Value> initialize(const v8::Arguments &args) {
    defend_vm(args.Length() == 1);
    Camera2d *camera = VMGetObject<Camera2d>(args[0], GraphicsBindings::Camera2dClassId);
    defend_vm(camera != 0x0);

    PlatformPhysics2dEditor *editor = VMGetObject<PlatformPhysics2dEditor>(args.This(), PlatformEditor2dClassId);
    defend_vm(editor != 0x0);

    editor->Init(camera);

    return v8::Undefined();
  }

    // TODO: Use a JS constructor for this?
  v8::Handle<v8::Value> NewEditor(const v8::Arguments &) {
    return AllocateVMObject(PlatformEditor2dTemplate, vmnew("VM PlatformPhysics2dEditor") PlatformPhysics2dEditor());
  }
}

void AddPhysicsEditor2dBindings(VM *vm) {
  VMScope scope(vm);
  v8::HandleScope handleScope;

  PlatformPhysics2dBindings::PlatformEditor2dTemplate = VMCreateObjectTemplate(PlatformPhysics2dBindings::PlatformEditor2dClassId);
  PlatformPhysics2dBindings::PlatformEditor2dTemplate->Set(v8::String::New("initialize"), v8::FunctionTemplate::New(PlatformPhysics2dBindings::initialize));
  PlatformPhysics2dBindings::PlatformEditor2dTemplate->Set(v8::String::New("tick"), v8::FunctionTemplate::New(PlatformPhysics2dBindings::tick));
  PlatformPhysics2dBindings::PlatformEditor2dTemplate->Set(v8::String::New("draw"), v8::FunctionTemplate::New(PlatformPhysics2dBindings::draw));

  v8::Local<v8::ObjectTemplate> physicsTemplate = v8::ObjectTemplate::New();
  physicsTemplate->Set(v8::String::New("NewEditor"), v8::FunctionTemplate::New(PlatformPhysics2dBindings::NewEditor));
  v8::Local<v8::Object> physicsObject = physicsTemplate->NewInstance();
  vm->context->Global()->Set(v8::String::New("PHYSICS"), physicsObject);
}
