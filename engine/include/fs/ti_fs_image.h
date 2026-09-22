#ifndef TI_FS_IMAGE_H
#define TI_FS_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fs_image_load(fs_image_t *image, fs_file *file);
void fs_image_store(fs_image_t *image, fs_file *file);
void fs_image_destroy(fs_image_t *image);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_IMAGE_H
