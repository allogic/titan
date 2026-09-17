#ifndef TI_VK_FWD_H
#define TI_VK_FWD_H

typedef enum vk_pipeline_type_t {
  VK_PIPELINE_TYPE_DFLT = 0,
  VK_PIPELINE_TYPE_MESH,
  VK_PIPELINE_TYPE_RAY,
  VK_PIPELINE_TYPE_COMP,
} vk_pipeline_type_t;

typedef struct vk_buffer_t {
  uint32_t zero_data;
  void *host_data;
  void *device_data;
  uint64_t size;
  VkBufferUsageFlags buffer_usage_flags;
  VkMemoryPropertyFlags memory_property_flags;
  VkMemoryAllocateFlags memory_allocate_flags;
  VkBuffer buffer_handle;
  VkDeviceMemory device_memory;
} vk_buffer_t;
typedef struct vk_swapchain_t {
  uint32_t is_dirty;
  uint32_t image_count;
  VkImage image[TI_SWAPCHAIN_MAX_IMAGE_COUNT];
  VkSwapchainKHR handle;
} vk_swapchain_t;
typedef struct vk_framebuffer_t {
  VkImage color_image[TI_SWAPCHAIN_MAX_IMAGE_COUNT];
  VkImage depth_image[TI_SWAPCHAIN_MAX_IMAGE_COUNT];
  VkImageView color_image_view[TI_SWAPCHAIN_MAX_IMAGE_COUNT];
  VkImageView depth_image_view[TI_SWAPCHAIN_MAX_IMAGE_COUNT];
  VkDeviceMemory color_device_memory[TI_SWAPCHAIN_MAX_IMAGE_COUNT];
  VkDeviceMemory depth_device_memory[TI_SWAPCHAIN_MAX_IMAGE_COUNT];
  VkDescriptorImageInfo color_descriptor_image_info[TI_SWAPCHAIN_MAX_IMAGE_COUNT];
  VkDescriptorImageInfo depth_descriptor_image_info[TI_SWAPCHAIN_MAX_IMAGE_COUNT];
  VkSampler color_sampler[TI_SWAPCHAIN_MAX_IMAGE_COUNT];
  VkSampler depth_sampler[TI_SWAPCHAIN_MAX_IMAGE_COUNT];
  VkFramebuffer handle[TI_SWAPCHAIN_MAX_IMAGE_COUNT];
} vk_framebuffer_t;
typedef struct vk_instance_t {
  uint32_t min_image_count;
  uint32_t max_image_count;
  uint32_t frame_index;
  uint32_t primary_queue_index;
  uint32_t present_queue_index;
  VkInstance instance;
#ifdef BUILD_DEBUG
  VkDebugUtilsMessengerEXT debug_utils_messenger;
#endif // BUILD_DEBUG
  VkSurfaceKHR surface;
  VkSurfaceCapabilitiesKHR surface_capabilities;
  VkSurfaceTransformFlagBitsKHR surface_transform;
  VkPhysicalDeviceFeatures2 physical_device_features2;
  VkPhysicalDeviceProperties2 physical_device_properties2;
  VkPhysicalDeviceMemoryProperties2 physical_device_memory_properties2;
  VkPhysicalDevice physical_device;
  VkDevice device;
  VkQueue primary_queue;
  VkQueue present_queue;
} vk_instance_t;

typedef struct vk_model_t {
  void *asset;
  vk_buffer_t vertex_buffer;
  vk_buffer_t index_buffer;
} vk_model_t;
typedef struct vk_pipeline_t {
  void *asset;
  vk_pipeline_type_t pipeline_type;
  char const *vertex_shader;
  char const *task_shader;
  char const *mesh_shader;
  char const *ray_gen_shader;
  char const *ray_miss_shader;
  char const *ray_closest_hit_shader;
  char const *ray_intersect_shader;
  char const *fragment_shader;
  char const *compute_shader;
  uint32_t enable_blending;
  uint32_t enable_depth_test;
  uint32_t enable_depth_write;
  uint32_t vertex_input_binding_description_count;
  uint32_t vertex_input_attribute_description_count;
  uint32_t push_constant_range_count;
  uint32_t descriptor_pool_size_count;
  uint32_t descriptor_set_layout_binding_count;
  uint32_t descriptor_set_count;
  VkPrimitiveTopology primitive_topology;
  VkPolygonMode polygon_mode;
  VkCullModeFlags cull_mode;
  VkVertexInputBindingDescription *vertex_input_binding_description;
  VkVertexInputAttributeDescription *vertex_input_attribute_description;
  VkPushConstantRange *push_constant_range;
  VkDescriptorPoolSize *descriptor_pool_size;
  VkDescriptorSetLayoutBinding *descriptor_set_layout_binding;
  VkRenderPass *render_pass;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSetLayout descriptor_set_layout_base;
  VkDescriptorSetLayout *descriptor_set_layout;
  VkDescriptorSet *descriptor_set;
  VkPipelineLayout pipeline_layout;
  VkPipeline pipeline_handle;
  VkBuffer sbt_buffer_handle;
  VkDeviceMemory sbt_device_memory;
  VkDeviceAddress sbt_device_address;
  uint32_t ray_gen_group_count;
  uint32_t ray_miss_group_count;
  uint32_t ray_hit_group_count;
  uint32_t callable_group_count;
  VkStridedDeviceAddressRegionKHR ray_gen_region;
  VkStridedDeviceAddressRegionKHR ray_miss_region;
  VkStridedDeviceAddressRegionKHR ray_hit_region;
  VkStridedDeviceAddressRegionKHR callable_region;
} vk_pipeline_t;
typedef struct vk_font_t {
  void *asset;
} vk_font_t;
typedef struct vk_descriptor_binding_t {
  void *asset;
} vk_descriptor_binding_t;

#endif // TI_VK_FWD_H
