#include <ti_pch.h>

#include <imgui.h>
#include <imgui_internal.h>

uint8_t g_layout_init = 1;

void layout_build(void) {
  if (g_layout_init) {

    g_layout_init = 0;

    ImGuiViewport *viewport = ImGui::GetMainViewport();

    ImGuiID dockspace_id = ImGui::GetID("main_dockspace");

    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

    ImGuiID dock_main = dockspace_id;
    ImGuiID dock_left;
    ImGuiID dock_right;
    ImGuiID dock_bottom;

    dock_left = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left, 0.20F, 0, &dock_main);
    dock_right = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.25F, 0, &dock_main);
    dock_bottom = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Down, 0.30F, 0, &dock_main);

    ImGui::DockBuilderDockWindow("Scene", dock_main);
    ImGui::DockBuilderDockWindow("Hierarchy", dock_left);
    ImGui::DockBuilderDockWindow("Inspector", dock_right);
    ImGui::DockBuilderDockWindow("Filesystem", dock_bottom);

    ImGui::DockBuilderFinish(dockspace_id);
  }
}
