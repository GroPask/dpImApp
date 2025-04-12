#include <dpImApp/dpImApp.hpp>

#include <imgui.h>

int main(int, char**)
{
    dpImApp::App app("dpImApp_Example_CloseButton");
    return app.Run([&]()
    {
        if (ImGui::Button("Close App"))
            app.Close();
    });
}
