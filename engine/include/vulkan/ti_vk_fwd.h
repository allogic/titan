#ifndef TI_VK_FWD_H
#define TI_VK_FWD_H

// TODO: assetify this as well..
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
  VkCommandPool command_pool;
  VkCommandBuffer command_buffer;
} vk_instance_t;

typedef struct vk_swapchain_t {
  fs_asset_t asset;
  uint32_t is_dirty;
  uint32_t image_count;
  VkImage image[TI_SWAPCHAIN_MAX_IMAGE_COUNT];
  VkSwapchainKHR handle;
} vk_swapchain_t;
typedef struct vk_buffer_t {
  fs_asset_t asset;
  void *host_data;
  void *device_data;
  VkBuffer buffer_handle;
  VkDeviceMemory device_memory;
} vk_buffer_t;
typedef struct vk_model_t {
  fs_asset_t asset;
  vk_buffer_t vertex_buffer;
  vk_buffer_t index_buffer;
} vk_model_t;
typedef struct vk_pipeline_t {
  fs_asset_t asset;
  char const *vertex_shader;
  char const *task_shader;
  char const *mesh_shader;
  char const *ray_gen_shader;
  char const *ray_miss_shader;
  char const *ray_closest_hit_shader;
  char const *ray_intersect_shader;
  char const *fragment_shader;
  char const *compute_shader;
  uint32_t vertex_input_binding_description_count;
  uint32_t vertex_input_attribute_description_count;
  uint32_t push_constant_range_count;
  uint32_t descriptor_pool_size_count;
  uint32_t descriptor_set_layout_binding_count;
  VkVertexInputBindingDescription *vertex_input_binding_description;
  VkVertexInputAttributeDescription *vertex_input_attribute_description;
  VkPushConstantRange *push_constant_range;
  VkDescriptorPoolSize *descriptor_pool_size;
  VkDescriptorSetLayoutBinding *descriptor_set_layout_binding;
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
  fs_asset_t asset;
} vk_font_t;
typedef struct vk_descriptor_binding_t {
  fs_asset_t asset;
} vk_descriptor_binding_t;
typedef struct vk_image_t {
  fs_asset_t asset;
  VkImageView image_view;
  VkDeviceMemory device_memory;
  VkSampler sampler;
  VkImage handle;
} vk_image_t;
typedef struct vk_framebuffer_t {
  fs_asset_t asset;
  uint8_t is_dirty;
  uint32_t width;
  uint32_t height;
  vk_image_t color_attachment[TI_SWAPCHAIN_MAX_IMAGE_COUNT];
  vk_image_t depth_attachment[TI_SWAPCHAIN_MAX_IMAGE_COUNT];
  VkFramebuffer handle[TI_SWAPCHAIN_MAX_IMAGE_COUNT];
} vk_framebuffer_t;
typedef struct vk_renderpass_t {
  fs_asset_t asset;
  VkRenderPass handle;
} vk_renderpass_t;

typedef struct vk_time_info_t {
  float time;
  float delta_time;
} vk_time_info_t;
typedef struct vk_screen_info_t {
  ivec2_t resolution;
} vk_screen_info_t;
typedef struct vk_mouse_info_t {
  ivec2_t position;
} vk_mouse_info_t;
typedef struct vk_camera_info_t {
  fvec4_t position;
  fvec4_t direction;
  fmat4x4_t view;
  fmat4x4_t view_inv;
  fmat4x4_t projection;
  fmat4x4_t projection_inv;
  fmat4x4_t view_projection;
  fmat4x4_t view_projection_inv;
  fvec4_t frustum_plane[FRUSTUM_PLANE_COUNT];
} vk_camera_info_t;

TI_STATIC_ASSERT(TI_ALIGN_OF(vk_time_info_t) == 4);
TI_STATIC_ASSERT(TI_ALIGN_OF(vk_screen_info_t) == 4);
TI_STATIC_ASSERT(TI_ALIGN_OF(vk_mouse_info_t) == 4);
TI_STATIC_ASSERT(TI_ALIGN_OF(vk_camera_info_t) == 4);

typedef struct vk_full_screen_vertex_t {
  fvec4_t position;
} vk_full_screen_vertex_t;
typedef struct vk_debug_line_vertex_t {
  fvec4_t position;
  fvec4_t color;
} vk_debug_line_vertex_t;

TI_STATIC_ASSERT(TI_ALIGN_OF(vk_full_screen_vertex_t) == 4);
TI_STATIC_ASSERT(TI_ALIGN_OF(vk_debug_line_vertex_t) == 4);

typedef uint32_t vk_full_screen_index_t;
typedef uint32_t vk_debug_line_index_t;

typedef struct vk_renderer_t {
  fs_asset_t asset;
  uint32_t is_debug_enabled;
  uint32_t image_index;
  uint32_t debug_line_vertex_offset;
  uint32_t debug_line_index_offset;
  VkDescriptorBufferInfo time_info_descriptor_buffer_info;
  VkDescriptorBufferInfo screen_info_descriptor_buffer_info;
  VkDescriptorBufferInfo mouse_info_descriptor_buffer_info;
  VkDescriptorBufferInfo camera_info_descriptor_buffer_info;
  VkFence frame_fence;
  VkSemaphore render_finished_semaphore[TI_SWAPCHAIN_MAX_IMAGE_COUNT];
  VkSemaphore image_available_semaphore;
  vk_buffer_t debug_line_vertex_buffer;
  vk_buffer_t debug_line_index_buffer;
  vk_buffer_t full_screen_vertex_buffer;
  vk_buffer_t full_screen_index_buffer;
  vk_pipeline_t debug_line_pipeline;
} vk_renderer_t;

#endif // TI_VK_FWD_H
