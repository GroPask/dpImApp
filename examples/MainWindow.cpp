#include <dpImApp/dpImApp.hpp>

#include <imgui.h>

int main(int, char**)
{
    dpImApp::App app("dpImApp_Example_MainWindow");
    return app.Run([&]()
    {
        app.BeginMainWindowContent();
        ImGui::Text("Hello");
        app.EndMainWindowContent();

        if (ImGui::Begin("Window"))
            ImGui::Text("Text");
        ImGui::End();
    });
}
