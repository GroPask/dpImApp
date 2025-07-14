#include <dpImApp/dpImApp.hpp>

#include <imgui.h>

int main(int, char**)
{
    dpImApp::App app("dpImApp_Test_Simple");

    #ifdef __APPLE__
    (void)app;
    // Waiting Glfw for a fix:
    // https://github.com/glfw/glfw/issues/2570
    // https://github.com/glfw/glfw/pull/2571
    #else
    int iteration_count = 0;
    return app.Run([&]()
    {
        ImGui::Text("Hello World");
        ImGui::Button("Click Me");

        if (iteration_count == 4)
            app.Close();

        ++iteration_count;
    });
    #endif
}
