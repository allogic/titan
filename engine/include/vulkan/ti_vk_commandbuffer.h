#ifndef TI_VK_COMMANDBUFFER_H
#define TI_VK_COMMANDBUFFER_H

#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

VkCommandBuffer vk_commandbuffer_primary_record_immediate(void);
void vk_commandbuffer_primary_submit_immediate(VkCommandBuffer command_buffer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_COMMANDBUFFER_H
