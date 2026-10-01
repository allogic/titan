#ifndef TI_PCH_H
#define TI_PCH_H

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

/////////////////////////////////
// BEGIN DEPENDENCIES
/////////////////////////////////

#if defined(OS_WINDOWS)
#  define WIN32_LEAN_AND_MEAN
#  define NOMINMAX
#  define VK_USE_PLATFORM_WIN32_KHR
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
#include <math/ti_math_const.h>
#include <physic/ti_physic_const.h>
#include <component/ti_cp_const.h>
#include <vulkan/ti_vk_const.h>
#include <filesystem/ti_fs_const.h>
#include <imgui/ti_im_const.h>

#include <ti_fwd.h>
#include <math/ti_math_fwd.h>
#include <physic/ti_physic_fwd.h>
#include <component/ti_cp_fwd.h>
#include <filesystem/ti_fs_fwd.h>
#include <vulkan/ti_vk_fwd.h>
#include <imgui/ti_im_fwd.h>
#include <clang/ti_cl_fwd.h>

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
#include <ti_archive.h>

#include <math/ti_math.h>
#include <physic/ti_physic.h>
#include <component/ti_cp.h>
#include <vulkan/ti_vk.h>
#include <filesystem/ti_fs.h>
#include <imgui/ti_im.h>
#include <clang/ti_cl.h>

#include <ti_scene.h>
#include <ti_audio.h>
#include <ti_audio_demo.h>

#endif // TI_PCH_H
