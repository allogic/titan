#include <ti_pch.h>

#include <imgui.h>

void scene_draw(void) {
  ImGui::Begin("Scene");

  ImGui::InputText("File Name", g_scene.file_name, TI_PATH_SIZE);
  ImGui::InputText("File Path", g_scene.file_path, TI_PATH_SIZE);

  if (ImGui::Button("Store")) {

    scene_store(&g_scene);
  }

  ImGui::SameLine();

  if (ImGui::Button("Load")) {

    scene_load(&g_scene);
  }

  ImGui::End();
}
void scene_reset(void) {
}
