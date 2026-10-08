#include <imgui/ti_im_viewport.h>

#include <imgui.h>
#include <imgui_impl_vulkan.h>

static void draw_background(void);
static void draw_controls(void);
static void draw_viewport(vk_viewport_t *viewport);

void im_viewport_update(vk_viewport_t *viewport) {
  {
    if (viewport->color_attachment != VK_NULL_HANDLE) {
      ImGui_ImplVulkan_RemoveTexture(viewport->color_attachment);
    }

    if (viewport->depth_attachment != VK_NULL_HANDLE) {
      ImGui_ImplVulkan_RemoveTexture(viewport->depth_attachment);
    }
  }

  {
    vk_framebuffer_t *main_framebuffer = (vk_framebuffer_t *)g_vk_renderer->main_framebuffer_hdl->instance;
    vk_image_t *color_attachment = (vk_image_t *)main_framebuffer->color_attachment_hdl[0]->instance;
    vk_image_t *depth_attachment = (vk_image_t *)main_framebuffer->depth_attachment_hdl->instance;

    viewport->color_attachment = ImGui_ImplVulkan_AddTexture(color_attachment->image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    viewport->depth_attachment = ImGui_ImplVulkan_AddTexture(depth_attachment->image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  }
}
void im_viewport_draw(vk_viewport_t *viewport) {
  ImGui::Begin("Viewport", 0, ImGuiWindowFlags_NoDecoration);

  bool focused = ImGui::IsWindowFocused();
  bool hovered = ImGui::IsWindowHovered();

  if (focused) {

    // TODO: make escapable viewport so controls work only as long
    //       as the window is focused..

    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {

      // TODO
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {

      // TODO
    }
  }

  draw_background();
  draw_controls();
  draw_viewport(viewport);

  ImGui::End();
}
void im_viewport_refresh(vk_viewport_t *viewport) {
  // TODO
}
void im_viewport_reset(vk_viewport_t *viewport) {
  if (viewport->color_attachment != VK_NULL_HANDLE) {
    ImGui_ImplVulkan_RemoveTexture(viewport->color_attachment);
  }

  if (viewport->depth_attachment != VK_NULL_HANDLE) {
    ImGui_ImplVulkan_RemoveTexture(viewport->depth_attachment);
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
static void draw_controls(void) {
  /*
  ImGui::InputText("File Name", g_scene.file_name, TI_PATH_SIZE);
  ImGui::InputText("File Path", g_scene.file_path, TI_PATH_SIZE);

  if (ImGui::Button("Store")) {

    scene_store(&g_scene);
  }

  ImGui::SameLine();

  if (ImGui::Button("Load")) {

    scene_load(&g_scene);
  }
  */
}
static void draw_viewport(vk_viewport_t *viewport) {
  ImVec2 window_position = ImGui::GetWindowPos();
  ImVec2 window_size = ImGui::GetWindowSize();
  ImVec2 mouse_position = ImGui::GetMousePos();
  ImVec2 screen_position = ImGui::GetCursorScreenPos();

  viewport->mouse_position_x = (uint32_t)(mouse_position.x - screen_position.x);
  viewport->mouse_position_y = (uint32_t)(mouse_position.y - screen_position.y);

  if ((window_size.x != viewport->width) || (window_size.y != viewport->height)) {

    viewport->width = (uint32_t)window_size.x;
    viewport->height = (uint32_t)window_size.y;

    vk_framebuffer_t *main_framebuffer = (vk_framebuffer_t *)g_vk_renderer->main_framebuffer_hdl->instance;

    main_framebuffer->is_dirty = 1;
  }

  ImDrawList *draw = ImGui::GetWindowDrawList();

  draw->AddImageRounded(
    viewport->color_attachment,
    window_position,
    ImVec2(window_position.x + window_size.x, window_position.y + window_size.y),
    ImVec2(0.0F, 0.0F),
    ImVec2(1.0F, 1.0F),
    IM_COL32_WHITE,
    5.0F,
    ImDrawFlags_RoundCornersAll);
}
