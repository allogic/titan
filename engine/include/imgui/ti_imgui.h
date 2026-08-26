#ifndef TI_IMGUI_H
#define TI_IMGUI_H

#include <imgui/ti_imgui_fs.h>
#include <imgui/ti_imgui_hierarchy.h>
#include <imgui/ti_imgui_scene.h>
#include <imgui/ti_imgui_layout.h>
#include <imgui/ti_imgui_inspector.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void imgui_create(void);
void imgui_draw(VkCommandBuffer command_buffer);
void imgui_message(HWND window_handle, UINT window_message, WPARAM w_param, LPARAM l_param);
void imgui_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_IMGUI_H
