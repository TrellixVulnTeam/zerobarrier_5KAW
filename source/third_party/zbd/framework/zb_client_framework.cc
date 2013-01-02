
#include "zb_client_framework.h"

#include "../zbd.c"
#include "../controls/zb_controls.cc"
#include "../graphics/zb_graphics.cc"
#include "../io/zb_io.cc"
#include "../math/zb_math.cc"
#include "../profile/zb_profile.cc"
#include "../script/zb_script.cc"
#include "../time/zb_time.cc"
#include "../thread/zb_thread.cc"

#include "internal/entry_win32.cc"
#include "internal/os_win32.cc"

#include "../controls/controls_v8_bindings.cc"
#include "internal/os_win32_v8_bindings.cc"
#include "../graphics/graphics_v8_bindings.cc"
#include "../time/time_v8_bindings.cc"
#include "../profile/profile_v8_bindings.cc"

void ReportVMError(const VMError &errors) {
  zb_spam("Script Error: %s(%d) -- %s\n%s", errors.file.c_str(), errors.line, errors.message.c_str(), errors.callstack.c_str());
}

void Launch(OS *os) {
  Controls controls;
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
