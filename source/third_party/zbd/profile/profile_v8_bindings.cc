namespace ProfileBindings {
  v8::Handle<v8::Value> tick(const v8::Arguments &) {
    ProfileFrameStart();
    return v8::Undefined();
  }

  v8::Handle<v8::Value> start(const v8::Arguments &args) {
    defend_vm(args.Length() == 1);

    zbstring name;
    verify_vm(ReadString(args, 0, &name));

    ProfileStartStoreStrings(name.c_str());
    return v8::Undefined();
  }

  v8::Handle<v8::Value> stop(const v8::Arguments &) {
    ProfileStop();
    return v8::Undefined();
  }

#ifndef ZB_DISABLE_PROFILE_VISUALIZER
  v8::Handle<v8::Value> draw(const v8::Arguments &args) {
    defend_vm (args.Length() == 1);
    Camera2d *camera = VMGetObject<Camera2d>(args[0], GraphicsBindings::Camera2dClassId);
    defend_vm(camera != 0x0);

    ProfileVisualizerDraw(*camera);
    return v8::Undefined();
  }
#endif
}

void AddProfileBindings(VM *vm) {
  VMScope scope(vm);
  v8::HandleScope handleScope;

  v8::Local<v8::ObjectTemplate> profileTemplate = v8::ObjectTemplate::New();
  profileTemplate->Set(v8::String::New("tick"), v8::FunctionTemplate::New(ProfileBindings::tick));
  profileTemplate->Set(v8::String::New("start"), v8::FunctionTemplate::New(ProfileBindings::start));
  profileTemplate->Set(v8::String::New("stop"), v8::FunctionTemplate::New(ProfileBindings::stop));

#ifndef ZB_DISABLE_PROFILE_VISUALIZER
  profileTemplate->Set(v8::String::New("draw"), v8::FunctionTemplate::New(ProfileBindings::draw));
#endif

  v8::Local<v8::Object> profileObj = profileTemplate->NewInstance();
  vm->context->Global()->Set(v8::String::New("PROF"), profileObj);
}
