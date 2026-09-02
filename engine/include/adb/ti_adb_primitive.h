#ifndef TI_ADB_PRIMITIVE_H
#define TI_ADB_PRIMITIVE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void adb_primitive_load(adb_primitive_t *primitive, fs_file *file);
void adb_primitive_store(adb_primitive_t *primitive, fs_file *file);
void adb_primitive_destroy(adb_primitive_t *primitive);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_ADB_PRIMITIVE_H
