
namespace ControlsBindings {
  v8::Handle<v8::Value> keyPressed(const v8::Arguments &args) {
    defend_vm(args[0]->IsInt32() || args[0]->IsString());
    Controls *controls = (Controls*)v8::External::Unwrap(args.This()->GetInternalField(0));
    if (args[0]->IsInt32()) {
      return v8::Boolean::New(controls->KeyPressed(args[0]->Int32Value()));
    }
    else {
      return v8::Boolean::New(controls->KeyPressedTranslate(*v8::String::AsciiValue(args[0])));
    }
  }

  v8::Handle<v8::Value> keyDown(const v8::Arguments &args) {
    defend_vm(args[0]->IsInt32() || args[0]->IsString());
    Controls *controls = (Controls*)v8::External::Unwrap(args.This()->GetInternalField(0));
    
    if (args[0]->IsInt32()) {
      return v8::Boolean::New(controls->KeyDown(args[0]->Int32Value()));
    }
    else {
      return v8::Boolean::New(controls->KeyDownTranslate(*v8::String::AsciiValue(args[0])));
    }
  }

  v8::Handle<v8::Value> mouseButtonDown(const v8::Arguments &args) {
    defend_vm(args[0]->IsInt32());
    Controls *controls = (Controls*)v8::External::Unwrap(args.This()->GetInternalField(0));
    return v8::Boolean::New(controls->MouseButtonDown(args[0]->Int32Value()));
  }

  v8::Handle<v8::Value> mouseButtonPressed(const v8::Arguments &args) {
    defend_vm(args[0]->IsInt32());
    Controls *controls = (Controls*)v8::External::Unwrap(args.This()->GetInternalField(0));
    return v8::Boolean::New(controls->MouseButtonPressed(args[0]->Int32Value()));
  }

  v8::Handle<v8::Value> mousePosition(const v8::Arguments &args) {
    Controls *controls = (Controls*)v8::External::Unwrap(args.This()->GetInternalField(0));
    f32 x = 0.0f;
    f32 y = 0.0f;
    controls->MousePosition(&x, &y);

    v8::Handle<v8::Object> point = v8::Object::New();
    point->Set(v8::String::New("x"), v8::Number::New(x));
    point->Set(v8::String::New("y"), v8::Number::New(y));
    
    return point;
  }

  v8::Handle<v8::Value> setTextCapture(const v8::Arguments &args) {
    defend_vm(args[0]->IsBoolean());
    Controls *controls = (Controls*)v8::External::Unwrap(args.This()->GetInternalField(0));
    controls->SetTextCapture(args[0]->BooleanValue());
    return v8::Undefined();
  }

  v8::Handle<v8::Value> clearCapturedText(const v8::Arguments &args) {
    Controls *controls = (Controls*)v8::External::Unwrap(args.This()->GetInternalField(0));
    controls->ClearCapturedText();
    return v8::Undefined();
  }

  v8::Handle<v8::Value> getCapturedText(const v8::Arguments &args) {
    Controls *controls = (Controls*)v8::External::Unwrap(args.This()->GetInternalField(0));
    return v8::String::New(controls->GetCapturedText().c_str());
  }
}

void AddControlsBindings(VM *vm, Controls *controls) {
  VMScope scope(vm);
  v8::HandleScope handleScope;

  v8::Local<v8::ObjectTemplate> controlsTemplate = v8::ObjectTemplate::New();
  controlsTemplate->SetInternalFieldCount(1);
  controlsTemplate->Set(v8::String::New("keyDown"), v8::FunctionTemplate::New(ControlsBindings::keyDown));
  controlsTemplate->Set(v8::String::New("keyPressed"), v8::FunctionTemplate::New(ControlsBindings::keyPressed));
  controlsTemplate->Set(v8::String::New("mouseButtonDown"), v8::FunctionTemplate::New(ControlsBindings::mouseButtonDown));
  controlsTemplate->Set(v8::String::New("mouseButtonPressed"), v8::FunctionTemplate::New(ControlsBindings::mouseButtonPressed));
  controlsTemplate->Set(v8::String::New("mousePosition"), v8::FunctionTemplate::New(ControlsBindings::mousePosition));
  controlsTemplate->Set(v8::String::New("setTextCapture"), v8::FunctionTemplate::New(ControlsBindings::setTextCapture));
  controlsTemplate->Set(v8::String::New("clearCapturedText"), v8::FunctionTemplate::New(ControlsBindings::clearCapturedText));
  controlsTemplate->Set(v8::String::New("getCapturedText"), v8::FunctionTemplate::New(ControlsBindings::getCapturedText));
  
  v8::Local<v8::Object> controlsObj = controlsTemplate->NewInstance();
  controlsObj->SetInternalField(0, v8::External::New(controls));

  vm->context->Global()->Set(v8::String::New("Controls"), controlsObj);

  // Keys - Win32 Specific.
  v8::Local<v8::Object> keysObj = v8::Object::New();
  // keysObj->Set(v8::String::New("A"), v8::Integer::New('A'));
  // keysObj->Set(v8::String::New("B"), v8::Integer::New('B'));
  // keysObj->Set(v8::String::New("C"), v8::Integer::New('C'));
  // keysObj->Set(v8::String::New("D"), v8::Integer::New('D'));
  // keysObj->Set(v8::String::New("E"), v8::Integer::New('E'));
  // keysObj->Set(v8::String::New("F"), v8::Integer::New('F'));
  // keysObj->Set(v8::String::New("G"), v8::Integer::New('G'));
  // keysObj->Set(v8::String::New("H"), v8::Integer::New('H'));
  // keysObj->Set(v8::String::New("I"), v8::Integer::New('I'));
  // keysObj->Set(v8::String::New("J"), v8::Integer::New('J'));
  // keysObj->Set(v8::String::New("K"), v8::Integer::New('K'));
  // keysObj->Set(v8::String::New("L"), v8::Integer::New('L'));
  // keysObj->Set(v8::String::New("M"), v8::Integer::New('M'));
  // keysObj->Set(v8::String::New("N"), v8::Integer::New('N'));
  // keysObj->Set(v8::String::New("O"), v8::Integer::New('O'));
  // keysObj->Set(v8::String::New("P"), v8::Integer::New('P'));
  // keysObj->Set(v8::String::New("Q"), v8::Integer::New('Q'));
  // keysObj->Set(v8::String::New("R"), v8::Integer::New('R'));
  // keysObj->Set(v8::String::New("S"), v8::Integer::New('S'));
  // keysObj->Set(v8::String::New("T"), v8::Integer::New('T'));
  // keysObj->Set(v8::String::New("U"), v8::Integer::New('U'));
  // keysObj->Set(v8::String::New("V"), v8::Integer::New('V'));
  // keysObj->Set(v8::String::New("W"), v8::Integer::New('W'));
  // keysObj->Set(v8::String::New("X"), v8::Integer::New('X'));
  // keysObj->Set(v8::String::New("Y"), v8::Integer::New('Y'));
  // keysObj->Set(v8::String::New("Z"), v8::Integer::New('Z'));
  
  // keysObj->Set(v8::String::New("0"), v8::Integer::New('0'));
  // keysObj->Set(v8::String::New("1"), v8::Integer::New('1'));
  // keysObj->Set(v8::String::New("2"), v8::Integer::New('2'));
  // keysObj->Set(v8::String::New("3"), v8::Integer::New('3'));
  // keysObj->Set(v8::String::New("4"), v8::Integer::New('4'));
  // keysObj->Set(v8::String::New("5"), v8::Integer::New('5'));
  // keysObj->Set(v8::String::New("6"), v8::Integer::New('6'));
  // keysObj->Set(v8::String::New("7"), v8::Integer::New('7'));
  // keysObj->Set(v8::String::New("8"), v8::Integer::New('8'));
  // keysObj->Set(v8::String::New("9"), v8::Integer::New('9'));

  keysObj->Set(v8::String::New("Tab"), v8::Integer::New(VK_TAB));
  keysObj->Set(v8::String::New("LeftShift"), v8::Integer::New(VK_LSHIFT));
  keysObj->Set(v8::String::New("RightShift"), v8::Integer::New(VK_RSHIFT));
  keysObj->Set(v8::String::New("Shift"), v8::Integer::New(VK_SHIFT));
  keysObj->Set(v8::String::New("LeftCtrl"), v8::Integer::New(VK_LCONTROL));
  keysObj->Set(v8::String::New("RightCtrl"), v8::Integer::New(VK_RCONTROL));
  keysObj->Set(v8::String::New("Ctrl"), v8::Integer::New(VK_CONTROL));
  keysObj->Set(v8::String::New("LeftAlt"), v8::Integer::New(VK_LMENU));
  keysObj->Set(v8::String::New("RightAlt"), v8::Integer::New(VK_RMENU));
  keysObj->Set(v8::String::New("Alt"), v8::Integer::New(VK_MENU));
  keysObj->Set(v8::String::New("Return"), v8::Integer::New(VK_RETURN));
  keysObj->Set(v8::String::New("Backspace"), v8::Integer::New(VK_BACK));
  keysObj->Set(v8::String::New("Escape"), v8::Integer::New(VK_ESCAPE));
  keysObj->Set(v8::String::New("Spacebar"), v8::Integer::New(VK_SPACE));
  keysObj->Set(v8::String::New("CapsLock"), v8::Integer::New(VK_CAPITAL));
  keysObj->Set(v8::String::New("Up"), v8::Integer::New(VK_UP));
  keysObj->Set(v8::String::New("Down"), v8::Integer::New(VK_DOWN));
  keysObj->Set(v8::String::New("Left"), v8::Integer::New(VK_LEFT));
  keysObj->Set(v8::String::New("Right"), v8::Integer::New(VK_RIGHT));
  keysObj->Set(v8::String::New("Delete"), v8::Integer::New(VK_DELETE));
  //keysObj->Set(v8::String::New("Plus"), v8::Integer::New(VK_OEM_PLUS));
  //keysObj->Set(v8::String::New("Minus"), v8::Integer::New(VK_OEM_MINUS));
  //keysObj->Set(v8::String::New("SemiColon"), v8::Integer::New(VK_OEM_1));
  //keysObj->Set(v8::String::New("ForwardSlash"), v8::Integer::New(VK_OEM_2));
  //keysObj->Set(v8::String::New("Backtick"), v8::Integer::New(VK_OEM_3));
  //keysObj->Set(v8::String::New("LeftBracket"), v8::Integer::New(VK_OEM_4));
  //keysObj->Set(v8::String::New("Backslash"), v8::Integer::New(VK_OEM_5));
  //keysObj->Set(v8::String::New("RightBracket"), v8::Integer::New(VK_OEM_6));
  //keysObj->Set(v8::String::New("Quote"), v8::Integer::New(VK_OEM_7));
  //keysObj->Set(v8::String::New("Period"), v8::Integer::New(VK_OEM_PERIOD));
  //keysObj->Set(v8::String::New("Comma"), v8::Integer::New(VK_OEM_COMMA));

  keysObj->Set(v8::String::New("F1"), v8::Integer::New(VK_F1));
  keysObj->Set(v8::String::New("F2"), v8::Integer::New(VK_F2));
  keysObj->Set(v8::String::New("F3"), v8::Integer::New(VK_F3));
  keysObj->Set(v8::String::New("F4"), v8::Integer::New(VK_F4));
  keysObj->Set(v8::String::New("F5"), v8::Integer::New(VK_F5));
  keysObj->Set(v8::String::New("F6"), v8::Integer::New(VK_F6));
  keysObj->Set(v8::String::New("F7"), v8::Integer::New(VK_F7));
  keysObj->Set(v8::String::New("F8"), v8::Integer::New(VK_F8));
  keysObj->Set(v8::String::New("F9"), v8::Integer::New(VK_F9));
  keysObj->Set(v8::String::New("F10"), v8::Integer::New(VK_F10));
  keysObj->Set(v8::String::New("F11"), v8::Integer::New(VK_F11));
  keysObj->Set(v8::String::New("F12"), v8::Integer::New(VK_F12));

  vm->context->Global()->Set(v8::String::New("Keys"), keysObj);
}
