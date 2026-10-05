#ifndef TI_VK_IMAGE_H
#define TI_VK_IMAGE_H

#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void vk_image_create(vk_image_t *image, fs_image_t *config);
void vk_image_destroy(vk_image_t *image);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_IMAGE_H
