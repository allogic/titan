#ifndef TI_ENGINE_H
#define TI_ENGINE_H

/////////////////////////////////
// BEGIN DEPENDENCIES
/////////////////////////////////

#include <limits.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <float.h>
#include <time.h>
#include <math.h>

#if defined(OS_WINDOWS)
#  define WIN32_LEAN_AND_MEAN
#  define NOMINMAX
#  define VK_USE_PLATFORM_WIN32_KHR
#  include <windows.h>
#  include <dbghelp.h>
#elif defined(OS_LINUX)
#  define VK_USE_PLATFORM_WAYLAND_KHR
#elif defined(OS_DARWIN)
#  define VK_USE_PLATFORM_METAL_EXT
#endif // OS_SELECTION

#include <vulkan/vulkan.h>

#define FLECS_CUSTOM_BUILD
#define FLECS_META
#define FLECS_JSON
#define FLECS_SYSTEM
#define FLECS_PIPELINE
#include <flecs.h>

#include <fs.h>

#include <joltc.h> // TODO remove me

/////////////////////////////////
// BEGIN ENGINE
/////////////////////////////////

#include <ti_macros.h>
#include <vulkan/ti_vk_macros.h>

#include <ti_const.h>
#include <math/ti_ma_const.h>
#include <physic/ti_ph_const.h>
#include <component/ti_cp_const.h>
#include <vulkan/ti_vk_const.h>
#include <filesystem/ti_fs_const.h>

#include <ti_fwd.h>
#include <math/ti_ma_fwd.h>
#include <physic/ti_ph_fwd.h>
#include <component/ti_cp_fwd.h>
#include <filesystem/ti_fs_fwd.h>
#include <vulkan/ti_vk_fwd.h>

// TODO: remember to swap dmalloc for release builds..

#ifdef BUILD_DEBUG
#  define TI_ALLOC(SIZE, ZERO, REF) dmalloc_alloc(__FILE__, __func__, __LINE__, SIZE, ZERO, REF)
#  define TI_FREE(DATA) dmalloc_free(__FILE__, __func__, __LINE__, DATA)
#else
#  define TI_ALLOC(SIZE, ZERO, REF) dmalloc_alloc(__FILE__, __func__, __LINE__, SIZE, ZERO, REF)
#  define TI_FREE(DATA) dmalloc_free(__FILE__, __func__, __LINE__, DATA)
#endif // BUILD_DEBUG

#if defined(OS_WINDOWS)
#  include <platform/windows/ti_window.h>
#elif defined(OS_LINUX)
#  include <platform/linux/ti_window.h>
#elif defined(OS_DARWIN)
#  include <platform/darwin/ti_window.h>
#endif // OS_SELECTION

#include <ti_dmalloc.h>
#include <ti_map.h>

#include <math/ti_ma_misc.h>
#include <math/ti_ma_fvec2.h>
#include <math/ti_ma_fvec3.h>
#include <math/ti_ma_fvec4.h>
#include <math/ti_ma_ivec2.h>
#include <math/ti_ma_ivec3.h>
#include <math/ti_ma_ivec4.h>
#include <math/ti_ma_fquat.h>
#include <math/ti_ma_fmat4x4.h>

#include <physic/ti_ph_plane.h>
#include <physic/ti_ph_ray.h>
#include <physic/ti_ph_world.h>
#include <physic/ti_ph_demo.h>

#include <component/ti_cp_transform.h>
#include <component/ti_cp_camera.h>
#include <component/ti_cp_editor_camera_controller.h>
#include <component/ti_cp_material.h>
#include <component/ti_cp_mesh.h>
#include <component/ti_cp_skeleton.h>
#include <component/ti_cp_script.h>
#include <component/ti_cp_velocity.h>

#include <vulkan/ti_vk_enum.h>
#include <vulkan/ti_vk_commandbuffer.h>
#include <vulkan/ti_vk_memory.h>
#include <vulkan/ti_vk_buffer.h>
#include <vulkan/ti_vk_image.h>
#include <vulkan/ti_vk_swapchain.h>
#include <vulkan/ti_vk_framebuffer.h>
#include <vulkan/ti_vk_renderpass.h>
#include <vulkan/ti_vk_model.h>
#include <vulkan/ti_vk_pipeline.h>
#include <vulkan/ti_vk_font.h>
#include <vulkan/ti_vk_descriptor_binding.h>
#include <vulkan/ti_vk_renderer.h>
#include <vulkan/ti_vk_context.h>

#include <filesystem/ti_fs_primitive.h>
#include <filesystem/ti_fs_mesh.h>
#include <filesystem/ti_fs_model.h>
#include <filesystem/ti_fs_joint.h>
#include <filesystem/ti_fs_skin.h>
#include <filesystem/ti_fs_pipeline.h>
#include <filesystem/ti_fs_font.h>
#include <filesystem/ti_fs_input_variable.h>
#include <filesystem/ti_fs_descriptor_binding.h>
#include <filesystem/ti_fs_buffer.h>
#include <filesystem/ti_fs_image.h>
#include <filesystem/ti_fs_framebuffer.h>
#include <filesystem/ti_fs_swapchain.h>
#include <filesystem/ti_fs_renderpass.h>
#include <filesystem/ti_fs_renderer.h>
#include <filesystem/ti_fs_script.h>
#include <filesystem/ti_fs_sound.h>
#include <filesystem/ti_fs_asset.h>
#include <filesystem/ti_fs_import.h>
#include <filesystem/ti_fs_context.h>

#include <ti_archive.h>
#include <ti_clang.h>
#include <ti_scene.h>
#include <ti_audio.h>
#include <ti_audio_demo.h>

#endif // TI_ENGINE_H
