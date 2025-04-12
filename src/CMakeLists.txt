set(srcDir .)
set(incDir ../include/dpImApp)

target_sources(${PROJECT_NAME} PRIVATE CMakeLists.txt
    ${srcDir}/dpImApp.cpp
    ${incDir}/dpImApp.hpp
    ${incDir}/dpImApp.inl
    ${incDir}/Export.hpp
    ${srcDir}/ImGuiExampleGlfwOpenGl3MainPatched.cpp
    ${srcDir}/ImGuiGlfwBackend.cpp
    ${srcDir}/ImGuiOpenGl3Backend.cpp
)

if (WIN32 AND NOT BUILD_SHARED_LIBS)
    target_sources(${PROJECT_NAME} PRIVATE
        ${srcDir}/SystemSpecificWin32.cpp
    )
endif ()
