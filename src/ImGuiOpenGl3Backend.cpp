// Include .cpp to avoid putting it in the project and thus be able to disable warning on it without modifying it

#pragma warning(push, 0)
#define IMGUI_IMPL_API
#include <imgui_impl_opengl3.cpp>
#pragma warning(pop)
