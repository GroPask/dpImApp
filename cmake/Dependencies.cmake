if (DP_IMAPP_DOWNLOAD_MISSING_DEPENDENCIES)
    find_package(glfw3 QUIET)
    if (NOT glfw3_FOUND)
        set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
        set(GLFW_INSTALL OFF CACHE BOOL "")

        dp_download_and_add_dependency(URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip)
    endif ()
else ()
    find_package(glfw3 REQUIRED)
endif ()

function (patchImgui imguiSrcDir)
    file(DOWNLOAD
        https://raw.githubusercontent.com/conan-io/conan-center-index/e79c84ef264b967b21c72a7fa6dfeaf8a4cd2a1c/recipes/imgui/all/CMakeLists.txt
        ${imguiSrcDir}/CMakeLists.txt
    )
        
    dp_patch_file(${imguiSrcDir}/CMakeLists.txt
        ADD_LINE_BEFORE "set(MISC_DIR" "option(IMGUI_INSTALL \"Generate installation target\" ON)\n"
        ADD_LINE_BEFORE "set(MISC_DIR" "set(IMGUI_SRC_DIR ${imguiSrcDir})"
        ADD_LINE_AFTER [=[add_library(${PROJECT_NAME} ${SOURCE_FILES})]=] [=[add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})]=]
        ADD_LINE_BEFORE "include(GNUInstallDirs)" "if (IMGUI_INSTALL)"
        APPEND_LINE "endif()"
    )

    dp_patch_file(${imguiSrcDir}/imgui.h
        ADD_LINE_BEFORE "#ifdef IMGUI_USER_CONFIG" "#include \"imgui_export_headers.h\"\n"
    )
endfunction ()

if (DP_IMAPP_DOWNLOAD_MISSING_DEPENDENCIES)
    find_package(imgui QUIET)
    if (NOT imgui_FOUND)
        set(IMGUI_INSTALL OFF CACHE BOOL "")

        dp_download_and_add_dependency(
            URL https://github.com/ocornut/imgui/archive/refs/tags/v1.91.9b-docking.zip
            PATCH_FUNC patchImgui
            ALREADY_POPULATED_VAR imguiWasAlreadyPopulated
            SRC_DIR_VAR imguiSrcDir
        )
        
        if (NOT ${imguiWasAlreadyPopulated})
            file(COPY ${imguiSrcDir}/backends/ DESTINATION ${DP_IMAPP_IMGUI_BACKENDS_DIR})
        endif ()
    endif ()
else ()
    find_package(imgui REQUIRED)
endif ()

find_package(opengl_system QUIET) # Try Conan OpenGl first
if (NOT opengl_system_FOUND)
    find_package(OpenGL REQUIRED) # CMake OpenGl
    add_library(opengl::opengl ALIAS OpenGL::GL) # Create fake Conan OpenGl pointing to CMake OpenGl
endif ()

function (removeInString string indexBegin indexEnd outVar)
    string(SUBSTRING "${string}" 0 ${indexBegin} begin)
    string(SUBSTRING "${string}" ${indexEnd} -1 end)
    string(CONCAT result "${begin}" "${end}")
    set(${outVar} "${result}" PARENT_SCOPE)
endfunction ()

function (generateImGuiExampleGlfwOpenGl3MainPatched inFile outFile)
    file(COPY_FILE ${inFile} ${outFile})

    dp_patch_file(${outFile}
        ADD_LINE_BEFORE "#include \"imgui.h\"" "#include \"AppImplInterface.hpp\"\n#define IMGUI_IMPL_API\n"
        ADD_LINE_BEFORE "// [Win32] Our example" "#if IMGUI_VERSION_NUM < 19160\n    #error ImGui version not supported\n#endif\n"
        ADD_LINE_BEFORE "static void glfw_error_callback" "namespace dpImApp::detail {\n"
        ADD_LINE_BEFORE "// Main code" "void ImGuiExampleGlfwOpenGl3InitContext()\;"
        ADD_LINE_BEFORE "// Main code" "void ImGuiExampleGlfwOpenGl3CoreLoop(AppImplInterface& app_impl_interface, GLFWwindow* window)\;\n"
        REPLACE "main(int, char**)" "ImGuiExampleGlfwOpenGl3MainPatched(int main_window_width, int main_window_height, const char* main_window_title, AppImplInterface& app_impl_interface)"
        ADD_LINE_BEFORE "    // Decide GL+GLSL versions" "    ImGuiExampleGlfwOpenGl3InitContext()\;\n"
        ADD_LINE_BEFORE "    // Decide GL+GLSL versions" "    app_impl_interface.InitBeforeCreateMainWindow(main_window_width, main_window_height)\;\n"
        REPLACE "glfwCreateWindow(1280, 720, \"Dear ImGui GLFW+OpenGL3 example\"" "glfwCreateWindow(main_window_width, main_window_height, main_window_title"
        ADD_LINE_BEFORE "    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable" "    #ifdef IMGUI_HAS_DOCK"
        ADD_LINE_AFTER "// Enable Docking" "    #endif"
        ADD_LINE_BEFORE "    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable" "    #ifdef IMGUI_HAS_VIEWPORT"
        ADD_LINE_AFTER "//io.ConfigViewportsNoTaskBarIcon = true\;" "    #endif"
        ADD_LINE_BEFORE "    // When viewports are enabled" "    #ifdef IMGUI_HAS_VIEWPORT"
        ADD_LINE_AFTER "style.Colors[ImGuiCol_WindowBg].w = 1.0f\;\n    }" "    #endif"
        ADD_LINE_BEFORE "    // Our state" "    app_impl_interface.InitBeforeMainLoop(window)\;\n"
        REMOVE "    bool show_demo_window = true\\\;\n"
        REMOVE "    bool show_another_window = false\\\;\n"
        ADD_LINE_AFTER "ImGui::NewFrame()\;" "\n        app_impl_interface.Update()\;"
        ADD_LINE_BEFORE "        // Update and Render additional" "        #ifdef IMGUI_HAS_VIEWPORT"
        ADD_LINE_AFTER "(backup_current_context)\;\n        }" "        #endif"
        ADD_LINE_AFTER "glfwSwapBuffers(window)\;" "        ImGuiExampleGlfwOpenGl3CoreLoop(app_impl_interface, window)\;"
    )

    file(READ ${outFile} fileContent)
    
    string(FIND "${fileContent}" "    // Setup Dear ImGui context" initContextBeginIndex)
    string(FIND "${fileContent}" "    // Setup Platform/Renderer backends" initContextEndIndex)
    MATH(EXPR initContextLength "${initContextEndIndex}-${initContextBeginIndex}")
    string(SUBSTRING "${fileContent}" ${initContextBeginIndex} ${initContextLength} initContext)
    removeInString("${fileContent}" ${initContextBeginIndex} ${initContextEndIndex} fileContent)

    string(FIND "${fileContent}" "        // 1. Show the big demo window" demoBeginIndex)
    string(FIND "${fileContent}" "        // Rendering" demoEndIndex)
    removeInString("${fileContent}" ${demoBeginIndex} ${demoEndIndex} fileContent)

    string(FIND "${fileContent}" "    // Our state" stateBeginIndex)
    string(FIND "${fileContent}" "    // Main loop" stateEndIndex)
    MATH(EXPR stateLength "${stateEndIndex}-${stateBeginIndex}")
    string(SUBSTRING "${fileContent}" ${stateBeginIndex} ${stateLength} state)
    removeInString("${fileContent}" ${stateBeginIndex} ${stateEndIndex} fileContent)

    string(FIND "${fileContent}" "        // Start the Dear ImGui frame" coreLoopBeginIndex)
    string(FIND "${fileContent}" "        ImGuiExampleGlfwOpenGl3CoreLoop" coreLoopEndIndex)
    MATH(EXPR coreLoopLength "${coreLoopEndIndex}-${coreLoopBeginIndex}")
    string(SUBSTRING "${fileContent}" ${coreLoopBeginIndex} ${coreLoopLength} coreLoop)
    removeInString("${fileContent}" ${coreLoopBeginIndex} ${coreLoopEndIndex} fileContent)

    string(APPEND fileContent "\nvoid ImGuiExampleGlfwOpenGl3InitContext()\n{\n${initContext}}\n")
    string(APPEND fileContent "\nvoid ImGuiExampleGlfwOpenGl3CoreLoop(AppImplInterface& app_impl_interface, GLFWwindow* window)\n{\n    ImGuiIO& io = ImGui::GetIO(); (void)io;\n\n${state}${coreLoop}}\n\n} // namespace dpImApp::detail\n")

    file(WRITE ${outFile} "${fileContent}")
endfunction ()

set(DP_IMAPP_IMGUI_GLFW_OPENGL3_EXAMPLE_PATCHED_FILE ${CMAKE_CURRENT_BINARY_DIR}/generated/ImGuiExampleGlfwOpenGl3MainPatched.cpp)

if (NOT EXISTS ${DP_IMAPP_IMGUI_GLFW_OPENGL3_EXAMPLE_PATCHED_FILE})
    if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/generated)
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)
    endif ()

    if (DP_IMAPP_DOWNLOAD_MISSING_DEPENDENCIES)
        if (NOT DP_IMAPP_IMGUI_GLFW_OPENGL3_EXAMPLE_FILE STREQUAL "")
            generateImGuiExampleGlfwOpenGl3MainPatched(${DP_IMAPP_IMGUI_GLFW_OPENGL3_EXAMPLE_FILE} ${DP_IMAPP_IMGUI_GLFW_OPENGL3_EXAMPLE_PATCHED_FILE})
        else ()
            set(DP_IMAPP_IMGUI_GLFW_OPENGL3_EXAMPLE_DOWNLOADED_FILE ${CMAKE_CURRENT_BINARY_DIR}/ImGuiExampleGlfwOpenGl3Main.cpp)

            if (NOT EXISTS ${DP_IMAPP_IMGUI_GLFW_OPENGL3_EXAMPLE_DOWNLOADED_FILE})
                file(DOWNLOAD https://raw.githubusercontent.com/ocornut/imgui/refs/tags/v1.91.9b-docking/examples/example_glfw_opengl3/main.cpp ${DP_IMAPP_IMGUI_GLFW_OPENGL3_EXAMPLE_DOWNLOADED_FILE})
            endif ()
            generateImGuiExampleGlfwOpenGl3MainPatched(${DP_IMAPP_IMGUI_GLFW_OPENGL3_EXAMPLE_DOWNLOADED_FILE} ${DP_IMAPP_IMGUI_GLFW_OPENGL3_EXAMPLE_PATCHED_FILE})
        endif ()
    else ()
        generateImGuiExampleGlfwOpenGl3MainPatched(${DP_IMAPP_IMGUI_GLFW_OPENGL3_EXAMPLE_FILE} ${DP_IMAPP_IMGUI_GLFW_OPENGL3_EXAMPLE_PATCHED_FILE})
    endif ()
endif ()
