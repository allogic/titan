#ifndef TI_VK_DESCRIPTOR_BINDING_H
#define TI_VK_DESCRIPTOR_BINDING_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void vk_descriptor_binding_create(vk_descriptor_binding_t *descriptor_binding, char const *asset_path);
void vk_descriptor_binding_destroy(vk_descriptor_binding_t *descriptor_binding);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_DESCRIPTOR_BINDING_H
