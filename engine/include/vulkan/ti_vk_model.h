#ifndef TI_VK_MODEL_H
#define TI_VK_MODEL_H

#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void vk_model_create(vk_model_t *model, fs_model_t *config);
void vk_model_destroy(vk_model_t *model);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_MODEL_H
