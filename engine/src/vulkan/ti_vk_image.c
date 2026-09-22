#include <ti_pch.h>

void vk_image_create(vk_image_t *image, uint32_t width, uint32_t height, uint32_t depth, char const *asset_path) {
  image->config = (fs_image_t *)fs_get(asset_path);

  VkImageCreateInfo image_create_info = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .imageType = image->config->image_type,
    .extent = {
      .width = image->config->width ? image->config->width : width,     // TODO
      .height = image->config->height ? image->config->height : height, // TODO
      .depth = image->config->depth ? image->config->depth : depth,     // TODO
    },
    .mipLevels = image->config->mip_levels,
    .arrayLayers = 1,
    .format = image->config->format,
    .tiling = image->config->image_tiling,
    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    .usage = image->config->image_usage_flags,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };

  TI_VK_CHECK(vkCreateImage(g_vk_instance.device, &image_create_info, 0, &image->handle));

  VkMemoryRequirements memory_requirements = {0};

  vkGetImageMemoryRequirements(g_vk_instance.device, image->handle, &memory_requirements);

  uint32_t memory_type_index = vk_find_memory_type_index(memory_requirements.memoryTypeBits, image->config->memory_property_flags);

  VkMemoryAllocateFlagsInfo memory_allocate_flags_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
    .flags = image->config->memory_allocate_flags,
  };

  VkMemoryAllocateInfo memory_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .pNext = &memory_allocate_flags_info,
    .allocationSize = memory_requirements.size,
    .memoryTypeIndex = memory_type_index,
  };

  TI_VK_CHECK(vkAllocateMemory(g_vk_instance.device, &memory_allocate_info, 0, &image->device_memory));
  TI_VK_CHECK(vkBindImageMemory(g_vk_instance.device, image->handle, image->device_memory, 0));

  VkImageViewCreateInfo image_view_create_info = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .image = image->handle,
    .viewType = image->config->image_view_type,
    .format = image->config->format,
    .subresourceRange = {
      .aspectMask = image->config->image_aspect_flags,
      .baseMipLevel = 0,
      .levelCount = image->config->mip_levels,
      .baseArrayLayer = 0,
      .layerCount = 1,
    },
  };

  TI_VK_CHECK(vkCreateImageView(g_vk_instance.device, &image_view_create_info, 0, &image->image_view));

  VkSamplerCreateInfo sampler_create_info = {
    .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
    .magFilter = VK_FILTER_NEAREST,
    .minFilter = VK_FILTER_NEAREST,
    .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    .anisotropyEnable = 0,
    .maxAnisotropy = 0.0F,
    .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
    .unnormalizedCoordinates = 0,
    .compareEnable = 0,
    .compareOp = VK_COMPARE_OP_ALWAYS,
    .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
    .mipLodBias = 0.0F,
    .minLod = 0.0F,
    .maxLod = VK_LOD_CLAMP_NONE,
  };

  TI_VK_CHECK(vkCreateSampler(g_vk_instance.device, &sampler_create_info, 0, &image->sampler));

  VkCommandBuffer command_buffer = vk_primary_command_buffer_record_immediate();

  VkImageMemoryBarrier image_memory_barrier = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .srcAccessMask = VK_ACCESS_NONE,
    .dstAccessMask = VK_ACCESS_NONE,
    .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    .newLayout = image->config->image_layout,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .image = image->handle,
    .subresourceRange = {
      .aspectMask = image->config->image_aspect_flags,
      .baseMipLevel = 0,
      .levelCount = image->config->mip_levels,
      .baseArrayLayer = 0,
      .layerCount = 1,
    },
  };

  vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);

  vk_primary_command_buffer_submit_immediate(command_buffer);
}
void vk_image_destroy(vk_image_t *image) {
  vkDestroySampler(g_vk_instance.device, image->sampler, 0);
  vkDestroyImageView(g_vk_instance.device, image->image_view, 0);

  vkFreeMemory(g_vk_instance.device, image->device_memory, 0);

  vkDestroyImage(g_vk_instance.device, image->handle, 0);
}
