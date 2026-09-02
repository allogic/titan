#ifndef TI_ADB_FONT_H
#define TI_ADB_FONT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void adb_font_load(adb_font_t *font, fs_file *file);
void adb_font_store(adb_font_t *font, fs_file *file);
void adb_font_destroy(adb_font_t *font);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_ADB_FONT_H
