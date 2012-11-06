#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

namespace ControlMouseButton {
  enum ControlMouseButton {
    Left,
    Right,
    Middle
  };
}

class Controls {
public:
  Controls(void);

  void Tick(void);

  void SetKeyState(u32 key, bool down);
  void SetMouseButtonState(u32 button, bool down);
  void SetMousePosition(f32 x, f32 y);

  bool KeyDownTranslate(zbstring key);
  bool KeyPressedTranslate(zbstring key);
  bool KeyDown(u32 key);
  bool KeyPressed(u32 key);

  bool MouseButtonDown(u32 button);
  bool MouseButtonPressed(u32 button);
  void MousePosition(f32 *xOut, f32 *yOut);
  
  void SetTextCapture(bool capture);
  void ClearCapturedText(void);
  zbstring &GetCapturedText(void);
  void CaptureCharacter(zbstring c);
  
private:
  static const i32 KeyStateCount = 256;
  bool keyStates[KeyStateCount];
  bool pressStates[KeyStateCount];

  static const i32 MouseButtonCount = 10;
  bool mouseButtonStates[MouseButtonCount];
  bool mouseButtonPressStates[MouseButtonCount];
  
  f32 mouseX;
  f32 mouseY;

  bool shouldCaptureText;
  zbstring capturedText;
};

ZB_NAMESPACE_END
