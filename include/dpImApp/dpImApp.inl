#include "dpImApp/dpImApp.hpp"

#ifdef DP_IMAPP_SHARED
// Try to not leak at all ImGui in our headers
// Begin ImGui Forward DeclarationS
struct ImGuiContext;
#ifdef IMGUI_API
    #define FAKE_IMGUI_API IMGUI_API
#else
    #define FAKE_IMGUI_API
#endif
namespace ImGui { FAKE_IMGUI_API void SetCurrentContext(ImGuiContext* ctx); }
// End ImGui Forward DeclarationS

namespace dpImApp::detail
{
    inline void LocalInitImGuiContext(void* imgui_context)
    {
        ImGui::SetCurrentContext(static_cast<ImGuiContext*>(imgui_context));
    }
}
#endif

template <class F>
int dpImApp::App::Run(F&& update_func)
{
    #ifdef DP_IMAPP_SHARED
    return RunImpl(&detail::LocalInitImGuiContext, static_cast<F&&>(update_func));
    #else
    return RunImpl(nullptr, static_cast<F&&>(update_func));
    #endif
}
