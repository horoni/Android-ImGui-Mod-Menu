package com.horoni.menu;

public class NativeMethods {
    public static native void onDrawFrame();
    public static native void onSurfaceChanged(int width, int height);
    public static native void onSurfaceCreated();
//    public static native boolean handleTouch(float x, float y, int action);
}
