#include <vulkan/ti_vk_framebuffer.h>

void vk_framebuffer_create(vk_framebuffer_t *framebuffer) {
  cJSON *color_attachment = TI_JSON_ITEM(framebuffer->config, "color_attachment");

  uint32_t attachment_index = 0;
  uint32_t attachment_count = TI_JSON_ARRAY_COUNT(color_attachment);

  VkImageView *attachment_view = TI_ALLOC(sizeof(VkImageView) * (attachment_count + 1), 0, 0);

  framebuffer->color_attachment_hdl = (asset_handle_t **)TI_ALLOC(sizeof(asset_handle_t *) * attachment_count, 0, 0);

  while (attachment_index < attachment_count) {

    framebuffer->color_attachment_hdl[attachment_index] = adb_handle(framebuffer->hash, TI_JSON_ARRAY_STRING(color_attachment, attachment_index));

    attachment_view[attachment_index] = ((vk_image_t *)framebuffer->color_attachment_hdl[attachment_index]->instance)->image_view;

    attachment_index++;
  }

  framebuffer->depth_attachment_hdl = adb_handle(framebuffer->hash, TI_JSON_STRING(framebuffer->config, "depth_attachment"));

  attachment_view[attachment_count] = ((vk_image_t *)framebuffer->depth_attachment_hdl->instance)->image_view;

  asset_handle_t *renderpass_hdl = adb_handle(framebuffer->hash, TI_JSON_STRING(framebuffer->config, "renderpass"));

  VkFramebufferCreateInfo frame_buffer_create_info = {
    .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
    .renderPass = ((vk_renderpass_t *)renderpass_hdl->instance)->renderpass,
    .pAttachments = attachment_view,
    .attachmentCount = attachment_count + 1,
    .width = TI_JSON_INT(framebuffer->config, "width"),
    .height = TI_JSON_INT(framebuffer->config, "height"),
    .layers = 1,
  };

  TI_VK_CHECK(vkCreateFramebuffer(g_vk_instance->device, &frame_buffer_create_info, 0, &framebuffer->framebuffer));

  TI_FREE(attachment_view);
}
void vk_framebuffer_destroy(vk_framebuffer_t *framebuffer) {
  vkDestroyFramebuffer(g_vk_instance->device, framebuffer->framebuffer, 0);

  TI_FREE(framebuffer->color_attachment_hdl);
}
