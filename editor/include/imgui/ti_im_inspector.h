#ifndef TI_IM_INSPECTOR_H
#define TI_IM_INSPECTOR_H

#include <ti_editor.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void im_inspector_draw(void);
void im_inspector_select(im_inspector_type_t type, void *data);
void im_inspector_reset(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_IM_INSPECTOR_H
