#ifndef TI_FS_CONTEXT_H
#define TI_FS_CONTEXT_H

#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern fs *g_fs_context;

fs_result fs_create(char const *static_path, char const *asset_path);
fs_result fs_mkdir_recursive(fs *fs, const char *file_path, int32_t options);
fs_result fs_remove_recursive(fs *fs, char const *file_path);
fs_result fs_path_parent(char const *file_path, char *parent_path);
void fs_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_CONTEXT_H
