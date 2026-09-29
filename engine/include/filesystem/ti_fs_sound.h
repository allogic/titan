#ifndef TI_FS_SOUND_H
#define TI_FS_SOUND_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fs_sound_load(fs_sound_t *sound, fs_file *file);
void fs_sound_store(fs_sound_t *sound, fs_file *file);
void fs_sound_destroy(fs_sound_t *sound);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_SOUND_H
