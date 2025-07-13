#include "dpImApp/dpImApp.hpp"

#include "AppImpl.hpp"
#include "AppImpl.inl"

dpImApp::App::App(std::string_view main_window_title, int main_window_width, int main_window_height, AppFlags app_flags) :
    Impl(std::make_unique<detail::AppImpl>(main_window_title, main_window_width, main_window_height, app_flags))
{
}

dpImApp::App::~App() = default;

std::string dpImApp::App::ComputeStandardSettingsFolder(std::string_view app_folder) const
{
    return Impl->ComputeStandardSettingsFolder(app_folder);
}

void dpImApp::App::SetSettingsPath(std::string_view settings_folder, std::string_view settings_file_name)
{
    Impl->SetSettingsPath(settings_folder, settings_file_name);
}

void dpImApp::App::SetMainWindowMinSize(int min_with, int min_height)
{
    Impl->SetMainWindowMinSize(min_with, min_height);
}

void dpImApp::App::SetMainWindowAspectRatio(int numerator, int denominator)
{
    Impl->SetMainWindowAspectRatio(numerator, denominator);
}

void dpImApp::App::SetMainWindowFloating(bool floating)
{
    Impl->SetMainWindowFloating(floating);
}

double dpImApp::App::GetRunningTime() const
{
    return Impl->GetRunningTime();
}

void dpImApp::App::BeginMainWindowContent(MainWindowFlags main_window_flags)
{
    Impl->BeginMainWindowContent(main_window_flags);
}

void dpImApp::App::EndMainWindowContent()
{
    Impl->EndMainWindowContent();
}

void dpImApp::App::Close()
{
    Impl->Close();
}

int dpImApp::App::RunImpl(void (*local_init_func)(void*), const std::function<void()>& update_func)
{
    return Impl->Run(local_init_func, update_func);
}
