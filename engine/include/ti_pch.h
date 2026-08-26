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
#include <vulkan/ti_vulkan_macros.h>

#include <ti_const.h>
#include <alloc/ti_alloc_const.h>
#include <math/ti_math_const.h>
#include <physic/ti_physic_const.h>
#include <tree/ti_tree_const.h>
#include <comp/ti_comp_const.h>
#include <vulkan/ti_vulkan_const.h>
#include <renderer/ti_renderer_const.h>
#include <adb/ti_adb_const.h>

#include <ti_fwd.h>
#include <alloc/ti_alloc_fwd.h>
#include <math/ti_math_fwd.h>
#include <physic/ti_physic_fwd.h>
#include <tree/ti_tree_fwd.h>
#include <comp/ti_comp_fwd.h>
#include <vulkan/ti_vulkan_fwd.h>
#include <renderer/ti_renderer_fwd.h>
#include <adb/ti_adb_fwd.h>
#include <imgui/ti_imgui_fwd.h>

#include <alloc/ti_alloc.h>
#include <math/ti_math.h>
#include <physic/ti_physic.h>
#include <tree/ti_tree.h>
#include <comp/ti_comp.h>
#include <vulkan/ti_vulkan.h>
#include <renderer/ti_renderer.h>
#include <adb/ti_adb.h>
#include <imgui/ti_imgui.h>

#include <ti_engine.h>
#include <ti_fsutil.h> // TODO: obsolete..
#include <ti_scene.h>

#endif // TI_PCH_H
