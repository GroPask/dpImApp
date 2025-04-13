cmake_minimum_required(VERSION 3.24.0)

project(dpImApp VERSION 0.0.1 LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
option(DP_IMAPP_DOWNLOAD_MISSING_DEPENDENCIES "Download missing dependencies" ON)
option(DP_IMAPP_BUILD_EXAMPLES "Build examples programs" ${PROJECT_IS_TOP_LEVEL})
option(DP_IMAPP_BUILD_TESTS "Build tests programs" ${PROJECT_IS_TOP_LEVEL})
option(DP_IMAPP_INSTALL "Generate installation target" OFF) # TODO
set(DP_IMAPP_IMGUI_BACKENDS_DIR ${CMAKE_BINARY_DIR}/ImGuiBackends CACHE PATH "ImGui backends dir")

include(FetchContent)
FetchContent_Declare(dpCMake URL https://github.com/GroPask/dpCMake/archive/refs/heads/main.zip)
FetchContent_MakeAvailable(dpCMake)
include(${dpcmake_SOURCE_DIR}/dpCMake.cmake)

dp_configure_global(
    USE_FOLDERS_IF_TOP_LEVEL
    DEFAULT_CONFIGURATIONS_IF_TOP_LEVEL Debug Release
    DEPENDENCIES_TARGETS_FOLDER Dependencies
)

include(cmake/Dependencies.cmake)

add_library(${PROJECT_NAME})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        $<BUILD_INTERFACE:${DP_IMAPP_IMGUI_BACKENDS_DIR}>
)

if (BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DP_IMAPP_SHARED)
endif ()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw opengl::opengl PUBLIC imgui::imgui)
target_sources(${PROJECT_NAME} PRIVATE conanfile.py)

add_subdirectory(cmake)
add_subdirectory(src)

if (DP_IMAPP_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

if (DP_IMAPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

dp_configure_target(${PROJECT_NAME}
    DP_WARNINGS
    AUTO_SOURCE_GROUP
)
