#include <ti_pch.h>

#include <imgui.h>

static void draw_background(void);

void im_renderer_draw(void) {
  ImGui::Begin("Renderer", 0, ImGuiWindowFlags_NoDecoration);

  draw_background();

  ImGui::End();
}
void im_renderer_refresh(void) {
  // TODO
}
void im_renderer_reset(void) {
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
