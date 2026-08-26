#ifndef TI_IMGUI_HIERARCHY_H
#define TI_IMGUI_HIERARCHY_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern ecs_entity_t g_selected_entity;

void hierarchy_draw(void);
void hierarchy_reset(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_IMGUI_HIERARCHY_H
