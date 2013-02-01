
namespace TimeBindings {
  v8::Handle<v8::Value> stamp(const v8::Arguments &) {
    TimeStamp stamp = zb_time::Time();
    defend ((seconds)stamp < std::numeric_limits<seconds>::max());
    defend ((seconds)stamp > std::numeric_limits<seconds>::min());
    return v8::Number::New((seconds)stamp);
  }

  v8::Handle<v8::Value> diff(const v8::Arguments &args) {
    defend_vm (args.Length() == 2);

    seconds old = 0.0;
    verify_vm (ReadF64(args, 0, &old));

    seconds current = 0.0;
    verify_vm (ReadF64(args, 1, &current));

    return v8::Number::New(zb_time::TimeDifference(old, current));
  }
}

void AddTimeBindings(VM *vm) {
  VMScope scope(vm);
  v8::HandleScope handleScope;

  v8::Local<v8::ObjectTemplate> timeTemplate = v8::ObjectTemplate::New();
  timeTemplate->Set(v8::String::New("stamp"), v8::FunctionTemplate::New(TimeBindings::stamp));
  timeTemplate->Set(v8::String::New("diff"), v8::FunctionTemplate::New(TimeBindings::diff));

  v8::Local<v8::Object> timeObj = timeTemplate->NewInstance();
  vm->context->Global()->Set(v8::String::New("TIME"), timeObj);
}
