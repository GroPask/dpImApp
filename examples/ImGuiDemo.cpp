#include <dpImApp/dpImApp.hpp>

#include <imgui.h>

int main(int, char**)
{
    return dpImApp::App().Run([]()
    {
        ImGui::ShowDemoWindow();
    });
}
