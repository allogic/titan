#include <ti_pch.h>

void vk_renderpass_create(vk_renderpass_t *renderpass, char const *asset_path) {
  renderpass->asset.path = asset_path;

  fs_asset_load(&renderpass->asset);

  fs_renderpass_t *config = (fs_renderpass_t *)renderpass->asset.instance;

  VkAttachmentDescription color_attachment_description = {
    .format = VK_FORMAT_R8G8B8A8_UNORM,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
    .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
    .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    .initialLayout = g_vk_image_layout_table[config->initial_color_attachment_layout_index].value,
    .finalLayout = g_vk_image_layout_table[config->final_color_attachment_layout_index].value,
  };

  VkAttachmentDescription depth_attachment_description = {
    .format = VK_FORMAT_D32_SFLOAT,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
    .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
    .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    .initialLayout = g_vk_image_layout_table[config->initial_depth_attachment_layout_index].value,
    .finalLayout = g_vk_image_layout_table[config->initial_depth_attachment_layout_index].value,
  };

  VkAttachmentReference color_attachment_reference = {
    .attachment = 0,
    .layout = g_vk_image_layout_table[config->initial_color_attachment_layout_index].value, // TODO: verify this..
  };

  VkAttachmentReference depth_attachment_reference = {
    .attachment = 1,
    .layout = g_vk_image_layout_table[config->initial_depth_attachment_layout_index].value, // TODO: verify this..
  };

  VkSubpassDescription subpass_description = {
    .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
    .colorAttachmentCount = 1,
    .pColorAttachments = &color_attachment_reference,
    .pDepthStencilAttachment = &depth_attachment_reference,
  };

  VkSubpassDependency subpass_dependency = {
    .srcSubpass = VK_SUBPASS_EXTERNAL,
    .dstSubpass = 0,
    .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
    .srcAccessMask = VK_ACCESS_NONE,
    .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
  };

  VkAttachmentDescription attachment_descriptions[] = {
    color_attachment_description,
    depth_attachment_description,
  };

  VkRenderPassCreateInfo render_pass_create_info = {
    .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
    .pAttachments = attachment_descriptions,
    .attachmentCount = TI_ARRAY_COUNT(attachment_descriptions),
    .pSubpasses = &subpass_description,
    .subpassCount = 1,
    .pDependencies = &subpass_dependency,
    .dependencyCount = 1,
  };

  TI_VK_CHECK(vkCreateRenderPass(g_vk_instance.device, &render_pass_create_info, 0, &renderpass->handle));
}
void vk_renderpass_destroy(vk_renderpass_t *renderpass) {
  vkDestroyRenderPass(g_vk_instance.device, renderpass->handle, 0);

  fs_asset_destroy(&renderpass->asset);
}
