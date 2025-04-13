#ifndef DP_IMAPP_APP_IMPL_HPP
#define DP_IMAPP_APP_IMPL_HPP

#include "AppImplInterface.hpp"

#include <functional>
#include <string>
#include <string_view>

struct ImGuiTextBuffer;
struct GLFWwindow;

namespace dpImApp::detail
{
    class AppImpl : public AppImplInterface
    {
    public:
        inline AppImpl(std::string_view main_window_title, AppFlags app_flags);

        inline int Run(void (*local_init_func)(void*), const std::function<void()>& update_func);

        inline void BeginMainWindowContent(MainWindowFlags main_window_flags);
        inline void EndMainWindowContent();

        inline void Close();

        void InitBeforeCreateMainWindow() override;
        void InitBeforeMainLoop(GLFWwindow* main_window) override;
        void Update() override ;

    private:
        void ReadMainSaveDataLine(const char* line);
        void WriteAllMainSaveData(ImGuiTextBuffer& textBuffer) const;

        std::string MainWindowTitle;
        AppFlags Flags;

        void (*LocalInitFunc)(void*) = nullptr;
        const std::function<void()>* UpdateFunc = nullptr;

        bool IsRunning = false;
        GLFWwindow* MainWindow = nullptr;
    };

} // namespace dpImApp::detail

#endif
