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
        inline void SetMainWindowAspectRatio(int numerator, int denominator);

        inline int Run(void (*local_init_func)(void*), const std::function<void()>& update_func);

        inline void BeginMainWindowContent(MainWindowFlags main_window_flags);
        inline void EndMainWindowContent();

        inline void Close();

        void InitBeforeCreateMainWindow(int& main_window_width, int& main_window_height) override;
        void InitBeforeMainLoop(GLFWwindow* main_window) override;
        void Update() override ;

    private:
        void ReadMainSaveDataLine(const char* line);
        void WriteAllMainSaveData(ImGuiTextBuffer& textBuffer) const;

        void GlfwMainWindowMaximizeCallback(bool maximized);
        void GlfwMainWindowIconifyCallback(bool iconified);
        void GlfwMainWindowPosCallback(int posX, int posY);
        void GlfwMainWindowSizeCallback(int width, int height);
        void GlfwMainWindowRefreshCallback();

        std::string MainWindowTitle;
        AppFlags Flags;

        std::optional<std::pair<int, int>> PendingMainWindowMinSize;
        std::optional<std::pair<int, int>> PendingMainWindowAspectRatio;

        void (*LocalInitFunc)(void*) = nullptr;
        const std::function<void()>* UpdateFunc = nullptr;

        bool IsRunning = false;
        unsigned int FrameCount = 0;
        GLFWwindow* MainWindow = nullptr;

        std::optional<std::pair<int, int>> MainWindowPosFromSave;
        std::optional<std::pair<int, int>> MainWindowSizeFromSave;

        bool MainWindowMaximized = false;
        std::optional<bool> PendingNewMainWindowMaximized;
        bool MainWindowJustUnmaximized = false;

        bool MainWindowIconified = false;
        std::optional<bool> PendingNewMainWindowIconified;
        bool MainWindowJustUniconified = false;

        std::optional<std::pair<int, int>> PendingNewMainWindowPos;
        std::pair<int, int> MainWindowNotMaximizedNotIconifiedPos{ 0, 0 };

        std::optional<std::pair<int, int>> PendingNewMainWindowSize;
        std::pair<int, int> MainWindowNotMaximizedNotIconifiedSize{ 0, 0 };
    };

} // namespace dpImApp::detail

#endif
