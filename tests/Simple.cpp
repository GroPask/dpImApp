#include <dpImApp/dpImApp.hpp>

#include <imgui.h>

int main(int, char**)
{
    dpImApp::App app("dpImApp_Test_Simple");

    int iteration_count = 0;
    return app.Run([&]()
    {
        ImGui::Text("Hello World");

        if (++iteration_count == 5)
            app.Close();
    });
}
