#include <ti_pch.h>

void vk_framebuffer_create(vk_framebuffer_t *framebuffer, vk_renderpass_t *renderpass, char const *asset_path) {
  framebuffer->asset.path = asset_path;

  fs_asset_load(&framebuffer->asset);

  fs_framebuffer_t *config = (fs_framebuffer_t *)framebuffer->asset.instance;

  uint64_t final_image_attachment_view_count = config->color_attachment_count + 1;

  VkImageView *final_image_attachment_views = TI_ALLOC(sizeof(VkImageView) * final_image_attachment_view_count, 0, 0);

  uint32_t image_index = 0;
  uint32_t image_count = g_vk_swapchain.image_count;

  while (image_index < image_count) {

    framebuffer->color_attachment[image_index] = (vk_image_t *)TI_ALLOC(sizeof(vk_image_t) * config->color_attachment_count, 0, 0);

    uint64_t attachment_index = 0;
    uint64_t attachment_count = config->color_attachment_count;

    while (attachment_index < attachment_count) {

      vk_image_create(&framebuffer->color_attachment[image_index][attachment_index], framebuffer->width, framebuffer->height, 1, config->color_attachments[attachment_index].reference_path);

      final_image_attachment_views[attachment_index] = framebuffer->color_attachment[image_index][attachment_index].image_view;

      attachment_index++;
    }

    vk_image_create(&framebuffer->depth_attachment[image_index], framebuffer->width, framebuffer->height, 1, config->depth_attachment.reference_path);

    final_image_attachment_views[final_image_attachment_view_count - 1] = framebuffer->depth_attachment[image_index].image_view;

    VkFramebufferCreateInfo frame_buffer_create_info = {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .renderPass = renderpass->handle,
      .pAttachments = final_image_attachment_views,
      .attachmentCount = (uint32_t)final_image_attachment_view_count,
      .width = framebuffer->width,
      .height = framebuffer->height,
      .layers = 1,
    };

    TI_VK_CHECK(vkCreateFramebuffer(g_vk_instance.device, &frame_buffer_create_info, 0, &framebuffer->handle[image_index]));

    image_index++;
  }

  TI_FREE(final_image_attachment_views);
}
void vk_framebuffer_destroy(vk_framebuffer_t *framebuffer) {
  fs_framebuffer_t *config = (fs_framebuffer_t *)framebuffer->asset.instance;

  uint32_t image_index = 0;
  uint32_t image_count = g_vk_swapchain.image_count;

  while (image_index < image_count) {

    vkDestroyFramebuffer(g_vk_instance.device, framebuffer->handle[image_index], 0);

    uint64_t attachment_index = 0;
    uint64_t attachment_count = config->color_attachment_count;

    while (attachment_index < attachment_count) {

      vk_image_destroy(&framebuffer->color_attachment[image_index][attachment_index]);

      attachment_index++;
    }

    vk_image_destroy(&framebuffer->depth_attachment[image_index]);

    TI_FREE(framebuffer->color_attachment[image_index]);

    image_index++;
  }

  fs_asset_destroy(&framebuffer->asset);
}
