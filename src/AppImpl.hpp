#ifndef DP_IMAPP_APP_IMPL_HPP
#define DP_IMAPP_APP_IMPL_HPP

#include "AppImplInterface.hpp"

#include <functional>
#include <optional>
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

        inline void SetMainWindowMinSize(int min_with, int min_height);

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

        void GlfwMainWindowMaximizeCallback(bool maximized);
        void GlfwMainWindowPosCallback(int posX, int posY);
        void GlfwMainWindowRefreshCallback();

        std::string MainWindowTitle;
        AppFlags Flags;

        std::optional<std::pair<int, int>> PendingMainWindowMinSize;

        void (*LocalInitFunc)(void*) = nullptr;
        const std::function<void()>* UpdateFunc = nullptr;

        bool IsRunning = false;
        unsigned int FrameCount = 0;
        GLFWwindow* MainWindow = nullptr;

        bool MainWindowMaximized = false;
        std::optional<bool> PendingNewMainWindowMaximized;

        bool MainWindowJustUnmaximized = false;

        std::pair<int, int> MainWindowNotMaximizedPos{ 0, 0 };
        std::optional<std::pair<int, int>> PendingNewMainWindowPos;
    };

} // namespace dpImApp::detail

#endif
