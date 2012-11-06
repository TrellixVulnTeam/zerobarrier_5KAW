
bool VMRequireFile(const char *filename, VM *vm, VMError *errorsOut) {
  char *script = 0x0;
  u32 scriptSize = 0;
  bool success = false;
  if (zb_load_file(filename, (void**)&script, &scriptSize, true)) {
    VMScope vmScope(vm);
    v8::HandleScope handleScope;
    v8::TryCatch tryCatch;
    v8::Handle<v8::Object> module = vm->Require(script, filename);

    if (tryCatch.HasCaught()) {
      V8PopulateErrorMessage(tryCatch, false, errorsOut); 
    }

    zb_free_file_buffer(script);

    success = true;
  }

  return success;
}

void SandboxVM(VM *vm) {
  VMScope scope(vm);
  v8::HandleScope handleScope;

  v8::Local<v8::Object> g = vm->context->Global();
  g->Set(v8::String::New("Math"), v8::Undefined());
  g->Set(v8::String::New("Date"), v8::Undefined());
  g->Set(v8::String::New("RegExp"), v8::Undefined());
  g->Set(v8::String::New("escape"), v8::Undefined());
  g->Set(v8::String::New("unescape"), v8::Undefined());
  g->Set(v8::String::New("decodeURI"), v8::Undefined());
  g->Set(v8::String::New("decodeURIComponent"), v8::Undefined());
  g->Set(v8::String::New("encodeURI"), v8::Undefined());
  g->Set(v8::String::New("encodeURIComponent"), v8::Undefined());
}

namespace VMBindings {
  v8::Handle<v8::Value> require(const v8::Arguments &args) {
    defend_vm(args[0]->IsString());
    v8::Handle<v8::Value> exports;

    char *script = 0x0;
    u32 scriptSize = 0;
    v8::String::AsciiValue filename(args[0]);
    if (zb_load_file(*filename, (void**)&script, &scriptSize, true)) {
      VM *vm = (VM*)v8::External::Unwrap(args.This()->GetInternalField(0));
      exports = vm->Require(script, *filename);
      zb_free_file_buffer(script);
    }

    return exports;
  }

  v8::Handle<v8::Value> log(const v8::Arguments &args) {
    zb_spam(*v8::String::AsciiValue(args[0]));
    return v8::Undefined();
  }

  v8::Handle<v8::Value> collectGarbage(const v8::Arguments &args) {
    defend_vm(args[0]->IsInt32() || args[0]->IsUndefined());

    i32 hint = 1000;
    if (args[0]->IsInt32())
    {
      hint = args[0]->Int32Value(); 
    }

    while (!v8::V8::IdleNotification(hint));

    return v8::Undefined();
  }

  v8::Handle<v8::Value> processDebugMessages(const v8::Arguments &) {
    v8::Debug::ProcessDebugMessages();
    return v8::Undefined();
  }
}

void AddVMBindings(VM *vm) {
  VMScope scope(vm);
  v8::HandleScope handleScope;

  v8::Local<v8::ObjectTemplate> vmTemplate = v8::ObjectTemplate::New();
  vmTemplate->SetInternalFieldCount(1);
  vmTemplate->Set(v8::String::New("require"), v8::FunctionTemplate::New(VMBindings::require));
  vmTemplate->Set(v8::String::New("log"), v8::FunctionTemplate::New(VMBindings::log));
  vmTemplate->Set(v8::String::New("collectGarbage"), v8::FunctionTemplate::New(VMBindings::collectGarbage));
  vmTemplate->Set(v8::String::New("processDebugMessages"), v8::FunctionTemplate::New(VMBindings::processDebugMessages));

  v8::Local<v8::Object> vmObj = vmTemplate->NewInstance();
  vmObj->SetInternalField(0, v8::External::New(vm));

  vm->context->Global()->Set(v8::String::New("VM"), vmObj);
}

namespace JSONBindings {
  v8::Handle<v8::Value> readfile(const v8::Arguments &args) {
    defend_vm(args[0]->IsString());
    
    v8::Handle<v8::Value> parsedObject = v8::Null();

    char *file = 0x0;
    u32 filesize = 0;
    v8::String::AsciiValue filename(args[0]);
    if (zb_load_file(*filename, (void**)&file, &filesize, true)) {
      v8::Local<v8::Object> jsonObj = args.This();
      v8::Local<v8::Function> parse = v8::Function::Cast(*jsonObj->Get(v8::String::New("parse"))->ToObject());

      v8::Handle<v8::Value> callArgs[1];
      callArgs[0] = v8::String::New(file);
      parsedObject = parse->Call(jsonObj, 1, callArgs);

      zb_free_file_buffer(file);
    }

    return parsedObject;
  }
}

void AddJSONBindings(VM *vm) {
  VMScope scope(vm);
  v8::HandleScope handleScope;

  v8::Local<v8::Object> jsonObj = vm->context->Global()->Get(v8::String::New("JSON"))->ToObject();
  jsonObj->Set(v8::String::New("readfile"), v8::FunctionTemplate::New(JSONBindings::readfile)->GetFunction()); 
}
