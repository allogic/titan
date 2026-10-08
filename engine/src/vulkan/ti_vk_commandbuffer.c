#include <vulkan/ti_vk_commandbuffer.h>

VkCommandBuffer vk_commandbuffer_primary_record_immediate(void) {
  VkCommandBuffer command_buffer = 0;

  VkCommandBufferAllocateInfo command_buffer_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .commandPool = g_vk_instance->command_pool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = 1,
  };

  TI_VK_CHECK(vkAllocateCommandBuffers(g_vk_instance->device, &command_buffer_allocate_info, &command_buffer));

  VkCommandBufferBeginInfo command_buffer_begin_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  };

  TI_VK_CHECK(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));

  return command_buffer;
}
void vk_commandbuffer_primary_submit_immediate(VkCommandBuffer command_buffer) {
  TI_VK_CHECK(vkEndCommandBuffer(command_buffer));

  VkSubmitInfo submit_info = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .commandBufferCount = 1,
    .pCommandBuffers = &command_buffer,
  };

  TI_VK_CHECK(vkQueueSubmit(g_vk_instance->primary_queue, 1, &submit_info, 0));
  TI_VK_CHECK(vkQueueWaitIdle(g_vk_instance->primary_queue));

  vkFreeCommandBuffers(g_vk_instance->device, g_vk_instance->command_pool, 1, &command_buffer);
}
