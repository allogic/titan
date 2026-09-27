#ifndef TI_VK_ENUM_H
#define TI_VK_ENUM_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern vk_enum_record_t g_vk_format_table[355];
extern vk_enum_record_t g_vk_image_layout_table[42];
extern vk_enum_record_t g_vk_image_type_table[3];
extern vk_enum_record_t g_vk_image_tiling_table[3];
extern vk_enum_record_t g_vk_image_view_type_table[7];
extern vk_enum_record_t g_vk_descriptor_type_table[19];
extern vk_enum_record_t g_vk_primitive_topology_table[11];
extern vk_enum_record_t g_vk_polygon_mode_table[4];

uint64_t vk_find_format_index(VkFormat format);
uint64_t vk_find_image_layout_index(VkImageLayout image_layout);
uint64_t vk_find_image_type_index(VkImageType image_type);
uint64_t vk_find_image_tiling_index(VkImageTiling image_tiling);
uint64_t vk_find_image_view_type_index(VkImageViewType image_view_type);
uint64_t vk_find_descriptor_type_index(VkDescriptorType descriptor_type);
uint64_t vk_find_primitive_topology_index(VkPrimitiveTopology primitive_topology);
uint64_t vk_find_polygon_mode_index(VkPolygonMode polygon_mode);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_ENUM_H
