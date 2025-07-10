#ifndef DP_IMAPP_APP_HPP
#define DP_IMAPP_APP_HPP

#include "Config.hpp"

#include <functional>
#include <memory>
#include <string_view>

namespace dpImApp
{
    namespace detail { class AppImpl; }

    enum AppFlag : int
    {
        AlwaysAutoResizeMainWindowToContent = (1 << 0),
        NoResizableMainWindow               = (1 << 1), // Implicit if AlwaysAutoResizeMainWindowToContent
        NoSavedMainWindowPos                = (1 << 2),
        NoSavedMainWindowSize               = (1 << 3),
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
        inline App(std::string_view main_window_title = "");
        inline App(std::string_view main_window_title, AppFlags app_flags);
        DP_IMAPP_API App(std::string_view main_window_title, int main_window_width, int main_window_height, AppFlags app_flags = 0);
        DP_IMAPP_API ~App();

        DP_IMAPP_API void SetMainWindowMinSize(int min_with, int min_height);
        DP_IMAPP_API void SetMainWindowAspectRatio(int numerator, int denominator);

        DP_IMAPP_API double GetRunningTime() const;

        template <class F>
        int Run(F&& update_func);

        DP_IMAPP_API void BeginMainWindowContent(MainWindowFlags main_window_flags = 0);
        DP_IMAPP_API void EndMainWindowContent();

        DP_IMAPP_API void Close();

    private:
        DP_IMAPP_API int RunImpl(void (*local_init_func)(void*), const std::function<void()>& update_func);

        std::unique_ptr<detail::AppImpl> Impl;
    };

} // namespace dpImApp

#include "dpImApp/dpImApp.inl"

#endif
