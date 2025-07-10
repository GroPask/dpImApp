#include "AppImpl.hpp"

#include "dpImApp/ImGuiInfos.hpp"

#include <imgui_internal.h>
#include <imgui.h>

#include <GLFW/glfw3.h>

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>

namespace dpImApp::detail
{
    int ImGuiExampleGlfwOpenGl3MainPatched(int main_window_width, int main_window_height, const char* main_window_title, AppImplInterface& app_impl_interface);
    void ImGuiExampleGlfwOpenGl3CoreLoop(AppImplInterface& app_impl_interface, GLFWwindow* window);

} // namespace dpImApp::detail

inline dpImApp::detail::AppImpl::AppImpl(std::string_view main_window_title, AppFlags app_flags) :
    MainWindowTitle(main_window_title),
    Flags(app_flags)
{
    if ((Flags & AppFlag::AlwaysAutoResizeMainWindowToContent) != 0)
        Flags = Flags | AppFlag::NoResizableMainWindow;
}

inline void dpImApp::detail::AppImpl::SetMainWindowMinSize(int min_with, int min_height)
{
    const int final_min_with = (min_with > 0 ? min_with : GLFW_DONT_CARE);
    const int final_min_height = (min_height > 0 ? min_height : GLFW_DONT_CARE);

    if (MainWindow != nullptr)
        glfwSetWindowSizeLimits(MainWindow, final_min_with, final_min_height, GLFW_DONT_CARE, GLFW_DONT_CARE);
    else
        PendingMainWindowMinSize = std::make_pair(final_min_with, final_min_height);
}

inline void dpImApp::detail::AppImpl::SetMainWindowAspectRatio(int numerator, int denominator)
{
    const int final_numerator = (numerator > 0 ? numerator : GLFW_DONT_CARE);
    const int final_denominator = (denominator > 0 ? denominator : GLFW_DONT_CARE);

    if (MainWindow != nullptr)
        glfwSetWindowAspectRatio(MainWindow, final_numerator, final_denominator);
    else
        PendingMainWindowAspectRatio = std::make_pair(final_numerator, final_denominator);
}

inline int dpImApp::detail::AppImpl::Run(void (*local_init_func)(void*), const std::function<void()>& update_func)
{
    assert(!IsRunning);

    IsRunning = true;

    LocalInitFunc = local_init_func;
    UpdateFunc = &update_func;

    return detail::ImGuiExampleGlfwOpenGl3MainPatched(1280, 720, MainWindowTitle.c_str(), *this);
}

inline void dpImApp::detail::AppImpl::BeginMainWindowContent(MainWindowFlags main_window_flags)
{
    assert(IsRunning);

    static constexpr const char* MainWindowName = "###dpImAppMainWindow";

    if (!MainWindowMaximized && !MainWindowJustUnmaximized && !MainWindowIconified && !MainWindowJustUniconified)
    {
        if (ImGuiWindow* imGuiMainWindow = ImGui::FindWindowByName(MainWindowName))
        {
            const int imGuiMainWindowPosX = static_cast<int>(imGuiMainWindow->Pos.x);
            const int imGuiMainWindowPosY = static_cast<int>(imGuiMainWindow->Pos.y);

            if (imGuiMainWindowPosX != MainWindowNotMaximizedNotIconifiedPos.first || imGuiMainWindowPosY != MainWindowNotMaximizedNotIconifiedPos.second)
                glfwSetWindowPos(MainWindow, imGuiMainWindowPosX, imGuiMainWindowPosY);
        }
    }

    #ifdef IMGUI_HAS_VIEWPORT
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    #else 
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    #endif

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    ImGuiWindowFlags imgui_main_window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    #ifdef IMGUI_HAS_DOCK
    imgui_main_window_flags = imgui_main_window_flags | ImGuiWindowFlags_NoDocking;
    #endif

    if ((main_window_flags & MainWindowFlag::MenuBar) != 0)
        imgui_main_window_flags = imgui_main_window_flags | ImGuiWindowFlags_MenuBar;

    [[maybe_unused]] const bool result = ImGui::Begin(MainWindowName, nullptr, imgui_main_window_flags);
    assert(result);
}

inline void dpImApp::detail::AppImpl::EndMainWindowContent()
{
    assert(IsRunning);

    if ((Flags & AppFlag::AlwaysAutoResizeMainWindowToContent) != 0)
    {
        assert(MainWindow != nullptr);

        const ImGuiWindow* window = ImGui::GetCurrentWindow();

        const float x_margin = window->WindowPadding.x * 2.f;
        const float y_margin = window->WindowPadding.y * 2.f + window->TitleBarHeight + window->MenuBarHeight;

        if (window->ContentSizeIdeal.x > 0.f && window->ContentSizeIdeal.y > 0)
            glfwSetWindowSize(MainWindow, static_cast<int>(std::ceil(window->ContentSizeIdeal.x + x_margin)), static_cast<int>(std::ceil(window->ContentSizeIdeal.y + y_margin)));
        else
            glfwSetWindowSize(MainWindow, static_cast<int>(std::ceil(window->SizeFull.x + x_margin)), static_cast<int>(std::ceil(window->SizeFull.y + y_margin)));
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

inline void dpImApp::detail::AppImpl::Close()
{
    assert(IsRunning);
    assert(MainWindow != nullptr);

    glfwSetWindowShouldClose(MainWindow, GLFW_TRUE);
}

void dpImApp::detail::AppImpl::InitBeforeCreateMainWindow(int& main_window_width, int& main_window_height)
{
    #if defined(DP_IMAPP_SHARED) && defined(DP_IMAPP_IMGUI_SEEMS_STATIC)
    assert(LocalInitFunc != nullptr);
    LocalInitFunc(ImGui::GetCurrentContext());
    #else
    assert(LocalInitFunc == nullptr);
    #endif

    static constexpr const char* dp_imapp_save_data_name = "dpImApp";
    static constexpr const char* dp_imapp_main_save_data_entry_name = "MainData";

    ImGuiSettingsHandler settingsHandler;
    settingsHandler.TypeName = dp_imapp_save_data_name;
    settingsHandler.TypeHash = ImHashStr(dp_imapp_save_data_name);
    settingsHandler.ReadOpenFn = [](ImGuiContext* /*ctx*/, ImGuiSettingsHandler* /*handler*/, const char* name) -> void*
    {
        if (std::strcmp(name, dp_imapp_main_save_data_entry_name) == 0)
            return reinterpret_cast<void*>(1);

        return nullptr;
    };
    settingsHandler.ReadLineFn = [](ImGuiContext* /*ctx*/, ImGuiSettingsHandler* handler, void* /*entry*/, const char* line)
    {
        AppImpl* self = static_cast<AppImpl*>(handler->UserData);
        assert(self != nullptr);
        self->ReadMainSaveDataLine(line);
    };
    settingsHandler.WriteAllFn = [](ImGuiContext* /*ctx*/, ImGuiSettingsHandler* handler, ImGuiTextBuffer* out_buf)
    {
        out_buf->appendf("[%s][%s]\n", handler->TypeName, dp_imapp_main_save_data_entry_name);

        const AppImpl* self = static_cast<const AppImpl*>(handler->UserData);
        assert(self != nullptr);
        self->WriteAllMainSaveData(*out_buf);

        out_buf->appendf("\n");
    };
    settingsHandler.UserData = this;
    ImGui::AddSettingsHandler(&settingsHandler);

    {
        ImGuiContext& g = *GImGui;

        assert(!g.SettingsLoaded);
        assert(g.SettingsWindows.empty());
        assert(g.IO.IniFilename);
        
        ImGui::LoadIniSettingsFromDisk(g.IO.IniFilename);
        g.SettingsLoaded = true;
    }

    if (MainWindowPosFromSave.has_value())
    {
        glfwWindowHint(GLFW_POSITION_X, MainWindowPosFromSave.value().first);
        glfwWindowHint(GLFW_POSITION_Y, MainWindowPosFromSave.value().second);
    }

    if (MainWindowSizeFromSave.has_value())
    {
        main_window_width = MainWindowSizeFromSave.value().first;
        main_window_height = MainWindowSizeFromSave.value().second;
    }

    if (!MainWindowSizeFromSave.has_value() && (Flags & AppFlag::AlwaysAutoResizeMainWindowToContent) != 0)
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    if ((Flags & AppFlag::NoResizableMainWindow) != 0)
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void dpImApp::detail::AppImpl::InitBeforeMainLoop(GLFWwindow* main_window)
{
    assert(main_window != nullptr);
    assert(IsRunning);
    assert(MainWindow == nullptr);

    MainWindow = main_window;

    glfwSetWindowUserPointer(MainWindow, this);
    glfwSetWindowMaximizeCallback(MainWindow, [](GLFWwindow* window, int maximized) { static_cast<AppImpl*>(glfwGetWindowUserPointer(window))->GlfwMainWindowMaximizeCallback(maximized != 0); });
    glfwSetWindowIconifyCallback(MainWindow, [](GLFWwindow* window, int iconified) { static_cast<AppImpl*>(glfwGetWindowUserPointer(window))->GlfwMainWindowIconifyCallback(iconified != 0); });
    glfwSetWindowPosCallback(MainWindow, [](GLFWwindow* window, int posX, int posY) { static_cast<AppImpl*>(glfwGetWindowUserPointer(window))->GlfwMainWindowPosCallback(posX, posY); });
    glfwSetWindowSizeCallback(MainWindow, [](GLFWwindow* window, int width, int height) { static_cast<AppImpl*>(glfwGetWindowUserPointer(window))->GlfwMainWindowSizeCallback(width, height); });
    glfwSetWindowRefreshCallback(MainWindow, [](GLFWwindow* window) { static_cast<AppImpl*>(glfwGetWindowUserPointer(window))->GlfwMainWindowRefreshCallback(); });

    if (PendingMainWindowMinSize.has_value())
        glfwSetWindowSizeLimits(MainWindow, PendingMainWindowMinSize.value().first, PendingMainWindowMinSize.value().second, GLFW_DONT_CARE, GLFW_DONT_CARE);

    if (PendingMainWindowAspectRatio.has_value())
        glfwSetWindowAspectRatio(MainWindow, PendingMainWindowAspectRatio.value().first, PendingMainWindowAspectRatio.value().second);

    glfwGetWindowPos(MainWindow, &MainWindowNotMaximizedNotIconifiedPos.first, &MainWindowNotMaximizedNotIconifiedPos.second);
    glfwGetWindowSize(MainWindow, &MainWindowNotMaximizedNotIconifiedSize.first, &MainWindowNotMaximizedNotIconifiedSize.second);
}

//#include <chrono>
//#include <thread>

void dpImApp::detail::AppImpl::Update()
{
    assert(UpdateFunc != nullptr);

    if (PendingNewMainWindowMaximized.has_value())
    {
        const bool MainWindowWasMaximized = MainWindowMaximized;

        MainWindowMaximized = PendingNewMainWindowMaximized.value();
        PendingNewMainWindowMaximized.reset();

        if (MainWindowWasMaximized && !MainWindowMaximized)
            MainWindowJustUnmaximized = true;
    }

    if (PendingNewMainWindowIconified.has_value())
    {
        const bool MainWindowWasIconified = MainWindowIconified;

        MainWindowIconified = PendingNewMainWindowIconified.value();
        PendingNewMainWindowIconified.reset();

        if (MainWindowWasIconified && !MainWindowIconified)
            MainWindowJustUniconified = true;
    }

    if (PendingNewMainWindowPos.has_value())
    {
        if (!MainWindowMaximized && !MainWindowIconified)
        {
            MainWindowNotMaximizedNotIconifiedPos = std::move(PendingNewMainWindowPos.value());
            ImGui::MarkIniSettingsDirty();
        }

        PendingNewMainWindowPos.reset();
    }

    if (PendingNewMainWindowSize.has_value())
    {
        if (!MainWindowMaximized && !MainWindowIconified)
        {
            MainWindowNotMaximizedNotIconifiedSize = std::move(PendingNewMainWindowSize.value());
            ImGui::MarkIniSettingsDirty();
        }

        PendingNewMainWindowSize.reset();
    }

    //std::printf("%d Begin\n", FrameCount);
    (*UpdateFunc)();
    //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    //std::printf("%d End\n", FrameCount);

    if (FrameCount == 0)
    {
        if (!MainWindowSizeFromSave.has_value() && (Flags & AppFlag::AlwaysAutoResizeMainWindowToContent) != 0)
            glfwShowWindow(MainWindow);
    }

    MainWindowJustUnmaximized = false;
    MainWindowJustUniconified = false;

    ++FrameCount;
}

void dpImApp::detail::AppImpl::ReadMainSaveDataLine(const char* line)
{
    assert(line != nullptr);
    assert(IsRunning);
    assert(MainWindow == nullptr);

    #ifdef _MSC_VER
    #define sscanf sscanf_s
    #endif

    if ((Flags & AppFlag::NoSavedMainWindowPos) == 0)
    {
        int main_window_x;
        int main_window_y;
        if (sscanf(line, "MainWindowPos=%d,%d\n", &main_window_x, &main_window_y) == 2)
        {
            MainWindowPosFromSave = std::make_pair(main_window_x, main_window_y);
            return;
        }
    }

    if ((Flags & AppFlag::NoSavedMainWindowSize) == 0)
    {
        int main_window_width;
        int main_window_height;
        if (sscanf(line, "MainWindowSize=%d,%d\n", &main_window_width, &main_window_height) == 2)
        {
            MainWindowSizeFromSave = std::make_pair(main_window_width, main_window_height);
            return;
        }
    }

    #ifdef _MSC_VER
    #undef sscanf
    #endif
}

void dpImApp::detail::AppImpl::WriteAllMainSaveData(ImGuiTextBuffer& textBuffer) const
{
    assert(IsRunning);

    if ((Flags & AppFlag::NoSavedMainWindowPos) == 0)
        textBuffer.appendf("MainWindowPos=%d,%d\n", MainWindowNotMaximizedNotIconifiedPos.first, MainWindowNotMaximizedNotIconifiedPos.second);

    if ((Flags & AppFlag::NoSavedMainWindowSize) == 0)
        textBuffer.appendf("MainWindowSize=%d,%d\n", MainWindowNotMaximizedNotIconifiedSize.first, MainWindowNotMaximizedNotIconifiedSize.second);
}

void dpImApp::detail::AppImpl::GlfwMainWindowMaximizeCallback(bool maximized)
{
    PendingNewMainWindowMaximized = maximized;
}

void dpImApp::detail::AppImpl::GlfwMainWindowIconifyCallback(bool iconified)
{
    PendingNewMainWindowIconified = iconified;
}

void dpImApp::detail::AppImpl::GlfwMainWindowPosCallback(int posX, int posY)
{
    PendingNewMainWindowPos = std::make_pair(posX, posY);
}

void dpImApp::detail::AppImpl::GlfwMainWindowSizeCallback(int width, int height)
{
    PendingNewMainWindowSize = std::make_pair(width, height);
}

void dpImApp::detail::AppImpl::GlfwMainWindowRefreshCallback()
{
    ImGuiExampleGlfwOpenGl3CoreLoop(*this, MainWindow);
}
