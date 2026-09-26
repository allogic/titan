#ifndef TI_IM_TEXT_EDITOR_H
#define TI_IM_TEXT_EDITOR_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void im_test_editor_setup(char const *source_code);
void im_text_editor_draw(void);
void im_text_editor_refresh(void);
void im_text_editor_reset(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_IM_TEXT_EDITOR_H
