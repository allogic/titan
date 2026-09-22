#ifndef TI_VK_IMAGE_H
#define TI_VK_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void vk_image_create(vk_image_t *image, uint32_t width, uint32_t height, uint32_t depth, char const *asset_path);
void vk_image_destroy(vk_image_t *image);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_IMAGE_H
