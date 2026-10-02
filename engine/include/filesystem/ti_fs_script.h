#ifndef TI_FS_SCRIPT_H
#define TI_FS_SCRIPT_H

#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fs_script_load(fs_script_t *script, fs_file *file);
void fs_script_store(fs_script_t *script, fs_file *file);
void fs_script_destroy(fs_script_t *script);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_SCRIPT_H
