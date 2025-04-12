#include <dpImApp/dpImApp.hpp>

#include <imgui_internal.h>
#include <imgui.h>

int main(int, char**)
{
    dpImApp::App app("dpImApp_Example_DockSpace");
    return app.Run([&]()
    {
        app.BeginMainWindowContent();
 
        #ifdef IMGUI_HAS_DOCK
        const ImGuiID dock_space_id = ImGui::GetID("MyDockSpace");
        
        if (ImGui::DockBuilderGetNode(dock_space_id) == NULL)
        {
            ImGui::DockBuilderSetNodeSize(dock_space_id, ImGui::GetContentRegionAvail());
            ImGui::DockBuilderFinish(dock_space_id);
        }
        
        ImGui::DockSpace(dock_space_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        #endif

        app.EndMainWindowContent();

        if (ImGui::Begin("WindowA"))
            ImGui::Text("TextA");
        ImGui::End();

        if (ImGui::Begin("WindowB"))
            ImGui::Text("TextB");
        ImGui::End();

        if (ImGui::Begin("WindowC"))
            ImGui::Text("TextC");
        ImGui::End();

        if (ImGui::Begin("WindowD"))
            ImGui::Text("TextD");
        ImGui::End();
    });
}
