#ifndef TI_FS_FONT_H
#define TI_FS_FONT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fs_font_load(fs_font_t *font, fs_file *file);
void fs_font_store(fs_font_t *font, fs_file *file);
void fs_font_destroy(fs_font_t *font);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_FONT_H
