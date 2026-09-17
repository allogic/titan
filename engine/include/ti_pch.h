#ifndef TI_PCH_H
#define TI_PCH_H

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

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <dbghelp.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#define FLECS_CUSTOM_BUILD
#define FLECS_META
#define FLECS_JSON
#define FLECS_SYSTEM
#include <flecs.h>

#include <fs.h>

/////////////////////////////////
// BEGIN ENGINE
/////////////////////////////////

#include <ti_macros.h>
#include <vulkan/ti_vk_macros.h>

#include <ti_const.h>
#include <math/ti_math_const.h>
#include <physic/ti_physic_const.h>
#include <comp/ti_comp_const.h>
#include <vulkan/ti_vk_const.h>
#include <renderer/ti_renderer_const.h>
#include <fs/ti_fs_const.h>
#include <imgui/ti_im_const.h>

#include <ti_fwd.h>
#include <math/ti_math_fwd.h>
#include <physic/ti_physic_fwd.h>
#include <comp/ti_comp_fwd.h>
#include <platform/ti_pl_fwd.h>
#include <vulkan/ti_vk_fwd.h>
#include <fs/ti_fs_fwd.h>
#include <renderer/ti_renderer_fwd.h>
#include <imgui/ti_im_fwd.h>

#ifdef BUILD_DEBUG
#  define TI_ALLOC(SIZE, ZERO, REF) dmalloc_alloc(__FILE__, __func__, __LINE__, SIZE, ZERO, REF)
#  define TI_FREE(DATA) dmalloc_free(__FILE__, __func__, __LINE__, DATA)
#else
#  define TI_ALLOC(SIZE, ZERO, REF)
#  define TI_FREE(DATA)
#endif // BUILD_DEBUG

#include <ti_dmalloc.h>
#include <ti_map.h>

#include <math/ti_math.h>
#include <physic/ti_physic.h>
#include <comp/ti_comp.h>
#include <platform/ti_pl.h>
#include <vulkan/ti_vk.h>
#include <fs/ti_fs.h>
#include <renderer/ti_renderer.h>
#include <imgui/ti_im.h>

#include <ti_archive.h>
#include <ti_scene.h>
#include <ti_import.h>

#endif // TI_PCH_H
