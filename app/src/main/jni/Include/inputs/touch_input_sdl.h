#include <deque>
#include <cmath>

namespace SDL_Input {
    void (*o_onNativeTouch)(JNIEnv *env, jobject instance,
        int touchDevId, int pointerFingerId,
        int action, float x, 
        float y, float p);

    void hook(JNIEnv *env, jobject instance,
        int touchDevId, int pointerFingerId,
        int action, float x, 
        float y, float p) {
            // Save original vars
            int o_action = action;
            float o_x = x, o_y = y;
            
            ImGuiIO &io = ImGui::GetIO();
            
            // Normalize coordinates, trunc() is faster than round()
            x *= trunc(io.DisplaySize.x);
            y *= trunc(io.DisplaySize.y);
            
            if (touchDevId == 3)
                io.AddMouseSourceEvent(ImGuiMouseSource_TouchScreen);
            
            int32_t pointer_index = pointerFingerId;
            switch(action) {
                case AMOTION_EVENT_ACTION_DOWN:
                case AMOTION_EVENT_ACTION_UP:
                    io.AddMousePosEvent(x, y);
                    io.AddMouseButtonEvent(0, action == AMOTION_EVENT_ACTION_DOWN);
                    break;
                case AMOTION_EVENT_ACTION_BUTTON_PRESS:
                case AMOTION_EVENT_ACTION_BUTTON_RELEASE: { // TODO: Implement this
                    /*jint button_state = env->CallIntMethod(input_event, MotionEvent_getButtonState);
                    io.AddMouseButtonEvent(0, (button_state & AMOTION_EVENT_BUTTON_PRIMARY) != 0);
                    io.AddMouseButtonEvent(1, (button_state & AMOTION_EVENT_BUTTON_SECONDARY) != 0);
                    io.AddMouseButtonEvent(2, (button_state & AMOTION_EVENT_BUTTON_TERTIARY) != 0);*/
                } break;
                case AMOTION_EVENT_ACTION_HOVER_MOVE:
                case AMOTION_EVENT_ACTION_MOVE:
                    io.AddMousePosEvent(x, y);
                    break;
                case AMOTION_EVENT_ACTION_SCROLL: // TODO: Implement this
                    /*io.AddMouseWheelEvent(env->CallFloatMethod(input_event, MotionEvent_getAxisValue, AMOTION_EVENT_AXIS_HSCROLL, pointer_index),
                        env->CallFloatMethod(input_event, MotionEvent_getAxisValue, AMOTION_EVENT_AXIS_VSCROLL, pointer_index));
                    */
                    break;
                default:
                    break;
            }
            if (io.WantCaptureMouse) return;
            
            o_onNativeTouch(env, instance, touchDevId, pointerFingerId, o_action, o_x, o_y, p);
    }
}