function (configureGlfw)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
    set(GLFW_INSTALL ${DP_IMAPP_INSTALL} CACHE BOOL "")
endfunction ()

if (DP_IMAPP_DOWNLOAD_MISSING_DEPENDENCIES)
    find_package(glfw3 QUIET)
    if (NOT glfw3_FOUND)
        dp_download_and_add_dependency(
            URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
            CONFIGURE_FUNC configureGlfw
        )
    endif ()
else ()
    find_package(glfw3 REQUIRED)
endif ()

function (patchImgui imguiSrcDir)
    file(DOWNLOAD
        https://raw.githubusercontent.com/conan-io/conan-center-index/refs/heads/master/recipes/imgui/all/CMakeLists.txt
        ${imguiSrcDir}/CMakeLists.txt
    )
    
    string(CONCAT aliasStr [=[add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})]=] "\nset_target_properties")
    
    dp_replace_in_file(${imguiSrcDir}/CMakeLists.txt
        "set(MISC_DIR" "option(IMGUI_INSTALL \"Generate installation target\" ON)\n\nset(MISC_DIR"
        "set(MISC_DIR" "set(IMGUI_SRC_DIR ${imguiSrcDir})\nset(MISC_DIR"
        "set_target_properties" ${aliasStr}
        "include(GNUInstallDirs)" "if (IMGUI_INSTALL)\ninclude(GNUInstallDirs)"
        "PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)\nendif()" "PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)\nendif()\nendif()"
    )
endfunction ()

function (configureImgui)
    set(IMGUI_INSTALL ${DP_IMAPP_INSTALL} CACHE BOOL "")
endfunction ()

if (DP_IMAPP_DOWNLOAD_MISSING_DEPENDENCIES)
    find_package(imgui QUIET)
    if (NOT imgui_FOUND)
        dp_download_and_add_dependency(
            URL https://github.com/ocornut/imgui/archive/refs/tags/v1.91.9b-docking.zip
            PATCH_FUNC patchImgui
            CONFIGURE_FUNC configureImgui
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
