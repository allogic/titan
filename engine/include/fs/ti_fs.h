#ifndef TI_FS_H
#define TI_FS_H

#include <fs/ti_fs_primitive.h>
#include <fs/ti_fs_mesh.h>
#include <fs/ti_fs_model.h>
#include <fs/ti_fs_joint.h>
#include <fs/ti_fs_skin.h>
#include <fs/ti_fs_pipeline.h>
#include <fs/ti_fs_font.h>
#include <fs/ti_fs_descriptor_binding.h>
#include <fs/ti_fs_buffer.h>
#include <fs/ti_fs_image.h>
#include <fs/ti_fs_framebuffer.h>
#include <fs/ti_fs_swapchain.h>
#include <fs/ti_fs_renderpass.h>
#include <fs/ti_fs_renderer.h>
#include <fs/ti_fs_asset.h>
#include <fs/ti_fs_import.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern fs *g_fs;

fs_result fs_create(char const *static_path, char const *asset_path);
fs_result fs_mkdir_recursive(fs *fs, const char *file_path, int32_t options);
fs_result fs_remove_recursive(fs *fs, char const *file_path);
void fs_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_H
