#include <dpImApp/dpImApp.hpp>

#include <imgui.h>

int main(int, char**)
{
    dpImApp::App app("dpImApp_Test_Simple");
    (void)app;

    #ifdef _WIN32
    int iteration_count = 0;
    return app.Run([&]()
    {
        ImGui::Text("Hello World");
        ImGui::Button("Click Me");

        if (++iteration_count == 5)
            app.Close();
    });
    #endif
}
