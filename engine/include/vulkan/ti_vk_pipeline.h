#ifndef TI_VK_PIPELINE_H
#define TI_VK_PIPELINE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// TODO: fix dependency injection of renderpass..
void vk_pipeline_create(vk_pipeline_t *pipeline, vk_renderpass_t *renderpass, char const *asset_path);
void vk_pipeline_destroy(vk_pipeline_t *pipeline);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_PIPELINE_H
