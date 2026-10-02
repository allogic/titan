#ifndef TI_IM_TEXT_EDITOR_H
#define TI_IM_TEXT_EDITOR_H

#include <ti_editor.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void im_text_editor_setup(void);
void im_text_editor_open(char const *asset_path);
void im_text_editor_close(void);
void im_text_editor_draw(void);
void im_text_editor_refresh(void);
void im_text_editor_reset(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_IM_TEXT_EDITOR_H
