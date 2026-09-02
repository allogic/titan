#ifndef TI_FS_H
#define TI_FS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern fs *g_fs;

fs_result fs_setup(void);
fs_result fs_mkdir_recursive(fs *fs, const char *file_path, int32_t options);
fs_result fs_remove_recursive(fs *fS, char const *file_path);
void fs_close(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_H
