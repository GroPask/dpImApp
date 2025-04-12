#ifndef DP_IMAPP_HPP
#define DP_IMAPP_HPP

#include "Export.hpp"

#include <functional>
#include <memory>
#include <string_view>

// TODO
// Save position main window
// Auto size main window
// Install
// Auto dock space sur la main window pour faire comme c'est le cas pas default ?
// Always static dpImAppMain to handle exe side code (for global dll sharing) and WinMain ?
// Kind of Interface/VTable with multiple callback to pass through Patched.cpp ?
// Simple test ImGui::Text and x loop and app.Close();

namespace dpImApp
{
    enum AppFlag : int
    {
    };
    using AppFlags = int;

    enum MainWindowFlag : int
    {
        MenuBar = (1 << 0),
    };
    using MainWindowFlags = int;

    class App
    {
    public:
        DP_IMAPP_API App(std::string_view main_window_title = "", AppFlags app_flags = 0);
        DP_IMAPP_API ~App();

        template <class F>
        int Run(F&& update_func);

        DP_IMAPP_API void BeginMainWindowContent(MainWindowFlags main_window_flags = 0);
        DP_IMAPP_API void EndMainWindowContent();

        DP_IMAPP_API void Close();

    private:
        DP_IMAPP_API int RunImpl(void (*local_init_func)(void*), const std::function<void()>& update_func);

        struct Data;
        std::unique_ptr<Data> InternalData;
    };

} // namespace dpImApp

#include "dpImApp/dpImApp.inl"

#endif
