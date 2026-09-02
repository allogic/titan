#include <ti_pch.h>

uint32_t vk_find_memory_type_index(uint32_t type_filter, VkMemoryPropertyFlags memory_property_flags) {
  uint32_t memory_type_index = 0;
  uint32_t memory_type_count = g_window.physical_device_memory_properties2.memoryProperties.memoryTypeCount;

  while (memory_type_index < memory_type_count) {

    if ((type_filter & (1 << memory_type_index)) && ((g_window.physical_device_memory_properties2.memoryProperties.memoryTypes[memory_type_index].propertyFlags & memory_property_flags) == memory_property_flags)) {
      return memory_type_index;
    }

    memory_type_index++;
  }

  return UINT32_MAX;
}

VkCommandBuffer vk_primary_command_buffer_record_immediate(void) {
  VkCommandBuffer command_buffer = 0;

  VkCommandBufferAllocateInfo command_buffer_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .commandPool = g_renderer.command_pool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = 1,
  };

  TI_VK_CHECK(vkAllocateCommandBuffers(g_window.device, &command_buffer_allocate_info, &command_buffer));

  VkCommandBufferBeginInfo command_buffer_begin_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  };

  TI_VK_CHECK(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));

  return command_buffer;
}
void vk_primary_command_buffer_submit_immediate(VkCommandBuffer command_buffer) {
  TI_VK_CHECK(vkEndCommandBuffer(command_buffer));

  VkSubmitInfo submit_info = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .commandBufferCount = 1,
    .pCommandBuffers = &command_buffer,
  };

  TI_VK_CHECK(vkQueueSubmit(g_window.primary_queue, 1, &submit_info, 0));
  TI_VK_CHECK(vkQueueWaitIdle(g_window.primary_queue));

  vkFreeCommandBuffers(g_window.device, g_renderer.command_pool, 1, &command_buffer);
}
