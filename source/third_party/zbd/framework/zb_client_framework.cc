
#include "zb_client_framework.h"

#ifndef ZB_DISABLE_PROFILE_VISUALIZER
#include "internal/profile_visualizer.cc"
#endif

Controls *CONTROLS = 0x0;

#include "internal/entry_win32.cc"
#include "internal/os_win32.cc"
#include "internal/platform_editor_2d.cc"

#include "internal/v8_bindings/controls_v8_bindings.cc"
#include "internal/v8_bindings/os_win32_v8_bindings.cc"
#include "internal/v8_bindings/graphics_v8_bindings.cc"
#include "internal/v8_bindings/time_v8_bindings.cc"
#include "internal/v8_bindings/platform_editor_2d_v8_bindings.cc"
#include "internal/v8_bindings/profile_v8_bindings.cc"

void ReportVMError(const VMError &errors) {
  zb_spam("Script Error: %s(%d) -- %s\n%s", errors.file.c_str(), errors.line, errors.message.c_str(), errors.callstack.c_str());
}

void Launch(OS *os) {
  Controls controls;
  CONTROLS = &controls;
  os->controls = &controls;

  ViewParameters vp;
  vp.width = 1280;
  vp.height = 720;
  vp.windowed = true;

  os->Boot();

  // TODO: This setup block can probably be wrapped and moved into script.
  os->InitializeView(vp.width, vp.height);
  vp.view = os->GetWindowHandle();

  RenderThread renderThread;
  renderThread.Initialize(vp);
  Font::Initialize();
  renderThread.start();

  VM vm;
  VMError errors;

  do {
    errors.Reset();

    vm.Reset();
    vm.EnableDebugging("client", 5858);
    SandboxVM(&vm);
    AddVMBindings(&vm);
    AddJSONBindings(&vm);
    AddOSBindings(&vm, os);
    AddTimeBindings(&vm);
    AddProfileBindings(&vm);
    AddControlsBindings(&vm, &controls);
    AddGraphicsBindings(&vm);
    AddPhysicsEditor2dBindings(&vm);

    VMRequireFile("js/require.js", &vm, &errors);
    VMRequireFile("js/boot.js", &vm, &errors);

    if (!errors.HasError()) {
      vm.Call("main", &errors);
    }

    if (errors.HasError()) {
      ReportVMError(errors);

      if (errors.isRecoverable) {
        errors.Reset();
        vm.Call("onError", &errors);
      }
    }

    vm.DisableDebugging();
  } while (vm.ReturnValueToString() == "false");

  // Re-enable debugging to work around a crash in V8 on garbage collection.
  vm.EnableDebugging("client", 5858);
  vm.Shutdown();

  renderThread.stop();
  Font::Shutdown();
  renderThread.Shutdown();
  ProfileShutdown();
  os->Shutdown();
}
