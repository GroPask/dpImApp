#include "dpImApp/dpImApp.hpp"

#include <imgui.h>

#include <GLFW/glfw3.h>

#include <string>

#include <cassert>

namespace dpImApp::detail
{
    int ImGuiExampleGlfwOpenGl3MainPatched(const char* main_window_title, const std::function<void(GLFWwindow*)>& init_func, const std::function<void()>& update_func);

} // namespace dpImApp::detail

struct dpImApp::App::Data
{
    Data(std::string_view main_window_title, AppFlags app_flags) :
        MainWindowTitle(main_window_title),
        Flags(app_flags)
    {
    }

    std::string MainWindowTitle;
    AppFlags Flags;

    bool IsRunning = false;
    GLFWwindow* MainWindow = nullptr;
};

dpImApp::App::App(std::string_view main_window_title, AppFlags app_flags) :
    InternalData(std::make_unique<Data>(main_window_title, app_flags))
{
}

dpImApp::App::~App() = default;

void dpImApp::App::BeginMainWindowContent(MainWindowFlags main_window_flags)
{
    assert(InternalData->IsRunning);

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

void dpImApp::App::EndMainWindowContent()
{
    assert(InternalData->IsRunning);

    ImGui::End();
    ImGui::PopStyleVar();
}

void dpImApp::App::Close()
{
    assert(InternalData->IsRunning);
    assert(InternalData->MainWindow != nullptr);

    glfwSetWindowShouldClose(InternalData->MainWindow, GLFW_TRUE);
}

int dpImApp::App::RunImpl(void (*local_init_func)(void*), const std::function<void()>& update_func)
{
    assert(!InternalData->IsRunning);

    InternalData->IsRunning = true;

    const int result = detail::ImGuiExampleGlfwOpenGl3MainPatched(InternalData->MainWindowTitle.c_str(),
        [this, local_init_func](GLFWwindow* main_window)
        {
            assert(main_window != nullptr);
            assert(InternalData->IsRunning);
            assert(InternalData->MainWindow == nullptr);

            InternalData->MainWindow = main_window;

            #ifdef DP_IMAPP_SHARED
            assert(local_init_func != nullptr);
            local_init_func(ImGui::GetCurrentContext());
            #else
            assert(local_init_func == nullptr);
            #endif
        },
        update_func
    );

    InternalData->IsRunning = false;
    InternalData->MainWindow = nullptr;

    return result;
}
