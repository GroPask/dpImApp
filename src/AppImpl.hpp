#ifndef DP_IMAPP_APP_IMPL_HPP
#define DP_IMAPP_APP_IMPL_HPP

#include "AppImplInterface.hpp"

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

using ImGuiID = unsigned int;
struct ImGuiTextBuffer;
struct GLFWwindow;

namespace dpImApp::detail
{
    class AppImpl : public AppImplInterface
    {
    public:
        inline AppImpl(std::string_view main_window_title, int main_window_width, int main_window_height, AppFlags app_flags);

        inline std::string ComputeStandardSettingsFolder(std::string_view app_folder) const;
        inline std::string ComputeStandardSettingsFolder() const;

        inline void SetSettingsPath(std::string_view settings_folder, std::string_view settings_file_name);

        inline void AddSimpleSettingsHandler(std::string_view name, std::function<void(const char*)>&& read_func, std::function<void(ImGuiTextBuffer&)>&& write_func);
        inline void AddSimpleSettingsHandler(std::function<void(const char*)>&& read_func, std::function<void(ImGuiTextBuffer&)>&& write_func);

        inline void SetMainWindowMinSize(int min_with, int min_height);
        inline void SetMainWindowAspectRatio(int numerator, int denominator);
        inline void SetMainWindowFloating(bool floating);

        inline double GetRunningTime() const;

        inline int Run(void (*local_init_func)(void*), const std::function<void()>& update_func);

        inline void BeginMainWindowContent(MainWindowFlags main_window_flags);
        inline void EndMainWindowContent();

        inline void Close();

        void InitBeforeCreateMainWindow(int& main_window_width, int& main_window_height) override;
        void InitBeforeMainLoop(GLFWwindow* main_window) override;
        void Update() override ;

    private:
        struct SimpleSettingsHandlerInfos
        {
            std::string Name;
            std::function<void(const char*)> ReadFunc;
            std::function<void(ImGuiTextBuffer&)> WriteFunc;
        };

        void ReadMainSaveDataLine(const char* line);
        void WriteAllMainSaveData(ImGuiTextBuffer& textBuffer) const;

        void GlfwMainWindowMaximizeCallback(bool maximized);
        void GlfwMainWindowIconifyCallback(bool iconified);
        void GlfwMainWindowPosCallback(int posX, int posY);
        void GlfwMainWindowSizeCallback(int width, int height);
        void GlfwMainWindowRefreshCallback();

        std::string MainWindowTitle;
        std::pair<int, int> MainWindowSize;
        AppFlags Flags;

        std::string SettingsPath;
        std::unordered_map<ImGuiID, SimpleSettingsHandlerInfos> SimpleSettingsHandlers;

        std::optional<std::pair<int, int>> PendingMainWindowMinSize;
        std::optional<std::pair<int, int>> PendingMainWindowAspectRatio;
        std::optional<bool> PendingMainWindowFloating;

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
