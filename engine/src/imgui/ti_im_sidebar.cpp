#include <ti_pch.h>

#include <imgui.h>

static void draw_button(ImVec2 position, sidebar_tab_t current_tab, char const *label);

sidebar_tab_t g_sidebar_tab = SIDEBAR_TAB_NONE;

void sidebar_draw(void) {
  ImGui::SetNextWindowPos(ImVec2(0.0F, (float)g_window.titlebar_height));
  ImGui::SetNextWindowSize(ImVec2((float)g_window.sidebar_width, (float)g_window.window_height - (float)g_window.titlebar_height));

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
  ImGui::Begin("sidebar", 0, sidebar_flags);
  ImGui::PopStyleVar(1);

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
  ImGui::PushFont((ImFont *)g_imgui_font_symbols_32);

  draw_button(ImVec2(7.0F, 10.0F), SIDEBAR_TAB_HIERARCHY, ICON_MS_LISTS);
  draw_button(ImVec2(7.0F, 55.0F), SIDEBAR_TAB_FILESYSTEM, ICON_MS_FOLDER);
  draw_button(ImVec2(7.0F, 100.0F), SIDEBAR_TAB_HANDLE, ICON_MS_LINE_END_CIRCLE);

  ImGui::PopFont();
  ImGui::PopStyleVar(1);

  ImGui::End();
  ImGui::PopStyleColor(4);
}
void sidebar_reset(void) {
  // TODO
}

static void draw_button(ImVec2 position, sidebar_tab_t current_tab, char const *label) {
  sidebar_tab_t tab = g_sidebar_tab;

  ImGui::SetCursorPos(position);

  if (tab == current_tab) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
  }

  if (ImGui::Button(label)) {
    g_sidebar_tab = current_tab;
  }

  if (tab == current_tab) {
    ImGui::PopStyleColor(1);
  }
}
