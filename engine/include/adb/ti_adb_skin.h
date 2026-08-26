#ifndef TI_ADB_SKIN_H
#define TI_ADB_SKIN_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void adb_skin_create(adb_skin_t *skin);
void adb_skin_load(adb_skin_t *skin, FILE *file);
void adb_skin_store(adb_skin_t *skin, FILE *file);
void adb_skin_destroy(adb_skin_t *skin);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_ADB_SKIN_H
