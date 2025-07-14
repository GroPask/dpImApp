#include <dpImApp/dpImApp.hpp>

#include <imgui.h>

#include <cstdlib>
#include <cstring>

namespace
{
    void my_strncpy(char* dst, const char* src, std::size_t dst_size)
    {
        if (dst_size == 0)
            return;

        for (std::size_t i = 0; ; ++i)
        {
            if (i >= dst_size)
            {
                dst[dst_size - 1] = '\0';
                return;
            }

            dst[i] = src[i];

            if (src[i] == '\0')
                return;
        }
    }
}

int main(int, char**)
{
    int main_result = EXIT_SUCCESS;

    for (int run_count = 0; run_count < 3; ++run_count)
    {
        static constexpr std::size_t buffer_size = 64;

        int my_value = 123;

        char my_string[buffer_size];
        my_strncpy(my_string, "foo", buffer_size);

        int my_value2 = 456;

        dpImApp::App app("dpImApp_Test_Settings");

        app.SetSettingsPath(app.ComputeStandardSettingsFolder("foo/bar bar\\test/complex"), "TestImGui.ini");

        app.AddSimpleSettingsHandler(
            [&my_value, &my_string, &my_value2](const char* line)
            {
                int value;
                if (dpImApp::SafeSscanf(line, "Value=%d\n", &value) == 1)
                    my_value = value;

                char string[buffer_size];
                int value2;
                if (dpImApp::SafeSscanf(line, "String=%63s %d\n", string, buffer_size, &value2) == 2)
                {
                    my_strncpy(my_string, string, buffer_size);
                    my_value2 = value2;
                }
            },
            [&my_value, &my_string, &my_value2](ImGuiTextBuffer& outTextBuffer)
            {
                outTextBuffer.appendf("Value=%d\n", my_value);
                outTextBuffer.appendf("String=%s %d\n", my_string, my_value2);
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
                if (my_value != 123 || std::strcmp(my_string, "foo") != 0 || my_value2 != 456)
                {
                    main_result = EXIT_FAILURE;
                    app.Close();
                }
                else
                {
                    my_value = 42;
                    my_strncpy(my_string, "bar", buffer_size);
                    my_value2 = 987;
                }
            }
            else if (my_value != 42 || std::strcmp(my_string, "bar") != 0 || my_value2 != 987)
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
