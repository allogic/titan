#ifndef TI_IM_H
#define TI_IM_H

#include <imgui/ti_im_symbol.h>
#include <imgui/ti_im_fs.h>
#include <imgui/ti_im_hierarchy.h>
#include <imgui/ti_im_viewport.h>
#include <imgui/ti_im_inspector.h>
#include <imgui/ti_im_handle.h>
#include <imgui/ti_im_titlebar.h>
#include <imgui/ti_im_sidebar.h>
#include <imgui/ti_im_renderer.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern uint8_t g_im_show_left_panel;
extern uint8_t g_im_show_right_panel;

extern void *g_im_font_default_16;

extern void *g_im_font_symbols_16;
extern void *g_im_font_symbols_18;
extern void *g_im_font_symbols_22;
extern void *g_im_font_symbols_32;

void im_create(void);
void im_draw(void);
void im_message(HWND window_handle, UINT window_message, WPARAM w_param, LPARAM l_param);
void im_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_IM_H
