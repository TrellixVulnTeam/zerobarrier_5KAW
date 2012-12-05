
void VMError::Reset(void) {
  hasSyntaxError = false;
  hasExecutionError = false;
  isRecoverable = false;
  file.clear();
  message.clear();
  exception.clear();
  callstack.clear();
  line = -1;
  column = -1;
}

VM::VM(void)
: isolate(0x0) {
  // Do nothing.
}

void VM::Initialize(void) {
  Shutdown();

  isolate = v8::Isolate::New();
  v8::Isolate::Scope isoScope(isolate);
  v8::V8::SetCaptureStackTraceForUncaughtExceptions(true);
  
  // Exposing this changes the behavior of v8::V8::IdleNotification
  // in addition to adding 'gc' to the global javascript object.
  // The behavior change seems to affect being able to force
  // a garbage collect by calling v8::V8::IdleNotification.
  const char *flags = "--expose-gc";
  v8::V8::SetFlagsFromString(flags, strlen(flags));

  context = v8::Context::New();
}

void VM::Reset(void) {
  Initialize();
}

void VM::Shutdown(void) {
  if (isolate) {
    isolate->Enter();
    //DisableDebugging();
  }

  if (!returnValue.IsEmpty()) {
    returnValue.Dispose();
    returnValue.Clear();
  }
  
  // This is a bit of a hack.
  // The context isn't being garbage collected on shutdown (god knows why)
  // It's possible that it's a bug on my end, with reference counting or something.
  // Until that's sorted, go through and nuke all the objects in the context so that
  // at least all the c++ objects get free'd.
  if (!context.IsEmpty()) {
    v8::Context::Scope contextScope(context);
    v8::HandleScope handleScope;

    v8::Local<v8::Object> global = context->Global();

    // Grab the 'gc' object. Do this because the next couple lines nuke
    // all references from the global object.
    v8::Local<v8::Object> gc = context->Global()->Get(v8::String::New("gc"))->ToObject();

    v8::Local<v8::Array> names = global->GetPropertyNames();
    u32 propCount = names->Length();
    for (u32 i = 0; i < propCount; ++i) {
      v8::Local<v8::Value> key = names->Get(i);  
      global->Set(key, v8::Null());
    }

    // Call the gc, a bunch.
    for (int i = 0; i < 25; ++i)
      v8::Handle<v8::Function>::Cast(gc)->Call(gc, 0, 0x0);

    context.Dispose();
    context.Clear();
  }

  if (isolate) {
    isolate->Exit();
    isolate->Dispose();
    isolate = 0x0;
  }
}

bool VM::Execute(const char *script, const char *name, VMError *errorOut) {
  v8::Isolate::Scope isoScope(isolate);
  v8::Context::Scope contextScope(context);

  v8::TryCatch tryCatch;
  if (!returnValue.IsEmpty()) {
    returnValue.Dispose();
    returnValue.Clear();
  }

  bool syntaxError = false;
  v8::HandleScope handleScope;
  v8::Handle<v8::String> source = v8::String::New(script);
  v8::Handle<v8::Script> compiledScript = v8::Script::Compile(source, v8::String::New(name));

  if (tryCatch.HasCaught()) {
    syntaxError = true;
  }

  if (!compiledScript.IsEmpty()) {
    v8::Local<v8::Value> retVal = compiledScript->Run();
    if (!retVal.IsEmpty()) {
      returnValue = v8::Persistent<v8::Value>::New(retVal);
    }
  }

  if (tryCatch.HasCaught()) {
    V8PopulateErrorMessage(tryCatch, syntaxError, errorOut);
  }

  return !tryCatch.HasCaught();
}

v8::Handle<v8::Object> VM::Require(const char *script, const char *name) {
  v8::Local<v8::Object> global = context->Global();

  v8::Local<v8::Value> oldGlobalObj = global->Get(v8::String::New("global"));
  v8::Local<v8::Value> oldModuleObj = global->Get(v8::String::New("module"));

  v8::Local<v8::Object> newGlobalObj = v8::Object::New();
  v8::Local<v8::Object> newModuleObj = v8::Object::New();

  global->Set(v8::String::New("global"), newGlobalObj);
  global->Set(v8::String::New("module"), newModuleObj);
  newModuleObj->Set(v8::String::New("exports"), v8::Object::New());
  
  // Call script
  v8::Local<v8::String> source = v8::String::New(script);
  v8::Local<v8::Script> compiledScript = v8::Script::Compile(source, v8::String::New(name));

  if (!compiledScript.IsEmpty()) {
    compiledScript->Run();
  }

  v8::Local<v8::Array> names = newGlobalObj->GetOwnPropertyNames();
  u32 propCount = names->Length();
  for (u32 i = 0; i < propCount; ++i) {
    v8::Local<v8::Value> key = names->Get(i);
    global->Set(key, newGlobalObj->Get(key));
  }

  v8::Local<v8::Value> exports = newModuleObj->Get(v8::String::New("exports"));
  if (!oldGlobalObj.IsEmpty()) {
    global->Set(v8::String::New("global"), oldGlobalObj);
    global->Set(v8::String::New("module"), oldModuleObj);
  }
  
  defend(!exports.IsEmpty() && exports->IsObject());
  return exports->ToObject();
}

bool VM::Call(const char *globalFunctionName, int argc, v8::Handle<v8::Value> argv[], VMError *errorOut) {
  v8::TryCatch tryCatch;
  if (!returnValue.IsEmpty()) {
    returnValue.Dispose();
    returnValue.Clear();
  }

  v8::Local<v8::Object> function = context->Global()->Get(v8::String::New(globalFunctionName))->ToObject();
  const bool validFunction = !function.IsEmpty() && function->IsFunction() && !tryCatch.HasCaught();
  if (validFunction)
  {
    v8::Local<v8::Value> retVal = v8::Handle<v8::Function>::Cast(function)->Call(function, argc, argv);
    if (!retVal.IsEmpty()) {
      returnValue = v8::Persistent<v8::Value>::New(retVal);
    }
  }

  if (tryCatch.HasCaught()) {
    V8PopulateErrorMessage(tryCatch, false, errorOut);
  }

  return validFunction;
}

bool VM::Call(const char *globalFunctionName, VMError *errorOut) {
  v8::Isolate::Scope isoScope(isolate);
  v8::Context::Scope contextScope(context);
  v8::HandleScope handleScope;

  return Call(globalFunctionName, 0, 0x0, errorOut);
}

zbstring VM::ReturnValueToString(void) {
  v8::Isolate::Scope isoScope(isolate);
  v8::Context::Scope contextScope(context);
  return !returnValue.IsEmpty() ? *v8::String::AsciiValue(returnValue) : "";
}

void VM::EnableDebugging(const char *name, i32 port) {
  v8::Isolate::Scope isoScope(isolate);
  v8::Context::Scope contextScope(context);
  v8::Debug::EnableAgent(name, port, false /*true doesn't work for whatever reason...*/);
}

void VM::DisableDebugging(void) {
  v8::Isolate::Scope isoScope(isolate);
  v8::Context::Scope contextScope(context);
  v8::Debug::DisableAgent();
}

void VM::Enter(void) {
  isolate->Enter();
  context->Enter();
}

void VM::Exit(void) {
  context->Exit();
  isolate->Exit();
}

void VM::TryCollectGarbage(i32 hint) {
  while (!v8::V8::IdleNotification(hint));
}

void V8StackTraceToString(v8::Handle<v8::StackTrace> trace, zbstring &stringOut) {
  i32 frameCount = trace->GetFrameCount();
  for (i32 i = 0; i < frameCount; ++i) {
    if (i) {
      stringOut += "\n";
    }

    v8::Handle<v8::StackFrame> stackFrame = trace->GetFrame(i);
    const zbstring functionName = *v8::String::AsciiValue(stackFrame->GetFunctionName());
    stringOut += functionName == "" ? "GLOBALSCOPE" : functionName;
    stringOut += "(...) [";

    v8::Handle<v8::String> filename = stackFrame->GetScriptName();
    stringOut += !filename.IsEmpty() ? *v8::String::AsciiValue(filename) : "?";

    char lineNumber[33] = {0};
    _itoa(stackFrame->GetLineNumber(), lineNumber, 10);
    stringOut += ":";
    stringOut += lineNumber;
    stringOut += "]";
  }
}

void V8PopulateErrorMessage(const v8::TryCatch &tryCatch, bool syntaxError, VMError *errorOut) {
  if (errorOut != 0x0) {
    errorOut->hasSyntaxError = syntaxError;
    errorOut->hasExecutionError = !syntaxError;

    errorOut->isRecoverable = tryCatch.CanContinue();
    v8::Handle<v8::Message> message = tryCatch.Message();
    errorOut->file = *v8::String::AsciiValue(message->GetScriptResourceName());
    errorOut->message = *v8::String::AsciiValue(message->Get());
    errorOut->exception = *v8::String::AsciiValue(tryCatch.Exception());
    V8StackTraceToString(message->GetStackTrace(), errorOut->callstack);
    errorOut->line = message->GetLineNumber();
    errorOut->column = message->GetStartColumn();
  }
}
