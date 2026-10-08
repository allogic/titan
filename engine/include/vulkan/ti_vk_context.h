#ifndef TI_VK_CONTEXT_H
#define TI_VK_CONTEXT_H

#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern asset_handle_t *g_vk_instance_hdl;
extern asset_handle_t *g_vk_swapchain_hdl;
extern asset_handle_t *g_vk_renderer_hdl;
extern asset_handle_t *g_vk_viewport_hdl;

extern asset_handle_t *g_vk_time_info_buffer_hdl;
extern asset_handle_t *g_vk_screen_info_buffer_hdl;
extern asset_handle_t *g_vk_mouse_info_buffer_hdl;
extern asset_handle_t *g_vk_camera_info_buffer_hdl;

#define g_vk_instance ((vk_instance_t *)g_vk_instance_hdl->instance)
#define g_vk_swapchain ((vk_swapchain_t *)g_vk_swapchain_hdl->instance)
#define g_vk_renderer ((vk_renderer_t *)g_vk_renderer_hdl->instance)
#define g_vk_viewport ((vk_viewport_t *)g_vk_viewport_hdl->instance)

// #define g_vk_main_renderpass ((vk_renderpass_t *)g_vk_main_renderpass_hdl->instance)
// #define g_vk_imgui_renderpass ((vk_renderpass_t *)g_vk_imgui_renderpass_hdl->instance)

// #define g_vk_main_framebuffer ((vk_framebuffer_t *)g_vk_main_framebuffer_hdl->instance)
// #define g_vk_imgui_framebuffer ((vk_framebuffer_t *)g_vk_imgui_framebuffer_hdl->instance)

#define g_vk_time_info_buffer ((vk_buffer_t *)g_vk_time_info_buffer_hdl->instance)
#define g_vk_screen_info_buffer ((vk_buffer_t *)g_vk_screen_info_buffer_hdl->instance)
#define g_vk_mouse_info_buffer ((vk_buffer_t *)g_vk_mouse_info_buffer_hdl->instance)
#define g_vk_camera_info_buffer ((vk_buffer_t *)g_vk_camera_info_buffer_hdl->instance)

void vk_context_create(void);
void vk_context_update(void);
void vk_context_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_CONTEXT_H
