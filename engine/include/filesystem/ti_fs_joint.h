#ifndef TI_FS_JOINT_H
#define TI_FS_JOINT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fs_joint_load(fs_joint_t *joint, fs_file *file);
void fs_joint_store(fs_joint_t *joint, fs_file *file);
void fs_joint_destroy(fs_joint_t *joint);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_JOINT_H
