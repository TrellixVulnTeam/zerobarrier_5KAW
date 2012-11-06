#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

class OSWin32 {
public:
  OSWin32(void);

  void Initialize(HINSTANCE instance, const zbstring &commandLine, i32 commandShow);
  void InitializeView(i32 width, i32 height);

  void Boot(void);
  void Tick(void);
  void Shutdown(void);
  
  const zbstring &CommandLine(void);
  bool HasQuit(void);

  HWND GetWindowHandle(void);

  Controls *controls;

private:
  LRESULT window_procedure(UINT msg, WPARAM wparam, LPARAM lparam);
  static LRESULT CALLBACK static_window_procedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

  HINSTANCE instance;
  zbstring commandline;
  i32 commandShow;
  bool windowClosed;

  HWND hwnd;
};

typedef OSWin32 OS;

ZB_NAMESPACE_END
