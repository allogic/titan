#ifndef TI_VK_FRAMEBUFFER_H
#define TI_VK_FRAMEBUFFER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern vk_framebuffer_t g_framebuffer;

void framebuffer_create(void);
void framebuffer_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_FRAMEBUFFER_H
