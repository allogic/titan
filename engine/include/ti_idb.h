#ifndef TI_iDB_H
#define TI_iDB_H

#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void idb_create(void);
handle_t *idb_reference(handle_t *source_handle, char const *asset_path);
void idb_dereference(handle_t *handle);
void idb_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_IDB_H
