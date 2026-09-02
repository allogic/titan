#ifndef TI_VK_BUFFER_H
#define TI_VK_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void buffer_create(buffer_t *buffer);
void buffer_map(buffer_t *buffer);
void buffer_unmap(buffer_t *buffer);
void buffer_destroy(buffer_t *buffer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_BUFFER_H
