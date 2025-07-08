#include <dpImApp/dpImApp.hpp>

#include <imgui.h>

int main(int, char**)
{
    return 0;

    #ifdef 0
    dpImApp::App app("dpImApp_Test_Simple");

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
