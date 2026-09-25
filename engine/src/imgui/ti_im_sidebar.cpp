#include <ti_pch.h>

#include <imgui.h>

static void draw_button(ImVec2 position, im_sidebar_tab_t current_tab, char const *label);

im_sidebar_tab_t g_sidebar_tab = IM_SIDEBAR_TAB_FILESYSTEM;

void im_sidebar_draw(void) {
  ImGui::SetNextWindowPos(ImVec2(0.0F, (float)g_pl_window.titlebar_height));
  ImGui::SetNextWindowSize(ImVec2((float)g_pl_window.sidebar_width, (float)g_pl_window.window_height - (float)g_pl_window.titlebar_height));

  ImGui::PushStyleColor(ImGuiCol_WindowBg, TI_LIGHT_GREY);
  ImGui::PushStyleColor(ImGuiCol_Button, TI_LIGHT_GREY);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, TI_HOVER_GREY);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, TI_ACTIVE_GREY);

  ImGuiWindowFlags sidebar_flags =
    ImGuiWindowFlags_NoDocking |
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoScrollWithMouse |
    ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoBringToFrontOnFocus;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0F, 5.0F));
  ImGui::Begin("Sidebar", 0, sidebar_flags);
  ImGui::PopStyleVar(1);

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
  ImGui::PushFont((ImFont *)g_im_font_symbols_32);

  draw_button(ImVec2(7.0F, 10.0F), IM_SIDEBAR_TAB_HIERARCHY, ICON_MS_LIST);
  draw_button(ImVec2(7.0F, 55.0F), IM_SIDEBAR_TAB_FILESYSTEM, ICON_MS_FOLDER);
  draw_button(ImVec2(7.0F, 100.0F), IM_SIDEBAR_TAB_HANDLE, ICON_MS_LINE_END_CIRCLE);
  draw_button(ImVec2(7.0F, 145.0F), IM_SIDEBAR_TAB_RENDERER, ICON_MS_PHOTO_CAMERA);

  ImGui::PopFont();
  ImGui::PopStyleVar(1);

  ImGui::End();
  ImGui::PopStyleColor(4);
}
void im_sidebar_reset(void) {
  // TODO
}

static void draw_button(ImVec2 position, im_sidebar_tab_t current_tab, char const *label) {
  uint8_t show = g_im_show_left_panel;
  im_sidebar_tab_t tab = g_sidebar_tab;

  ImGui::SetCursorPos(position);

  if (show && (tab == current_tab)) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
  }

  if (ImGui::Button(label)) {
    if (tab == current_tab) {
      g_im_show_left_panel = !g_im_show_left_panel;
    } else {
      g_im_show_left_panel = 1;
    }

    g_sidebar_tab = current_tab;
  }

  if (show && (tab == current_tab)) {
    ImGui::PopStyleColor(1);
  }
}
