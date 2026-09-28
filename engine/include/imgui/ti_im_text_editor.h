#ifndef TI_IM_TEXT_EDITOR_H
#define TI_IM_TEXT_EDITOR_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void im_text_editor_setup(void);
void im_text_editor_open(fs_asset_t *asset);
void im_text_editor_close(void);
void im_text_editor_draw(void);
void im_text_editor_refresh(void);
void im_text_editor_reset(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_IM_TEXT_EDITOR_H
