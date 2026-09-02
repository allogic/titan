#ifndef TI_VK_H
#define TI_VK_H

#include <vulkan/ti_vk_buffer.h>
#include <vulkan/ti_vk_pipeline.h>
#include <vulkan/ti_vk_swapchain.h>
#include <vulkan/ti_vk_framebuffer.h>
#include <vulkan/ti_vk_renderpass.h>
#include <vulkan/ti_vk_window.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint32_t vk_find_memory_type_index(uint32_t type_filter, VkMemoryPropertyFlags memory_property_flags);

VkCommandBuffer vk_primary_command_buffer_record_immediate(void);
void vk_primary_command_buffer_submit_immediate(VkCommandBuffer command_buffer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_H
