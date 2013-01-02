#pragma once

struct VMError {
  VMError(void) { Reset(); }
  void Reset(void);

  bool HasError(void) { return hasSyntaxError || hasExecutionError; }

  // Data.
  bool hasSyntaxError;
  bool hasExecutionError;
  bool isRecoverable;
  zbstring file;
  zbstring message;
  zbstring exception;
  zbstring callstack;
  i32 line;
  i32 column;
};

class VM {
public:
  VM(void);

  void Initialize(void);
  void Reset(void);
  void Shutdown(void);

  bool Execute(const char *script, const char *name, VMError *errorOut);
  bool Call(const char *globalFunctionName, i32 argc, v8::Handle<v8::Value> argv[], VMError *errorOut); // Must be inside a VMScope and v8::HandleScope.
  bool Call(const char *globalFunctionName, VMError *errorOut); // Handles scopes for you.

  v8::Handle<v8::Object> VM::Require(const char *script, const char *name);

  zbstring ReturnValueToString(void);

  // Use these functions to set the VM context while using v8 objects.
  // Only use these if using a VMScope doesn't make sense.
  void Enter(void);
  void Exit(void);

  // Enables remote debugging. (use node-inspector)
  void EnableDebugging(const char *name, i32 port);
  void DisableDebugging(void);

  void GarbageCollectFull(void);
  void GarbageCollectIncremental(i32 hint);
  void ForceCollectGarbage(void);

  // Data.
  v8::Isolate *isolate;
  v8::Persistent<v8::Context> context;
  v8::Persistent<v8::Value> returnValue;
};

class VMScope {
public:
  VMScope(VM *vm_) : vm(vm_) { vm_->Enter(); }
  ~VMScope(void) { vm->Exit(); }
  VM *vm;
};

void V8StackTraceToString(v8::Handle<v8::StackTrace> trace, zbstring &stringOut);
void V8PopulateErrorMessage(const v8::TryCatch &tryCatch, bool syntaxError, VMError *errorOut);

#if !defined(ZB_DISABLE_VM_CHECKS)
#define defend_vm(condition) ZB_MULTI_LINE_MACRO_BEGIN \
  if (!(condition)) { \
    return v8::ThrowException(v8::String::New("Failed: " #condition)); \
  } \
  ZB_ASSUME(condition); \
ZB_MULTI_LINE_MACRO_END

#define verify_vm(condition) defend_vm(condition)
#else
#define defend_vm(condition) ZB_MULTI_LINE_MACRO_BEGIN \
  ZB_ASSUME(condition); \
  banish(condition); \
ZB_MULTI_LINE_MACRO_END

#define verify_vm(condition) condition
#endif
