#ifndef TI_IM_INSPECTOR_H
#define TI_IM_INSPECTOR_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void im_inspector_draw(void);
void im_inspector_select(im_inspector_type_t type, void *selection);
void im_inspector_reset(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_IM_INSPECTOR_H
