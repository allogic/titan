#include <ti_pch.h>

#include <imgui.h>

static void draw_background(void);
static void draw_controls(void);
static void draw_viewport(void);

void viewport_draw(void) {
  ImGui::Begin("Viewport", 0, ImGuiWindowFlags_NoDecoration);

  draw_background();
  draw_controls();
  draw_viewport();

  ImGui::End();
}
void viewport_refresh(void) {
  // TODO
}
void viewport_reset(void) {
  // TODO
}

static void draw_background(void) {
  ImDrawList *draw = ImGui::GetWindowDrawList();

  ImVec2 position = ImGui::GetWindowPos();
  ImVec2 size = ImGui::GetWindowSize();

  draw->AddRectFilled(
    position,
    ImVec2(position.x + size.x, position.y + size.y),
    TI_DARK_GREY,
    5.0F,
    ImDrawFlags_RoundCornersAll);
}
static void draw_controls(void) {
  ImGui::InputText("File Name", g_scene.file_name, TI_PATH_SIZE);
  ImGui::InputText("File Path", g_scene.file_path, TI_PATH_SIZE);

  if (ImGui::Button("Store")) {

    scene_store(&g_scene);
  }

  ImGui::SameLine();

  if (ImGui::Button("Load")) {

    scene_load(&g_scene);
  }
}
static void draw_viewport(void) {
  VkDescriptorSet gbuffer_image = 0;

  ImVec2 image_position_min = ImGui::GetCursorScreenPos();
  // image_position_min.y -= 4.0F;

  ImVec2 image_position_max = ImVec2(image_position_min.x + (float)g_window.window_width, image_position_min.y + (float)g_window.window_height);
  // image_position_max.y -= controlbar_size.y;

  ImDrawList *draw = ImGui::GetWindowDrawList();

  // draw->AddImageRounded(
  //   gbuffer_image,
  //   image_position_min,
  //   image_position_max,
  //   ImVec2(0.0F, 0.0F),
  //   ImVec2(1.0F, 1.0F),
  //   IM_COL32_WHITE,
  //   0.0F);
}
