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
