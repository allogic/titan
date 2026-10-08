#include <vulkan/ti_vk_context.h>

asset_handle_t *g_vk_instance_hdl = 0;
asset_handle_t *g_vk_swapchain_hdl = 0;
asset_handle_t *g_vk_renderer_hdl = 0;
asset_handle_t *g_vk_viewport_hdl = 0; // TODO: not referenced yet..

asset_handle_t *g_vk_time_info_buffer_hdl = 0;
asset_handle_t *g_vk_screen_info_buffer_hdl = 0;
asset_handle_t *g_vk_mouse_info_buffer_hdl = 0;
asset_handle_t *g_vk_camera_info_buffer_hdl = 0;

void vk_context_create(void) {
  g_vk_instance_hdl = adb_handle(0, "asset/instance/main.json");

  vk_context_update();

  g_vk_time_info_buffer_hdl = adb_handle(g_vk_instance->hash, "asset/buffer/time_info.pak");
  g_vk_screen_info_buffer_hdl = adb_handle(g_vk_instance->hash, "asset/buffer/screen_info.pak");
  g_vk_mouse_info_buffer_hdl = adb_handle(g_vk_instance->hash, "asset/buffer/mouse_info.pak");
  g_vk_camera_info_buffer_hdl = adb_handle(g_vk_instance->hash, "asset/buffer/camera_info.pak");

  vk_buffer_map(g_vk_time_info_buffer);
  vk_buffer_map(g_vk_screen_info_buffer);
  vk_buffer_map(g_vk_mouse_info_buffer);
  vk_buffer_map(g_vk_camera_info_buffer);

  g_vk_swapchain_hdl = adb_handle(g_vk_instance->hash, "asset/swapchain/main.pak");
  g_vk_renderer_hdl = adb_handle(g_vk_instance->hash, "asset/renderer/main.pak");

  if (g_window.editor_create_proc) {
    g_window.editor_create_proc();
  }
}
void vk_context_update(void) {
  TI_VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(g_vk_instance->physical_device, g_vk_instance->surface, &g_vk_instance->surface_capabilities));

  g_window.width = g_vk_instance->surface_capabilities.currentExtent.width;
  g_window.height = g_vk_instance->surface_capabilities.currentExtent.height;

  g_vk_instance->min_image_count = g_vk_instance->surface_capabilities.minImageCount;
  g_vk_instance->max_image_count = g_vk_instance->surface_capabilities.maxImageCount;
  g_vk_instance->surface_transform = g_vk_instance->surface_capabilities.currentTransform;
}
void vk_context_destroy(void) {
  TI_VK_CHECK(vkQueueWaitIdle(g_vk_instance->primary_queue));
  TI_VK_CHECK(vkQueueWaitIdle(g_vk_instance->present_queue));

  if (g_window.editor_destroy_proc) {
    g_window.editor_destroy_proc();
  }
}
