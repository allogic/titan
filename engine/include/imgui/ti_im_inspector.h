#ifndef TI_IM_INSPECTOR_H
#define TI_IM_INSPECTOR_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void inspector_draw(void);
void inspector_select(inspector_type_t type, void *selection);
void inspector_reset(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_IM_INSPECTOR_H
