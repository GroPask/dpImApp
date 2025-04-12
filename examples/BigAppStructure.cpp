#include <dpImApp/dpImApp.hpp>

#include <imgui_internal.h>
#include <imgui.h>

int main(int, char**)
{
    dpImApp::App app("dpImApp_Example_BigAppStructure");
    return app.Run([&]()
    {
        app.BeginMainWindowContent(dpImApp::MainWindowFlag::MenuBar);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Close"))
                    app.Close();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        #ifdef IMGUI_HAS_DOCK
        const ImGuiID dock_space_id = ImGui::GetID("MyDockSpace");

        if (ImGui::DockBuilderGetNode(dock_space_id) == NULL)
        {
            ImGui::DockBuilderRemoveNode(dock_space_id);
            ImGui::DockBuilderAddNode(dock_space_id, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dock_space_id, ImGui::GetContentRegionAvail());
            
            ImGuiID dock_main_Id = dock_space_id;
            const ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_Id, ImGuiDir_Right, 0.20f, NULL, &dock_main_Id);
            const ImGuiID dock_bottom_id = ImGui::DockBuilderSplitNode(dock_main_Id, ImGuiDir_Down, 0.30f, NULL, &dock_main_Id);
            const ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_Id, ImGuiDir_Left, 0.20f, NULL, &dock_main_Id);

            ImGui::DockBuilderDockWindow("WindowA", dock_main_Id);
            ImGui::DockBuilderDockWindow("WindowB", dock_right_id);
            ImGui::DockBuilderDockWindow("WindowC", dock_bottom_id);
            ImGui::DockBuilderDockWindow("WindowD", dock_left_id);

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
