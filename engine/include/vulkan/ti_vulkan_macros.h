#ifndef TI_VULKAN_MACROS_H
#define TI_VULKAN_MACROS_H

#define TI_MAKE_DISPATCH_GROUP_COUNT(GLOBAL_SIZE, LOCAL_SIZE) ((int32_t)ceilf(((float)(GLOBAL_SIZE)) / (LOCAL_SIZE)))

#ifdef BUILD_DEBUG
#  define TI_VK_CHECK(EXPRESSION)                           \
    {                                                       \
      VkResult result = (EXPRESSION);                       \
      if (result != VK_SUCCESS) {                           \
        printf("%s failed with %d\n", #EXPRESSION, result); \
        __debugbreak();                                     \
      }                                                     \
    }
#else
#  define TI_VK_CHECK(EXPRESSION) (EXPRESSION)
#endif // BUILD_DEBUG

#endif // TI_VULKAN_MACROS_H
