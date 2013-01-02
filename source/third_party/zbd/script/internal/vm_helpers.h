#pragma once

bool VMRequireFile(const char *filename, VM *vm, VMError *errorsOut);
void AddVMBindings(VM *vm);
void SandboxVM(VM *vm);
void AddJSONBindings(VM *vm);

#define vmnew(...) znew(__VA_ARGS__)
#define vmdelete(...) zdelete(__VA_ARGS__)

#define vmhashclass(type) zb_hash_string(#type)

ZB_INLINE v8::Persistent<v8::ObjectTemplate> VMCreateObjectTemplate(string_hash classId) {
  v8::Persistent<v8::ObjectTemplate> objTemplate = v8::Persistent<v8::ObjectTemplate>::New(v8::ObjectTemplate::New());
  objTemplate->SetInternalFieldCount(2);
  objTemplate->Set(v8::String::New("cppHash"), v8::Int32::New(classId), v8::PropertyAttribute(v8::DontDelete | v8::ReadOnly | v8::DontEnum));
  return objTemplate;
}

ZB_INLINE bool VMObjectIsA(v8::Handle<v8::Object> obj, string_hash classId) {
  return (string_hash)v8::External::Unwrap(obj->GetInternalField(1)) == classId;
}

template <typename T> T* VMGetObject(v8::Handle<v8::Object> obj) {
  return (T*)v8::External::Unwrap(obj->GetInternalField(0));
}

template <typename T> T* VMGetObject(v8::Handle<v8::Value> v, string_hash classId) {
  return (v->IsObject() && VMObjectIsA(v->ToObject(), classId)) ? VMGetObject<T>(v->ToObject()) : 0x0;
}

template <typename T> v8::Handle<v8::Object> AllocateVMObject(v8::Handle<v8::ObjectTemplate> obj, T *data) {
  v8::Handle<v8::Object> objHandle = obj->NewInstance();
  objHandle->SetInternalField(0, v8::External::New(data));
  objHandle->SetInternalField(1, v8::External::New((void*)objHandle->Get(v8::String::New("cppHash"))->Int32Value()));

  v8::Persistent<v8::Object> persistentHandle = v8::Persistent<v8::Object>::New(objHandle);
  persistentHandle.MakeWeak(data, &DeallocateVMObject<T>);

  return objHandle;
}

template <typename T> void DeallocateVMObject(v8::Persistent<v8::Value> v, void* p) {
  T *castP = (T*)p;
  vmdelete(castP);
  v.Dispose();
}

ZB_INLINE bool ReadI32(const v8::Arguments& args, i32 index, i32 *iOut) {
  if (index < args.Length() && args[index]->IsNumber()) {
    *iOut = args[index]->Int32Value();
    return true;
  }

  return false;
}

ZB_INLINE bool ReadF32(const v8::Arguments& args, i32 index, f32 *fOut) {
  if (index < args.Length() && args[index]->IsNumber()) {
    *fOut = (f32)args[index]->NumberValue();
    return true;
  }

  return false;
}

ZB_INLINE bool ReadF64(const v8::Arguments& args, i32 index, f64 *fOut) {
  if (index < args.Length() && args[index]->IsNumber()) {
    *fOut = args[index]->NumberValue();
    return true;
  }

  return false;
}

ZB_INLINE bool ReadBool(const v8::Arguments& args, i32 index, bool *bOut) {
  if (index < args.Length() && args[index]->IsBoolean()) {
    *bOut = args[index]->BooleanValue();
    return true;
  }

  return false;
}

ZB_INLINE bool ReadString(const v8::Arguments& args, i32 index, zbstring *sOut) {
  if (index < args.Length()) {
    *sOut = *v8::String::AsciiValue(args[index]);
    return true;
  }

  return false;
}
