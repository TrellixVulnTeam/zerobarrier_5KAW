void Launch(OS *os);

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR commandLine, i32 commandShow) {
  ZB_SET_ASSERTION_HANDLER(zb_ods_assertion_handler);
  ZB_INITIALIZE_STACK_TRACING();

  OS os;
  os.Initialize(instance, zb_narrow(commandLine), commandShow);
  Launch(&os);

  ZB_SHUTDOWN_STACK_TRACING();
  return 0;
}
