#ifndef TI_ADB_JOINT_H
#define TI_ADB_JOINT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void adb_joint_load(adb_joint_t *joint, fs_file *file);
void adb_joint_store(adb_joint_t *joint, fs_file *file);
void adb_joint_destroy(adb_joint_t *joint);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_ADB_JOINT_H
