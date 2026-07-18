#ifndef PCH_H
#define PCH_H

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <float.h>
#include <time.h>
#include <math.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include <macros.h>
#include <fwd.h>
#include <heap.h>

#include <math/constant.h>
#include <math/misc.h>
#include <math/vector2.h>
#include <math/vector3.h>
#include <math/vector4.h>
#include <math/ivector2.h>
#include <math/ivector3.h>
#include <math/ivector4.h>
#include <math/quaternion.h>
#include <math/matrix4x4.h>

#endif // PCH_H
