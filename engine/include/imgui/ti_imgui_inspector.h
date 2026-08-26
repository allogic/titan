#ifndef TI_IMGUI_INSPECTOR_H
#define TI_IMGUI_INSPECTOR_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern inspector_type_t g_inspector_type;

void inspector_draw(void);
void inspector_reset(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_IMGUI_INSPECTOR_H
