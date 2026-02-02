#include <Menu.h>

#include <Obfuscate.h>
#include <Logger.h>

#include <GLES2/gl2.h>
#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_android.h>
#include <ImGui/backends/imgui_impl_opengl3.h>

#include <fonts/Roboto-Regular.h>

#include <Vars.h>

const ImVec2 window_size = ImVec2(500, 500);

float g_ScreenWidth, g_ScreenHeight;

namespace Menu {
    void Render() {
        //Esp(ImGui::GetBackgroundDrawList());
        if (!bShowMenu)
            return;
        ImGui::SetNextWindowSize(window_size, ImGuiCond_FirstUseEver);
        if(ImGui::Begin("horoni@github.com | GAMENAME [x64]")) {
            ImGui::Checkbox("Test Checkbox", &isTest);
        }
        ImGui::End();
        //ImGui::ShowDemoWindow();
    }

    void InitializeContext(float w, float h) {
        //if (!ImGui::GetCurrentContext()) {
        if (isInitialized) {
            if (w == 0.0 && h == 0.0) return;
            g_ScreenWidth = w;
            g_ScreenHeight = h;
            return;
        }
        ImGui::CreateContext();

        if (w == 0.0 && h == 0.0) {
            g_ScreenWidth = (float)SCREEN_WIDTH;
            g_ScreenHeight = (float)SCREEN_HEIGHT;
        } else {
            g_ScreenWidth = w;
            g_ScreenHeight = h;
        }

        ImGuiIO& io = ImGui::GetIO();

        io.DisplaySize = ImVec2(g_ScreenWidth, g_ScreenHeight);
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.IniFilename = nullptr;

        ImGui_ImplAndroid_Init();
        #if defined(RENDER_EGL) || defined(RENDER_JAVA_EGL)
        ImGui_ImplOpenGL3_Init("#version 300 es");
        #endif

        int systemScale = (1.0 / g_ScreenWidth) * g_ScreenWidth;
        ImFontConfig font_cfg;
        font_cfg.SizePixels = systemScale * 22.0f;
        io.Fonts->AddFontFromMemoryTTF(Roboto_Regular, systemScale * 30.0, 40.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());

        ImGui::GetStyle().ScaleAllSizes(2);

        //io.FontGlobalScale = 0.9f; // default 1.2

        isInitialized = true;
        LOGD("ImGui Context done");
    }
    
    void InitializeContext() {
        InitializeContext(0.0, 0.0);
    }
    
    void DrawFrame(float w, float h) {
        if(!isInitialized) return;
    
        ImGuiIO &io = ImGui::GetIO();
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplAndroid_NewFrame(w, h);
        ImGui::NewFrame();

        Menu::Render();

        ImGui::Render();
        
        #if defined(RENDER_JAVA_EGL)
        static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.00f);
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        #endif
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    
    void DrawFrame() {
        DrawFrame(g_ScreenWidth, g_ScreenHeight);
    }
}
