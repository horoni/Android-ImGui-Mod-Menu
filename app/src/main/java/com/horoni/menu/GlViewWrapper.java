package com.horoni.menu;

import android.opengl.GLSurfaceView;
import android.content.Context;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;
import android.graphics.PixelFormat;

public class GlViewWrapper extends GLSurfaceView implements GLSurfaceView.Renderer {
    GlViewWrapper(Context ctx)
    {
        super(ctx);
        setEGLContextClientVersion(3);
        setEGLConfigChooser(8,8,8,8,16,0);
        getHolder().setFormat(PixelFormat.TRANSPARENT);
        setZOrderOnTop(true);
        setRenderer(this);
    }

    @Override
    public void onDrawFrame(GL10 p1) {
        NativeMethods.onDrawFrame();
    }

    @Override
    public void onSurfaceCreated(GL10 p1, EGLConfig p2) {
        NativeMethods.onSurfaceCreated();
    }
    @Override
    public void onSurfaceChanged(GL10 p1, int width, int height) {
        NativeMethods.onSurfaceChanged(width, height);
    }
}
