#ifndef TI_ADB_H
#define TI_ADB_H

#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void adb_create(void);
asset_handle_t *adb_handle(uint64_t parent, char const *asset_path);
void adb_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_ADB_H
