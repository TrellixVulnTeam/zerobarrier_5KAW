
namespace OSBindings {
  v8::Handle<v8::Value> tick(const v8::Arguments &args) {
    OS *os = (OS*)v8::External::Unwrap(args.This()->GetInternalField(0));
    os->Tick();

    return v8::Undefined();
  }

  v8::Handle<v8::Value> hasQuit(const v8::Arguments &args) {
    OS *os = (OS*)v8::External::Unwrap(args.This()->GetInternalField(0));
    return v8::Boolean::New(os->HasQuit());
  }

  v8::Handle<v8::Value> showCursor(const v8::Arguments &args) {
    defend_vm(args[0]->IsBoolean());
    ShowCursor(args[0]->BooleanValue());
    return v8::Undefined();
  }
}

void AddOSBindings(VM *vm, OS *os) {
  VMScope scope(vm);
  v8::HandleScope handleScope;

  v8::Local<v8::ObjectTemplate> osTemplate = v8::ObjectTemplate::New();
  osTemplate->SetInternalFieldCount(1);
  osTemplate->Set(v8::String::New("tick"), v8::FunctionTemplate::New(OSBindings::tick));
  osTemplate->Set(v8::String::New("hasQuit"), v8::FunctionTemplate::New(OSBindings::hasQuit));
  osTemplate->Set(v8::String::New("showCursor"), v8::FunctionTemplate::New(OSBindings::showCursor));

  v8::Local<v8::Object> osObj = osTemplate->NewInstance();
  osObj->SetInternalField(0, v8::External::New(os));

  vm->context->Global()->Set(v8::String::New("OS"), osObj);
}
