#ifndef TI_VK_MEMORY_H
#define TI_VK_MEMORY_H

#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint32_t vk_memory_find_type_index(uint32_t type_filter, VkMemoryPropertyFlags memory_property_flags);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_MEMORY_H
