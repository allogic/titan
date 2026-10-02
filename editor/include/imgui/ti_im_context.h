#ifndef TI_IM_CONTEXT_H
#define TI_IM_CONTEXT_H

#include <ti_editor.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern uint8_t g_im_show_left_panel;
extern uint8_t g_im_show_right_panel;
extern uint8_t g_im_show_bottom_panel;

extern void *g_im_font_default_16;

extern void *g_im_font_symbols_16;
extern void *g_im_font_symbols_18;
extern void *g_im_font_symbols_22;
extern void *g_im_font_symbols_32;

void im_context_create(void);
void im_context_draw(void);
void im_context_message(HWND window_handle, UINT window_message, WPARAM w_param, LPARAM l_param);
void im_context_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_IM_CONTEXT_H
