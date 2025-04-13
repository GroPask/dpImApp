#include "dpImApp/dpImApp.hpp"

#include "ImGuiInfos.hpp"

#if defined(DP_IMAPP_SHARED) && defined(DP_IMAPP_IMGUI_SEEMS_STATIC)
// Try to not leak at all ImGui in our headers
// Begin ImGui Forward DeclarationS
struct ImGuiContext;
namespace ImGui { IMGUI_API void SetCurrentContext(ImGuiContext* ctx); }
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
    #if defined(DP_IMAPP_SHARED) && defined(DP_IMAPP_IMGUI_SEEMS_STATIC)
    return RunImpl(&detail::LocalInitImGuiContext, static_cast<F&&>(update_func));
    #else
    return RunImpl(nullptr, static_cast<F&&>(update_func));
    #endif
}
