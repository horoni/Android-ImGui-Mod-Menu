#include <android/input.h>
#include <android/keycodes.h>

// Taken from https://github.com/SlyrithDevelopment/Unity-ImGUI-Android/blob/main/src/hooks.cpp

ImGuiKey KeyCodeToImGuiKey(int32_t key_code) {
        switch (key_code) {
            case AKEYCODE_TAB:                  return ImGuiKey_Tab;
            case AKEYCODE_DPAD_LEFT:            return ImGuiKey_LeftArrow;
            case AKEYCODE_DPAD_RIGHT:           return ImGuiKey_RightArrow;
            case AKEYCODE_DPAD_UP:              return ImGuiKey_UpArrow;
            case AKEYCODE_DPAD_DOWN:            return ImGuiKey_DownArrow;
            case AKEYCODE_PAGE_UP:              return ImGuiKey_PageUp;
            case AKEYCODE_PAGE_DOWN:            return ImGuiKey_PageDown;
            case AKEYCODE_MOVE_HOME:            return ImGuiKey_Home;
            case AKEYCODE_MOVE_END:             return ImGuiKey_End;
            case AKEYCODE_INSERT:               return ImGuiKey_Insert;
            case AKEYCODE_FORWARD_DEL:          return ImGuiKey_Delete;
            case AKEYCODE_DEL:                  return ImGuiKey_Backspace;
            case AKEYCODE_SPACE:                return ImGuiKey_Space;
            case AKEYCODE_ENTER:                return ImGuiKey_Enter;
            case AKEYCODE_ESCAPE:               return ImGuiKey_Escape;
            case AKEYCODE_APOSTROPHE:           return ImGuiKey_Apostrophe;
            case AKEYCODE_COMMA:                return ImGuiKey_Comma;
            case AKEYCODE_MINUS:                return ImGuiKey_Minus;
            case AKEYCODE_PERIOD:               return ImGuiKey_Period;
            case AKEYCODE_SLASH:                return ImGuiKey_Slash;
            case AKEYCODE_SEMICOLON:            return ImGuiKey_Semicolon;
            case AKEYCODE_EQUALS:               return ImGuiKey_Equal;
            case AKEYCODE_LEFT_BRACKET:         return ImGuiKey_LeftBracket;
            case AKEYCODE_BACKSLASH:            return ImGuiKey_Backslash;
            case AKEYCODE_RIGHT_BRACKET:        return ImGuiKey_RightBracket;
            case AKEYCODE_GRAVE:                return ImGuiKey_GraveAccent;
            case AKEYCODE_CAPS_LOCK:            return ImGuiKey_CapsLock;
            case AKEYCODE_SCROLL_LOCK:          return ImGuiKey_ScrollLock;
            case AKEYCODE_NUM_LOCK:             return ImGuiKey_NumLock;
            case AKEYCODE_SYSRQ:                return ImGuiKey_PrintScreen;
            case AKEYCODE_BREAK:                return ImGuiKey_Pause;
            case AKEYCODE_NUMPAD_0:             return ImGuiKey_Keypad0;
            case AKEYCODE_NUMPAD_1:             return ImGuiKey_Keypad1;
            case AKEYCODE_NUMPAD_2:             return ImGuiKey_Keypad2;
            case AKEYCODE_NUMPAD_3:             return ImGuiKey_Keypad3;
            case AKEYCODE_NUMPAD_4:             return ImGuiKey_Keypad4;
            case AKEYCODE_NUMPAD_5:             return ImGuiKey_Keypad5;
            case AKEYCODE_NUMPAD_6:             return ImGuiKey_Keypad6;
            case AKEYCODE_NUMPAD_7:             return ImGuiKey_Keypad7;
            case AKEYCODE_NUMPAD_8:             return ImGuiKey_Keypad8;
            case AKEYCODE_NUMPAD_9:             return ImGuiKey_Keypad9;
            case AKEYCODE_NUMPAD_DOT:           return ImGuiKey_KeypadDecimal;
            case AKEYCODE_NUMPAD_DIVIDE:        return ImGuiKey_KeypadDivide;
            case AKEYCODE_NUMPAD_MULTIPLY:      return ImGuiKey_KeypadMultiply;
            case AKEYCODE_NUMPAD_SUBTRACT:      return ImGuiKey_KeypadSubtract;
            case AKEYCODE_NUMPAD_ADD:           return ImGuiKey_KeypadAdd;
            case AKEYCODE_NUMPAD_ENTER:         return ImGuiKey_KeypadEnter;
            case AKEYCODE_NUMPAD_EQUALS:        return ImGuiKey_KeypadEqual;
            case AKEYCODE_CTRL_LEFT:            return ImGuiKey_LeftCtrl;
            case AKEYCODE_SHIFT_LEFT:           return ImGuiKey_LeftShift;
            case AKEYCODE_ALT_LEFT:             return ImGuiKey_LeftAlt;
            case AKEYCODE_META_LEFT:            return ImGuiKey_LeftSuper;
            case AKEYCODE_CTRL_RIGHT:           return ImGuiKey_RightCtrl;
            case AKEYCODE_SHIFT_RIGHT:          return ImGuiKey_RightShift;
            case AKEYCODE_ALT_RIGHT:            return ImGuiKey_RightAlt;
            case AKEYCODE_META_RIGHT:           return ImGuiKey_RightSuper;
            case AKEYCODE_MENU:                 return ImGuiKey_Menu;
            case AKEYCODE_0:                    return ImGuiKey_0;
            case AKEYCODE_1:                    return ImGuiKey_1;
            case AKEYCODE_2:                    return ImGuiKey_2;
            case AKEYCODE_3:                    return ImGuiKey_3;
            case AKEYCODE_4:                    return ImGuiKey_4;
            case AKEYCODE_5:                    return ImGuiKey_5;
            case AKEYCODE_6:                    return ImGuiKey_6;
            case AKEYCODE_7:                    return ImGuiKey_7;
            case AKEYCODE_8:                    return ImGuiKey_8;
            case AKEYCODE_9:                    return ImGuiKey_9;
            case AKEYCODE_A:                    return ImGuiKey_A;
            case AKEYCODE_B:                    return ImGuiKey_B;
            case AKEYCODE_C:                    return ImGuiKey_C;
            case AKEYCODE_D:                    return ImGuiKey_D;
            case AKEYCODE_E:                    return ImGuiKey_E;
            case AKEYCODE_F:                    return ImGuiKey_F;
            case AKEYCODE_G:                    return ImGuiKey_G;
            case AKEYCODE_H:                    return ImGuiKey_H;
            case AKEYCODE_I:                    return ImGuiKey_I;
            case AKEYCODE_J:                    return ImGuiKey_J;
            case AKEYCODE_K:                    return ImGuiKey_K;
            case AKEYCODE_L:                    return ImGuiKey_L;
            case AKEYCODE_M:                    return ImGuiKey_M;
            case AKEYCODE_N:                    return ImGuiKey_N;
            case AKEYCODE_O:                    return ImGuiKey_O;
            case AKEYCODE_P:                    return ImGuiKey_P;
            case AKEYCODE_Q:                    return ImGuiKey_Q;
            case AKEYCODE_R:                    return ImGuiKey_R;
            case AKEYCODE_S:                    return ImGuiKey_S;
            case AKEYCODE_T:                    return ImGuiKey_T;
            case AKEYCODE_U:                    return ImGuiKey_U;
            case AKEYCODE_V:                    return ImGuiKey_V;
            case AKEYCODE_W:                    return ImGuiKey_W;
            case AKEYCODE_X:                    return ImGuiKey_X;
            case AKEYCODE_Y:                    return ImGuiKey_Y;
            case AKEYCODE_Z:                    return ImGuiKey_Z;
            case AKEYCODE_F1:                   return ImGuiKey_F1;
            case AKEYCODE_F2:                   return ImGuiKey_F2;
            case AKEYCODE_F3:                   return ImGuiKey_F3;
            case AKEYCODE_F4:                   return ImGuiKey_F4;
            case AKEYCODE_F5:                   return ImGuiKey_F5;
            case AKEYCODE_F6:                   return ImGuiKey_F6;
            case AKEYCODE_F7:                   return ImGuiKey_F7;
            case AKEYCODE_F8:                   return ImGuiKey_F8;
            case AKEYCODE_F9:                   return ImGuiKey_F9;
            case AKEYCODE_F10:                  return ImGuiKey_F10;
            case AKEYCODE_F11:                  return ImGuiKey_F11;
            case AKEYCODE_F12:                  return ImGuiKey_F12;
            default:                            return ImGuiKey_None;
        }
}

namespace Unity_input {
    int (*o_inject_event)(JNIEnv *env, jobject __this, jobject input_event);

    jmethodID MotionEvent_getX, MotionEvent_getY, MotionEvent_getAction, MotionEvent_getToolType, MotionEvent_getButtonState, MotionEvent_getAxisValue;
    jmethodID KeyEvent_getUnicodeChar, KeyEvent_getMetaState, KeyEvent_getAction, KeyEvent_getKeyCode, KeyEvent_getScanCode;
    int hook(JNIEnv *env, jobject __this, jobject input_event) {
        jclass motion_event = env->FindClass(("android/view/MotionEvent"));
        
        if(!motion_event){
            LOGI("Can't find MotionEvent!");
            return o_inject_event(env, __this, input_event);
        }

        ImGuiIO &io = ImGui::GetIO();

        if (env->IsInstanceOf(input_event, motion_event)) {
            if (!MotionEvent_getX) MotionEvent_getX = env->GetMethodID(motion_event, ("getX"), ("(I)F"));
            if (!MotionEvent_getY) MotionEvent_getY = env->GetMethodID(motion_event, ("getY"), ("(I)F"));
            if (!MotionEvent_getToolType) MotionEvent_getToolType = env->GetMethodID(motion_event, ("getToolType"), ("(I)I"));
            if (!MotionEvent_getAction) MotionEvent_getAction = env->GetMethodID(motion_event, ("getAction"), ("()I"));
            if (!MotionEvent_getButtonState) MotionEvent_getButtonState = env->GetMethodID(motion_event, ("getButtonState"), ("()I"));
            if (!MotionEvent_getAxisValue) MotionEvent_getAxisValue = env->GetMethodID(motion_event, ("getAxisValue"), ("(II)F"));

            auto action = env->CallIntMethod(input_event, MotionEvent_getAction);
            int32_t pointer_index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            action &= AMOTION_EVENT_ACTION_MASK;

            switch (action) {
                case AMOTION_EVENT_ACTION_DOWN:
                case AMOTION_EVENT_ACTION_UP:
                    if ((env->CallIntMethod(input_event, MotionEvent_getToolType, pointer_index) == AMOTION_EVENT_TOOL_TYPE_FINGER) ||
                        (env->CallIntMethod(input_event, MotionEvent_getToolType, pointer_index) == AMOTION_EVENT_TOOL_TYPE_UNKNOWN)) {
                        io.AddMousePosEvent(env->CallFloatMethod(input_event, MotionEvent_getX, pointer_index), env->CallFloatMethod(input_event, MotionEvent_getY, pointer_index));
                        io.AddMouseButtonEvent(0, action == AMOTION_EVENT_ACTION_DOWN);
                    }
                    break;
                case AMOTION_EVENT_ACTION_BUTTON_PRESS:
                case AMOTION_EVENT_ACTION_BUTTON_RELEASE: {
                    jint button_state = env->CallIntMethod(input_event, MotionEvent_getButtonState);
                    io.AddMouseButtonEvent(0, (button_state & AMOTION_EVENT_BUTTON_PRIMARY) != 0);
                    io.AddMouseButtonEvent(1, (button_state & AMOTION_EVENT_BUTTON_SECONDARY) != 0);
                    io.AddMouseButtonEvent(2, (button_state & AMOTION_EVENT_BUTTON_TERTIARY) != 0);
                } break;
                case AMOTION_EVENT_ACTION_HOVER_MOVE:
                case AMOTION_EVENT_ACTION_MOVE:
                    io.AddMousePosEvent(env->CallFloatMethod(input_event, MotionEvent_getX, pointer_index), env->CallFloatMethod(input_event, MotionEvent_getY, pointer_index));
                    break;
                case AMOTION_EVENT_ACTION_SCROLL:
                    io.AddMouseWheelEvent(env->CallFloatMethod(input_event, MotionEvent_getAxisValue, AMOTION_EVENT_AXIS_HSCROLL, pointer_index),
                        env->CallFloatMethod(input_event, MotionEvent_getAxisValue, AMOTION_EVENT_AXIS_VSCROLL, pointer_index));
                    break;
                default:
                    break;
            }
            
            if (io.WantCaptureMouse) return true;
        }

        jclass key_event = env->FindClass("android/view/KeyEvent");
        if(!key_event){
            LOGI("Can't find KeyEvent!");
            return o_inject_event(env, __this, input_event);
        }

        if (env->IsInstanceOf(input_event, key_event)) {

            if (!KeyEvent_getAction) KeyEvent_getAction = env->GetMethodID(key_event, ("getAction"), ("()I"));
            if (!KeyEvent_getKeyCode) KeyEvent_getKeyCode = env->GetMethodID(key_event, ("getKeyCode"), ("()I"));
            if (!KeyEvent_getUnicodeChar) KeyEvent_getUnicodeChar = env->GetMethodID(key_event, ("getUnicodeChar"), ("(I)I"));
            if (!KeyEvent_getMetaState) KeyEvent_getMetaState = env->GetMethodID(key_event, ("getMetaState"), ("()I"));
            if (!KeyEvent_getScanCode) KeyEvent_getScanCode = env->GetMethodID(key_event, ("getScanCode"), ("()I"));
                
            jint Action = env->CallIntMethod(input_event, KeyEvent_getAction);
            jint KeyCode = env->CallIntMethod(input_event, KeyEvent_getKeyCode);
            jint MetaState = env->CallIntMethod(input_event, KeyEvent_getMetaState);
            jint ScanCode = env->CallIntMethod(input_event, KeyEvent_getScanCode);

            io.AddKeyEvent(ImGuiKey_ModCtrl, (MetaState & AMETA_CTRL_ON) != 0);
            io.AddKeyEvent(ImGuiKey_ModShift, (MetaState & AMETA_SHIFT_ON) != 0);
            io.AddKeyEvent(ImGuiKey_ModAlt, (MetaState & AMETA_ALT_ON) != 0);
            io.AddKeyEvent(ImGuiKey_ModSuper, (MetaState & AMETA_META_ON) != 0);

            switch (Action) {
                case AKEY_EVENT_ACTION_DOWN:
                case AKEY_EVENT_ACTION_UP: {
                    ImGuiKey key = KeyCodeToImGuiKey(KeyCode);
                    if (key != ImGuiKey_None && (Action == AKEY_EVENT_ACTION_DOWN || Action == AKEY_EVENT_ACTION_UP)) {
                        io.AddKeyEvent(key, Action == AKEY_EVENT_ACTION_DOWN);
                        io.SetKeyEventNativeData(key, KeyCode, ScanCode);
                    }
                    if (io.WantCaptureKeyboard && Action != AKEY_EVENT_ACTION_UP) {
                        io.AddInputCharacter(env->CallIntMethod(input_event, KeyEvent_getUnicodeChar, MetaState));
                    }
                    break;
                }
                default:
                    break;
            }
        }
        return o_inject_event(env, __this, input_event);
    }
};
