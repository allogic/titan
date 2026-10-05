#include <vulkan/ti_vk_framebuffer.h>

void vk_framebuffer_create(vk_framebuffer_t *framebuffer, fs_framebuffer_t *config) {
  framebuffer->config = config;

  uint64_t image_attachment_view_count = framebuffer->config->color_attachment_count + 1;

  VkImageView *image_attachment_view = TI_ALLOC(sizeof(VkImageView) * image_attachment_view_count, 0, 0);

  framebuffer->color_attachment = (vk_image_t **)TI_ALLOC(sizeof(vk_image_t *) * framebuffer->config->color_attachment_count, 0, 0);

  uint64_t attachment_index = 0;
  uint64_t attachment_count = framebuffer->config->color_attachment_count;

  while (attachment_index < attachment_count) {

    framebuffer->color_attachment[attachment_index] = (vk_image_t *)idb_reference(framebuffer, framebuffer->config->color_attachment[attachment_index].reference_path);

    image_attachment_view[attachment_index] = framebuffer->color_attachment[attachment_index]->image_view;

    attachment_index++;
  }

  framebuffer->depth_attachment = (vk_image_t *)idb_reference(framebuffer, framebuffer->config->depth_attachment.reference_path);

  image_attachment_view[image_attachment_view_count - 1] = framebuffer->depth_attachment->image_view;

  vk_renderpass_t *renderpass = (vk_renderpass_t *)idb_reference(framebuffer, framebuffer->config->renderpass.reference_path);

  VkFramebufferCreateInfo frame_buffer_create_info = {
    .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
    .renderPass = renderpass->handle,
    .pAttachments = image_attachment_view,
    .attachmentCount = (uint32_t)image_attachment_view_count,
    .width = framebuffer->config->width,
    .height = framebuffer->config->height,
    .layers = 1,
  };

  TI_VK_CHECK(vkCreateFramebuffer(g_vk_instance.device, &frame_buffer_create_info, 0, &framebuffer->handle));

  TI_FREE(image_attachment_view);
}
void vk_framebuffer_destroy(vk_framebuffer_t *framebuffer) {
  vkDestroyFramebuffer(g_vk_instance.device, framebuffer->handle, 0);

  uint64_t attachment_index = 0;
  uint64_t attachment_count = framebuffer->config->color_attachment_count;

  while (attachment_index < attachment_count) {

    idb_dereference(framebuffer->color_attachment[attachment_index]);

    attachment_index++;
  }

  idb_dereference(framebuffer->depth_attachment[attachment_index]);

  TI_FREE(framebuffer->color_attachment);
}
