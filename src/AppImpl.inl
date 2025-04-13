#include "AppImpl.hpp"

#include "dpImApp/ImGuiInfos.hpp"

#include <imgui_internal.h>
#include <imgui.h>

#include <GLFW/glfw3.h>

#include <cassert>
#include <cmath>
#include <cstdio>

namespace dpImApp::detail
{
    int ImGuiExampleGlfwOpenGl3MainPatched(const char* main_window_title, AppImplInterface& app_impl_interface);

} // namespace dpImApp::detail

inline dpImApp::detail::AppImpl::AppImpl(std::string_view main_window_title, AppFlags app_flags) :
    MainWindowTitle(main_window_title),
    Flags(app_flags)
{
    if ((Flags & AppFlag::AlwaysAutoResizeMainWindowToContent) != 0)
    {
        Flags = Flags | AppFlag::NoResizableMainWindow;
        Flags = Flags | AppFlag::NoSavedMainWindowSize;
    }
}

inline int dpImApp::detail::AppImpl::Run(void (*local_init_func)(void*), const std::function<void()>& update_func)
{
    assert(!IsRunning);

    IsRunning = true;

    LocalInitFunc = local_init_func;
    UpdateFunc = &update_func;

    const int result = detail::ImGuiExampleGlfwOpenGl3MainPatched(MainWindowTitle.c_str(), *this);

    MainWindow = nullptr;
    UpdateFunc = nullptr;
    LocalInitFunc = nullptr;
    IsRunning = false;

    return result;
}

inline void dpImApp::detail::AppImpl::BeginMainWindowContent(MainWindowFlags main_window_flags)
{
    assert(IsRunning);

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

    [[maybe_unused]] const bool result = ImGui::Begin("###dpImAppMainWindow", nullptr, imgui_main_window_flags);
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

void dpImApp::detail::AppImpl::InitBeforeCreateMainWindow()
{
    if ((Flags & AppFlag::NoResizableMainWindow) != 0)
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void dpImApp::detail::AppImpl::InitBeforeMainLoop(GLFWwindow* main_window)
{
    assert(main_window != nullptr);
    assert(IsRunning);
    assert(MainWindow == nullptr);

    MainWindow = main_window;

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
}

void dpImApp::detail::AppImpl::Update()
{
    assert(UpdateFunc != nullptr);

    (*UpdateFunc)();
}

void dpImApp::detail::AppImpl::ReadMainSaveDataLine(const char* line)
{
    assert(line != nullptr);
    assert(IsRunning);
    assert(MainWindow != nullptr);

    if ((Flags & AppFlag::NoSavedMainWindowPos) == 0)
    {
        int main_window_x;
        int main_window_y;
        if (sscanf_s(line, "MainWindowPos=%d,%d\n", &main_window_x, &main_window_y) == 2)
        {
            glfwSetWindowPos(MainWindow, main_window_x, main_window_y);
            return;
        }
    }

    if ((Flags & AppFlag::NoSavedMainWindowSize) == 0)
    {
        int main_window_width;
        int main_window_height;
        if (sscanf_s(line, "MainWindowSize=%d,%d\n", &main_window_width, &main_window_height) == 2)
        {
            glfwSetWindowSize(MainWindow, main_window_width, main_window_height);
            return;
        }
    }
}

void dpImApp::detail::AppImpl::WriteAllMainSaveData(ImGuiTextBuffer& textBuffer) const
{
    assert(IsRunning);
    assert(MainWindow != nullptr);

    if ((Flags & AppFlag::NoSavedMainWindowPos) == 0)
    {
        int main_window_x;
        int main_window_y;
        glfwGetWindowPos(MainWindow, &main_window_x, &main_window_y);

        textBuffer.appendf("MainWindowPos=%d,%d\n", main_window_x, main_window_y);
    }

    if ((Flags & AppFlag::NoSavedMainWindowSize) == 0)
    {
        int main_window_width;
        int main_window_height;
        glfwGetWindowSize(MainWindow, &main_window_width, &main_window_height);

        textBuffer.appendf("MainWindowSize=%d,%d\n", main_window_width, main_window_height);
    }
}
