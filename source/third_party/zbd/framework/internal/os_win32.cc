//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

OSWin32::OSWin32(void)
: instance(0)
, commandShow(0)
, windowClosed(false)
, hwnd(0) {
  // Do nothing.
}

void OSWin32::Initialize(HINSTANCE instance, const zbstring &commandline, i32 commandShow) {
  this->instance = instance;
  this->commandline = commandline;
  this->commandShow = commandShow;
}

void OSWin32::Boot(void) {
  zb_time::Initialize();
}

void OSWin32::InitializeView(i32 width, i32 height) {
  const TCHAR *className = TEXT("zb_view");

  WNDCLASS windowClass = {0};
  windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
  windowClass.lpfnWndProc = &static_window_procedure;
  windowClass.hInstance = instance;
  windowClass.hIcon = LoadIcon(0, IDI_APPLICATION);
  windowClass.lpszClassName = className;

  ATOM result = RegisterClass(&windowClass);
  defend (result != 0);

  RECT adjusted = {0, 0, width, height};
  DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;
  AdjustWindowRect(&adjusted, style, false);
  hwnd = CreateWindowEx(0,
                        className,
                        TEXT(""),
                        style,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        adjusted.right - adjusted.left, adjusted.bottom - adjusted.top,
                        0,
                        0,
                        instance,
                        reinterpret_cast<LPVOID>(this)
                        );
}

void OSWin32::Shutdown(void) {
  // Do nothing.
}

const zbstring &OSWin32::CommandLine(void) {
  return commandline;
}

bool OSWin32::HasQuit(void) {
  return windowClosed;
}

void OSWin32::Tick(void) {
  controls->Tick();

  MSG msg;
  while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE) != 0) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);

    if (msg.message == WM_QUIT) {
      windowClosed = true;
      break;
    }
  }

  // Handle the mouse cursor here to try and obtain the most up-to-date information.
  POINT mousePos = {0};
  GetCursorPos(&mousePos);
  ScreenToClient(hwnd, &mousePos);

  // Clamp the mouse coords to the window. Silly Windows, why would I want coordinates outside my window?
  RECT clientRect = {0};
  GetClientRect(hwnd, &clientRect);
  mousePos.x = zbmax(zbmin(mousePos.x, clientRect.right), clientRect.left);
  mousePos.y = zbmax(zbmin(mousePos.y, clientRect.bottom), clientRect.top);

  controls->SetMousePosition(static_cast<f32>(mousePos.x), static_cast<f32>(mousePos.y));

  BYTE keyStates[256];
  if (GetKeyboardState(keyStates)) {
    for (i32 i = 0; i < 256; ++i) {
      controls->SetKeyState(i, (keyStates[i] & 0x80) != 0);
    }
  }
}

LRESULT CALLBACK OSWin32::static_window_procedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
  OSWin32 *self = 0;
  if (msg == WM_CREATE) {
    // Create the window data and associate it with the window.
    CREATESTRUCT *createStruct = reinterpret_cast<CREATESTRUCT*>(lparam);
    self = reinterpret_cast<OSWin32*>(createStruct->lpCreateParams);
    self->hwnd = hwnd;

    SetWindowLongPtr(hwnd, GWLP_USERDATA, PtrToLong(self));
    ShowWindow(self->hwnd, self->commandShow);
  }
  else {
    // Grab the instance that was stored previously.
    self = reinterpret_cast<OSWin32*>(LongToPtr(GetWindowLongPtr(hwnd, GWLP_USERDATA)));
    if (self == 0) { return DefWindowProc(hwnd, msg, wparam, lparam); }
  }

  defend (self->hwnd == hwnd);
  return self->window_procedure(msg, wparam, lparam);
}

LRESULT OSWin32::window_procedure(UINT msg, WPARAM wparam, LPARAM lparam) {
  switch (msg) {
    case WM_CHAR:
    {
      zbwstring wString;
      wString += (wchar_t)wparam;
      controls->CaptureCharacter(zb_narrow(wString)); return 0;
    }
    case WM_LBUTTONDOWN:  controls->SetMouseButtonState(ControlMouseButton::Left, true); return 0;
    case WM_LBUTTONUP:    controls->SetMouseButtonState(ControlMouseButton::Left, false); return 0;
    case WM_RBUTTONDOWN:  controls->SetMouseButtonState(ControlMouseButton::Right, true); return 0;
    case WM_RBUTTONUP:    controls->SetMouseButtonState(ControlMouseButton::Right, false); return 0;
    case WM_DESTROY:      PostQuitMessage(0); return 0;
  }

  return DefWindowProc(hwnd, msg, wparam, lparam);
}

HWND OSWin32::GetWindowHandle(void) {
  return hwnd;
}

ZB_NAMESPACE_END
