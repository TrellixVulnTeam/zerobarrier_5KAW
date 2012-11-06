//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

Controls::Controls(void) {
  memset(keyStates, 0, sizeof(keyStates));
  memset(pressStates, 0, sizeof(pressStates));
  memset(mouseButtonStates, 0, sizeof(mouseButtonStates));
  memset(mouseButtonPressStates, 0, sizeof(mouseButtonPressStates));
}

void Controls::Tick(void) {
  memset(mouseButtonPressStates, 0, sizeof(mouseButtonPressStates));
}

void Controls::SetKeyState(u32 key, bool down) {
  defend (key < KeyStateCount);
  pressStates[key] = !down && keyStates[key];
  keyStates[key] = down;
}

void Controls::SetMouseButtonState(u32 button, bool down) {
  defend (button < MouseButtonCount);
  mouseButtonStates[button] = down;
  mouseButtonPressStates[button] = !down;
}

void Controls::SetMousePosition(f32 x, f32 y) {
  mouseX = x;
  mouseY = y;
}

bool Controls::KeyDown(u32 key) {
  defend (key < KeyStateCount);
  return keyStates[key];
}

bool Controls::KeyPressed(u32 key) {
  defend (key < KeyStateCount);
  return pressStates[key];
}

bool Controls::KeyDownTranslate(zbstring key) {
  return KeyDown(VkKeyScan(zb_widen(key)[0]) & 0xff);
}

bool Controls::KeyPressedTranslate(zbstring key) {
  return KeyPressed(VkKeyScan(zb_widen(key)[0]) & 0xff);
}

bool Controls::MouseButtonDown(u32 button) {
  defend (button < MouseButtonCount);
  return mouseButtonStates[button];
}

bool Controls::MouseButtonPressed(u32 button) {
  defend (button < MouseButtonCount);
  return mouseButtonPressStates[button];
}

void Controls::MousePosition(f32 *xOut, f32 *yOut) {
  *xOut = mouseX;
  *yOut = mouseY;
}

void Controls::SetTextCapture(bool capture) {
  shouldCaptureText = capture;
}

void Controls::ClearCapturedText(void) {
  capturedText.clear();
}

zbstring &Controls::GetCapturedText(void) {
  return capturedText;
}

void Controls::CaptureCharacter(zbstring c) {
  if (shouldCaptureText) {
    if (c == "\b") { // backspace
      if (!capturedText.empty()) {
        capturedText.resize(capturedText.size() - 1);  
      }
    }
    else {
      capturedText += c;
    }
  }
}

ZB_NAMESPACE_END
