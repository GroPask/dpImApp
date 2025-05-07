set(srcDir .)
set(incDir ../include/dpImApp)

target_sources(${PROJECT_NAME} PRIVATE CMakeLists.txt
    ${srcDir}/AppImpl.hpp
    ${srcDir}/AppImpl.inl
    ${srcDir}/AppImplInterface.hpp
    ${incDir}/Config.hpp
    ${srcDir}/dpImApp.cpp
    ${incDir}/dpImApp.hpp
    ${incDir}/dpImApp.inl
    ${incDir}/ImGuiInfos.hpp
    ${srcDir}/ImGuiGlfwBackend.cpp
    ${srcDir}/ImGuiOpenGl3Backend.cpp
)

if (WIN32)
    if (BUILD_SHARED_LIBS)
        add_library(${PROJECT_NAME}Main STATIC)
        add_library(${PROJECT_NAME}Main::${PROJECT_NAME}Main ALIAS ${PROJECT_NAME}Main)
        target_sources(${PROJECT_NAME}Main PRIVATE ${srcDir}/Win32Main.cpp)
        target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}Main::${PROJECT_NAME}Main)
    else ()
        target_sources(${PROJECT_NAME} PRIVATE ${srcDir}/Win32Main.cpp)
    endif ()
endif ()
