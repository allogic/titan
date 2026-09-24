#include <ti_pch.h>

#include <imgui.h>

static void check_background_clicks(void);

static void draw_background(void);
static void draw_handles(void);

static uint64_t s_selected_handle = 0;

void im_handle_draw(void) {
  ImGui::Begin("Handle", 0, ImGuiWindowFlags_NoDecoration);

  draw_background();

  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0F, 3.0F));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5.0F, 0.0F));

  draw_handles();

  ImGui::PopStyleVar(2);

  ImGui::End();
}
void im_handle_refresh(void) {
  // TODO
}
void im_handle_reset(void) {
  s_selected_handle = 0;

  im_inspector_select(IM_INSPECTOR_TYPE_NONE, 0);
}

static void check_background_clicks(void) {
  if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) &&
      ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
      ImGui::IsAnyItemHovered() == 0) {

    s_selected_handle = 0;

    im_inspector_select(IM_INSPECTOR_TYPE_NONE, 0);
  }
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
static void draw_handles(void) {
  // TODO
}
