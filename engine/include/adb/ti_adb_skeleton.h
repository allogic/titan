#ifndef TI_ADB_SKELETON_H
#define TI_ADB_SKELETON_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void adb_skeleton_create(adb_skeleton_t *skeleton);
void adb_skeleton_load(adb_skeleton_t *skeleton, FILE *file);
void adb_skeleton_store(adb_skeleton_t *skeleton, FILE *file);
void adb_skeleton_destroy(adb_skeleton_t *skeleton);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_ADB_SKELETON_H
