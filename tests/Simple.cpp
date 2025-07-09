#include <dpImApp/dpImApp.hpp>

#include <imgui.h>

int main(int, char**)
{
    dpImApp::App app("dpImApp_Test_Simple");
    (void)app;

    #if defined(_WIN32) || defined(__linux__)
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
