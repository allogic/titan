#include <ti_pch.h>

#include <imgui.h>

static void reset_drag_state(void);

static void draw_scene_controls(void);
static void draw_panel_controls(void);
static void draw_window_controls(void);

void titlebar_draw(void) {
  ImGui::SetNextWindowPos(ImVec2(0.0F, 0.0F));
  ImGui::SetNextWindowSize(ImVec2((float)g_window.window_width, (float)g_window.titlebar_height));

  ImGui::PushStyleColor(ImGuiCol_WindowBg, TI_LIGHT_GREY);
  ImGui::PushStyleColor(ImGuiCol_Button, TI_LIGHT_GREY);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, TI_HOVER_GREY);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, TI_ACTIVE_GREY);

  ImGuiWindowFlags titlebar_flags =
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
  ImGui::Begin("titlebar", 0, titlebar_flags);
  ImGui::PopStyleVar(1);

  // ImGui::SetCursorPos(ImVec2(10.0F, 10.0F));
  // ImGui::PushFont((ImFont *)g_imgui_font_symbols_25);
  // ImGui::Text(ICON_MS_CIRCLE);
  // ImGui::PopFont();

  draw_scene_controls();
  draw_panel_controls();
  draw_window_controls();

  reset_drag_state();

  ImGui::End();
  ImGui::PopStyleColor(4);
}
void titlebar_reset(void) {
  // TODO
}

static void reset_drag_state(void) {
  if (ImGui::IsWindowHovered() &&
      !ImGui::IsAnyItemHovered() &&
      ImGui::IsMouseDown(ImGuiMouseButton_Left)) {

    ReleaseCapture();

    SendMessage(g_window.window_handle, WM_NCLBUTTONDOWN, HTCAPTION, 0);
  }

  ImGui::GetIO().MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
}

static void draw_scene_controls(void) {
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
  ImGui::PushFont((ImFont *)g_imgui_font_symbols_18);

  ImGui::SetCursorPos(ImVec2(5.0F, 5.0F));

  if (ImGui::Button(ICON_MS_FILE_SAVE)) {
    // TODO
  }

  ImGui::SameLine();

  if (ImGui::Button(ICON_MS_FILE_OPEN)) {
    // TODO
  }

  ImGui::PopFont();
  ImGui::PopStyleVar(1);
}
static void draw_panel_controls(void) {
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
  ImGui::PushFont((ImFont *)g_imgui_font_symbols_18);

  ImGui::SetCursorPos(ImVec2((float)g_window.window_width - 200.0F, 5.0F));

  if (ImGui::Button(ICON_MS_DOCK_TO_RIGHT)) {
    // TODO: split viewport..
  }

  ImGui::SameLine();

  if (ImGui::Button(ICON_MS_DOCK_TO_LEFT)) {
    g_show_inspector = !g_show_inspector;
  }

  ImGui::PopFont();
  ImGui::PopStyleVar(1);
}
static void draw_window_controls(void) {
  ImGui::SetCursorPos(ImVec2((float)g_window.window_width - 89.0F, 5.0F));

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
  ImGui::PushFont((ImFont *)g_imgui_font_symbols_18);

  if (ImGui::Button(ICON_MS_REMOVE)) {
    ShowWindow(g_window.window_handle, SW_MINIMIZE);
  }

  ImGui::SameLine();

  if (ImGui::Button(ICON_MS_RECTANGLE)) {
    static uint8_t is_maximized = 0;

    if (is_maximized) {
      ShowWindow(g_window.window_handle, SW_RESTORE);
    } else {
      ShowWindow(g_window.window_handle, SW_MAXIMIZE);
    }

    is_maximized = !is_maximized;
  }

  ImGui::SameLine();

  if (ImGui::Button(ICON_MS_CLOSE)) {
    g_window.is_running = 0;
  }

  ImGui::PopFont();
  ImGui::PopStyleVar(1);
}
