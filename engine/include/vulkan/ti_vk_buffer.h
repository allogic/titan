#ifndef TI_VK_BUFFER_H
#define TI_VK_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void vk_buffer_create(vk_buffer_t *buffer, char const *asset_path);
void vk_buffer_map(vk_buffer_t *buffer);
void vk_buffer_unmap(vk_buffer_t *buffer);
void vk_buffer_destroy(vk_buffer_t *buffer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_BUFFER_H
