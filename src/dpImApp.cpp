#include "dpImApp/dpImApp.hpp"

#include "AppImpl.hpp"
#include "AppImpl.inl"

dpImApp::App::App(std::string_view main_window_title, AppFlags app_flags) :
    Impl(std::make_unique<detail::AppImpl>(main_window_title, app_flags))
{
}

dpImApp::App::~App() = default;

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
