#include <ti_pch.h>

#include <imgui.h>
#include <imgui_impl_vulkan.h>

static void draw_background(void);
static void draw_controls(void);
static void draw_viewport(void);

static uint8_t s_is_dirty = 1;

static VkDescriptorSet *s_color_attachments = 0;
static VkDescriptorSet *s_depth_attachments = 0;

void im_viewport_draw(void) {
  ImGui::Begin("Viewport", 0, ImGuiWindowFlags_NoDecoration);

  draw_background();
  draw_controls();
  draw_viewport();

  ImGui::End();
}
void im_viewport_refresh(void) {
  // TODO
}
void im_viewport_reset(void) {
  uint64_t image_index = 0;
  uint64_t image_count = g_vk_swapchain.image_count;

  while (image_index < image_count) {

    if (s_color_attachments) {
      ImGui_ImplVulkan_RemoveTexture(s_color_attachments[image_index]);
    }

    if (s_depth_attachments) {
      ImGui_ImplVulkan_RemoveTexture(s_depth_attachments[image_index]);
    }

    image_index++;
  }

  if (s_color_attachments) {
    TI_FREE(s_color_attachments);
  }

  if (s_depth_attachments) {
    TI_FREE(s_depth_attachments);
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
static void draw_viewport(void) {
  if (s_is_dirty) {

    s_is_dirty = 0;

    {
      uint64_t image_index = 0;
      uint64_t image_count = g_vk_swapchain.image_count;

      while (image_index < image_count) {

        if (s_color_attachments) {
          ImGui_ImplVulkan_RemoveTexture(s_color_attachments[image_index]);
        }

        if (s_depth_attachments) {
          ImGui_ImplVulkan_RemoveTexture(s_depth_attachments[image_index]);
        }

        image_index++;
      }

      if (s_color_attachments) {
        TI_FREE(s_color_attachments);
      }

      if (s_depth_attachments) {
        TI_FREE(s_depth_attachments);
      }
    }

    {
      s_color_attachments = (VkDescriptorSet *)TI_ALLOC(sizeof(VkDescriptorSet) * g_vk_swapchain.image_count, 0, 0);
      s_depth_attachments = (VkDescriptorSet *)TI_ALLOC(sizeof(VkDescriptorSet) * g_vk_swapchain.image_count, 0, 0);

      uint64_t image_index = 0;
      uint64_t image_count = g_vk_swapchain.image_count;

      while (image_index < image_count) {

        s_color_attachments[image_index] = ImGui_ImplVulkan_AddTexture(g_vk_main_framebuffer.color_attachment[image_index].image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        s_depth_attachments[image_index] = ImGui_ImplVulkan_AddTexture(g_vk_main_framebuffer.depth_attachment[image_index].image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        image_index++;
      }
    }
  }

  ImVec2 position = ImGui::GetWindowPos();
  ImVec2 size = ImGui::GetWindowSize();

  ImDrawList *draw = ImGui::GetWindowDrawList();

  draw->AddImageRounded(
    s_color_attachments[g_vk_renderer.image_index], // TODO
    position,
    ImVec2(position.x + size.x, position.y + size.y),
    ImVec2(0.0F, 0.0F),
    ImVec2(1.0F, 1.0F),
    IM_COL32_WHITE,
    5.0F,
    ImDrawFlags_RoundCornersAll);
}
