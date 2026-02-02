package com.horoni.menu;

import android.app.Activity;
import android.os.Bundle;
import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.view.WindowManager;
import android.widget.TableRow.LayoutParams;
import android.view.ViewGroup;
import android.graphics.PixelFormat;
import android.view.Gravity;
import android.view.InputEvent;
import android.view.MotionEvent;

public class MainActivity extends Activity {
    static {
        System.loadLibrary("MCL");
    }
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        startMain(this);
    }
    
    public static void startMain(final Context ctx)
    {
        Handler hand = new Handler(Looper.getMainLooper());
        hand.postDelayed(() -> startMenu(ctx), 500);
    }
    
    public static void startMenu(Context ctx)
    {
        WindowManager wm = ((Activity)ctx).getWindowManager();
        WindowManager.LayoutParams layoutParams = new WindowManager.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT, 0, 0,
                WindowManager.LayoutParams.TYPE_APPLICATION,
                WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE | WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE,
                PixelFormat.TRANSPARENT);
        layoutParams.gravity = Gravity.TOP | Gravity.CENTER_VERTICAL | Gravity.CENTER_HORIZONTAL;
        GlViewWrapper view = new GlViewWrapper(ctx);
        wm.addView(view, layoutParams);
    }
}
