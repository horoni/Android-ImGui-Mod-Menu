#ifndef HORONI_MENU_H
#define HORONI_MENU_H

#define SCREEN_WIDTH 2400
#define SCREEN_HEIGHT 1080

extern float g_ScreenWidth, g_ScreenHeight;

namespace Menu {
    
    void Render();
    
    void InitializeContext(float w, float h);
    void InitializeContext();
    void DrawFrame(float w, float h);
    void DrawFrame();
    
    inline bool isInitialized = false;
    inline bool bShowMenu = true;
}

#endif // HORONI_MENU_H