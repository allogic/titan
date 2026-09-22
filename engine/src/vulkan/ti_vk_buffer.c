#include <ti_pch.h>

void vk_buffer_create(vk_buffer_t *buffer, char const *asset_path) {
  buffer->config = (fs_buffer_t *)fs_get(asset_path);

  VkBuffer staging_buffer = 0;
  VkDeviceMemory staging_device_memory = 0;

  {
    VkBufferCreateInfo buffer_create_info = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = buffer->config->size,
      .usage = buffer->config->buffer_usage_flags,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    TI_VK_CHECK(vkCreateBuffer(g_vk_instance.device, &buffer_create_info, 0, &buffer->buffer_handle));

    VkMemoryRequirements memory_requirements = {0};

    vkGetBufferMemoryRequirements(g_vk_instance.device, buffer->buffer_handle, &memory_requirements);

    uint32_t memory_type_index = vk_find_memory_type_index(memory_requirements.memoryTypeBits, buffer->config->memory_property_flags);

    VkMemoryAllocateFlagsInfo memory_allocate_flags_info = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
      .flags = buffer->config->memory_allocate_flags,
    };

    VkMemoryAllocateInfo memory_allocate_info = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .pNext = &memory_allocate_flags_info,
      .allocationSize = memory_requirements.size,
      .memoryTypeIndex = memory_type_index,
    };

    TI_VK_CHECK(vkAllocateMemory(g_vk_instance.device, &memory_allocate_info, 0, &buffer->device_memory));
    TI_VK_CHECK(vkBindBufferMemory(g_vk_instance.device, buffer->buffer_handle, buffer->device_memory, 0));
  }

  {
    VkBufferCreateInfo buffer_create_info = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = buffer->config->size,
      .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    TI_VK_CHECK(vkCreateBuffer(g_vk_instance.device, &buffer_create_info, 0, &staging_buffer));

    VkMemoryRequirements memory_requirements = {0};

    vkGetBufferMemoryRequirements(g_vk_instance.device, staging_buffer, &memory_requirements);

    uint32_t memory_type_index = vk_find_memory_type_index(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VkMemoryAllocateInfo memory_allocate_info = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = memory_requirements.size,
      .memoryTypeIndex = memory_type_index,
    };

    TI_VK_CHECK(vkAllocateMemory(g_vk_instance.device, &memory_allocate_info, 0, &staging_device_memory));
    TI_VK_CHECK(vkBindBufferMemory(g_vk_instance.device, staging_buffer, staging_device_memory, 0));
  }

  if (buffer->host_data) {

    void *staging_device_data = 0;

    TI_VK_CHECK(vkMapMemory(g_vk_instance.device, staging_device_memory, 0, buffer->config->size, 0, &staging_device_data));

    memcpy(staging_device_data, buffer->host_data, buffer->config->size);

    vkUnmapMemory(g_vk_instance.device, staging_device_memory);
  }

  VkCommandBuffer command_buffer = vk_primary_command_buffer_record_immediate();

  if (buffer->config->zero_data) {

    vkCmdFillBuffer(command_buffer, buffer->buffer_handle, 0, buffer->config->size, 0);
  }

  VkBufferCopy buffer_copy = {
    .srcOffset = 0,
    .dstOffset = 0,
    .size = buffer->config->size,
  };

  vkCmdCopyBuffer(command_buffer, staging_buffer, buffer->buffer_handle, 1, &buffer_copy);

  vk_primary_command_buffer_submit_immediate(command_buffer);

  vkFreeMemory(g_vk_instance.device, staging_device_memory, 0);
  vkDestroyBuffer(g_vk_instance.device, staging_buffer, 0);
}
void vk_buffer_map(vk_buffer_t *buffer) {
  TI_VK_CHECK(vkMapMemory(g_vk_instance.device, buffer->device_memory, 0, buffer->config->size, 0, &buffer->device_data));
}
void vk_buffer_unmap(vk_buffer_t *buffer) {
  vkUnmapMemory(g_vk_instance.device, buffer->device_memory);

  buffer->device_data = 0;
}
void vk_buffer_destroy(vk_buffer_t *buffer) {
  if (buffer->device_data) {

    vkUnmapMemory(g_vk_instance.device, buffer->device_memory);

    buffer->device_data = 0;
  }

  vkFreeMemory(g_vk_instance.device, buffer->device_memory, 0);
  vkDestroyBuffer(g_vk_instance.device, buffer->buffer_handle, 0);
}
