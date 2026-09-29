#include <ti_pch.h>

#include <imgui.h>

static void reset_drag_state(void);

static void draw_title(void);
static void draw_scene_controls(void);
static void draw_panel_controls(void);
static void draw_window_controls(void);

void im_titlebar_draw(void) {
  if (g_pl_window.is_maximized) {
    ImGui::SetNextWindowPos(ImVec2((float)g_pl_window.h_border_padding, (float)g_pl_window.v_border_padding));
    ImGui::SetNextWindowSize(ImVec2((float)g_pl_window.width - (float)g_pl_window.h_border_padding * 2, (float)g_pl_window.titlebar_height - (float)g_pl_window.v_border_padding));
  } else {
    ImGui::SetNextWindowPos(ImVec2(0.0F, 0.0F));
    ImGui::SetNextWindowSize(ImVec2((float)g_pl_window.width, (float)g_pl_window.titlebar_height));
  }

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
  ImGui::Begin("Titlebar", 0, titlebar_flags);
  ImGui::PopStyleVar(1);

  // ImGui::SetCursorPos(ImVec2(10.0F, 10.0F));
  // ImGui::PushFont((ImFont *)g_imgui_font_symbols_25);
  // ImGui::Text(ICON_MS_CIRCLE);
  // ImGui::PopFont();

  draw_title();

  draw_scene_controls();
  draw_panel_controls();
  draw_window_controls();

  reset_drag_state();

  ImGui::End();
  ImGui::PopStyleColor(4);
}
void im_titlebar_reset(void) {
  // TODO
}

static void reset_drag_state(void) {
  if (ImGui::IsWindowHovered() &&
      !ImGui::IsAnyItemHovered() &&
      ImGui::IsMouseDown(ImGuiMouseButton_Left)) {

    ReleaseCapture();

    SendMessage(g_pl_window.window_handle, WM_NCLBUTTONDOWN, HTCAPTION, 0);
  }

  ImGui::GetIO().MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
}

static void draw_title(void) {
  ImGui::SetCursorPos(ImVec2(16.0F, 11.0F));

  ImGui::Text("%s %s.%s.%s (%s) - %d FPS",
              g_pl_window.title,
              VERSION_MAJOR,
              VERSION_MINOR,
              VERSION_PATCH,
              GIT_VERSION_HASH,
              g_pl_window.final_fps_counter);
}
static void draw_scene_controls(void) {
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
  ImGui::PushFont((ImFont *)g_im_font_symbols_18);

  ImGui::SetCursorPos(ImVec2(350.0F, 5.0F));

  if (ImGui::Button(ICON_MS_NEW_WINDOW)) {

    // TODO
  }

  ImGui::SameLine();

  if (ImGui::Button(ICON_MS_SAVE)) {

    // TODO
  }

  ImGui::SameLine();

  if (ImGui::Button(ICON_MS_PLAY_ARROW)) {

    scene_play(&g_scene);
  }

  ImGui::SameLine();

  if (ImGui::Button(ICON_MS_STOP)) {

    scene_stop(&g_scene);
  }

  ImGui::PopFont();
  ImGui::PopStyleVar(1);
}
static void draw_panel_controls(void) {
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
  ImGui::PushFont((ImFont *)g_im_font_symbols_18);

  if (g_pl_window.is_maximized) {
    ImGui::SetCursorPos(ImVec2((float)g_pl_window.width - ((float)g_pl_window.h_border_padding * 2) - 200.0F, 5.0F));
  } else {
    ImGui::SetCursorPos(ImVec2((float)g_pl_window.width - 200.0F, 5.0F));
  }

  if (ImGui::Button(ICON_MS_DOCK_TO_RIGHT)) {
    g_im_show_left_panel = !g_im_show_left_panel;
  }

  ImGui::SameLine();

  if (ImGui::Button(ICON_MS_DOCK_TO_LEFT)) {
    g_im_show_right_panel = !g_im_show_right_panel;
  }

  ImGui::PopFont();
  ImGui::PopStyleVar(1);
}
static void draw_window_controls(void) {
  if (g_pl_window.is_maximized) {
    ImGui::SetCursorPos(ImVec2((float)g_pl_window.width - ((float)g_pl_window.h_border_padding * 2) - 89.0F, 5.0F));
  } else {
    ImGui::SetCursorPos(ImVec2((float)g_pl_window.width - 89.0F, 5.0F));
  }

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
  ImGui::PushFont((ImFont *)g_im_font_symbols_18);

  if (ImGui::Button(ICON_MS_REMOVE)) {
    ShowWindow(g_pl_window.window_handle, SW_MINIMIZE);
  }

  ImGui::SameLine();

  if (ImGui::Button(ICON_MS_RECTANGLE)) {

    if (g_pl_window.is_maximized) {
      ShowWindow(g_pl_window.window_handle, SW_RESTORE);
    } else {
      ShowWindow(g_pl_window.window_handle, SW_MAXIMIZE);
    }

    g_pl_window.is_maximized = !g_pl_window.is_maximized;
  }

  ImGui::SameLine();

  if (ImGui::Button(ICON_MS_CLOSE)) {
    g_pl_window.is_running = 0;
  }

  ImGui::PopFont();
  ImGui::PopStyleVar(1);
}
