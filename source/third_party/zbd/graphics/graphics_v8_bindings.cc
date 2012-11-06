
namespace GraphicsBindings {
  v8::Handle<v8::Value> tick(const v8::Arguments &args) {
    Graphics *graphics = (Graphics*)v8::External::Unwrap(args.This()->GetInternalField(0));
    graphics->Tick();
    return v8::Undefined();
  }
}

void AddGraphicsBindings(VM *vm, Graphics *graphics) {
  VMScope scope(vm);
  v8::HandleScope handleScope;

  v8::Local<v8::ObjectTemplate> graphicsTemplate = v8::ObjectTemplate::New();
  graphicsTemplate->SetInternalFieldCount(1);
  graphicsTemplate->Set(v8::String::New("tick"), v8::FunctionTemplate::New(GraphicsBindings::tick));

  v8::Local<v8::Object> graphicsObj = graphicsTemplate->NewInstance();
  graphicsObj->SetInternalField(0, v8::External::New(graphics));

  vm->context->Global()->Set(v8::String::New("Graphics"), graphicsObj);
}
