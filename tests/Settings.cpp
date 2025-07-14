#include <dpImApp/dpImApp.hpp>

#include <imgui.h>

#include <cstdlib>

int main(int, char**)
{
    int main_result = EXIT_SUCCESS;

    for (int run_count = 0; run_count < 3; ++run_count)
    {
        int my_value = 123;

        dpImApp::App app("dpImApp_Test_Settings");

        app.SetSettingsPath(app.ComputeStandardSettingsFolder("foo/bar bar\\test/complex"), "TestImGui.ini");

        app.AddSimpleSettingsHandler(
            [&my_value](const char* line)
            {
                int value;
                if (dpImApp::SafeSscanf(line, "Value=%d\n", &value) == 1)
                    my_value = value;
            },
            [&my_value](ImGuiTextBuffer& outTextBuffer)
            {
                outTextBuffer.appendf("Value=%d\n", my_value);
            }
        );

        #ifdef __APPLE__
        // Waiting Glfw for a fix
        #else
        int iteration_count = 0;
        app.Run([&]()
        {
            if (run_count == 0 && iteration_count == 0)
            {
                if (my_value != 123)
                {
                    main_result = EXIT_FAILURE;
                    app.Close();
                }
                else
                    my_value = 42;
            }
            else if (my_value != 42)
            {
                main_result = EXIT_FAILURE;
                app.Close();
            }
            else if (iteration_count == 4)
                app.Close();

            ++iteration_count;
        });
        #endif

        if (main_result != EXIT_SUCCESS)
            return main_result;
    }

    return main_result;
}
