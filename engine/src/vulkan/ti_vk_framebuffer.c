#include <ti_pch.h>

void vk_framebuffer_create(vk_framebuffer_t *framebuffer, vk_renderpass_t *renderpass, char const *asset_path) {
  framebuffer->config = (fs_framebuffer_t *)fs_get(asset_path);

  uint32_t image_index = 0;
  uint32_t image_count = g_vk_swapchain.image_count;

  while (image_index < image_count) {

    vk_image_create(&framebuffer->color_attachment[image_index], framebuffer->width, framebuffer->height, 1, framebuffer->config->color_attachment_image);
    vk_image_create(&framebuffer->depth_attachment[image_index], framebuffer->width, framebuffer->height, 1, framebuffer->config->depth_attachment_image);

    VkImageView image_attachments[] = {
      framebuffer->color_attachment[image_index].image_view,
      framebuffer->depth_attachment[image_index].image_view,
    };

    VkFramebufferCreateInfo frame_buffer_create_info = {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .renderPass = renderpass->handle,
      .pAttachments = image_attachments,
      .attachmentCount = TI_ARRAY_COUNT(image_attachments),
      .width = framebuffer->width,
      .height = framebuffer->height,
      .layers = 1,
    };

    TI_VK_CHECK(vkCreateFramebuffer(g_vk_instance.device, &frame_buffer_create_info, 0, &framebuffer->handle[image_index]));

    image_index++;
  }
}
void vk_framebuffer_destroy(vk_framebuffer_t *framebuffer) {
  uint32_t image_index = 0;
  uint32_t image_count = g_vk_swapchain.image_count;

  while (image_index < image_count) {

    vkDestroyFramebuffer(g_vk_instance.device, framebuffer->handle[image_index], 0);

    vk_image_destroy(&framebuffer->color_attachment[image_index]);
    vk_image_destroy(&framebuffer->depth_attachment[image_index]);

    image_index++;
  }
}
