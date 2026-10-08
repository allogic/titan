#ifndef TI_VK_FWD_H
#define TI_VK_FWD_H

typedef enum vk_pipeline_type_t {
  VK_PIPELINE_TYPE_NONE = 0,
  VK_PIPELINE_TYPE_DEFAULT,
  VK_PIPELINE_TYPE_MESH,
  VK_PIPELINE_TYPE_RAY_TRACING,
  VK_PIPELINE_TYPE_COMPUTE,
  VK_PIPELINE_TYPE_COUNT,
} vk_pipeline_type_t;

typedef struct vk_instance_t {
  uint64_t hash;
  cJSON *config;
  uint32_t min_image_count;
  uint32_t max_image_count;
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
#ifdef BUILD_DEBUG
  PFN_vkCreateDebugUtilsMessengerEXT create_debug_utils_messenger_ext_proc;
  PFN_vkDestroyDebugUtilsMessengerEXT destroy_debug_utils_messenger_ext_proc;
#endif // BUILD_DEBUG
  PFN_vkCmdDrawMeshTasksEXT cmd_draw_mesh_tasks_ext_proc;
  PFN_vkCmdTraceRaysKHR cmd_trace_rays_khr_proc;
  PFN_vkCmdBuildAccelerationStructuresKHR cmd_build_acceleration_structures_khr_proc;
  PFN_vkCreateAccelerationStructureKHR create_acceleration_structure_khr_proc;
  PFN_vkCreateRayTracingPipelinesKHR create_ray_tracing_pipelines_khr_proc;
  PFN_vkGetAccelerationStructureBuildSizesKHR get_acceleration_structure_build_sizes_khr_proc;
  PFN_vkGetAccelerationStructureDeviceAddressKHR get_acceleration_structure_device_address_khr_proc;
  PFN_vkGetRayTracingShaderGroupHandlesKHR get_ray_tracing_shader_group_handles_khr_proc;
  PFN_vkDestroyAccelerationStructureKHR destroy_acceleration_structure_khr_proc;
} vk_instance_t;
typedef struct vk_swapchain_t {
  uint64_t hash;
  cJSON *config;
  uint32_t is_dirty;
  uint32_t image_count;
  VkImage image[TI_SWAPCHAIN_MAX_IMAGE_COUNT];
  VkSwapchainKHR swapchain;
} vk_swapchain_t;
typedef struct vk_buffer_t {
  uint64_t hash;
  cJSON *config;
  void *host_data;
  void *device_data;
  VkBuffer buffer;
  VkDeviceMemory device_memory;
} vk_buffer_t;
typedef struct vk_model_t {
  uint64_t hash;
  cJSON *config;
  vk_buffer_t vertex_buffer;
  vk_buffer_t index_buffer;
} vk_model_t;
typedef struct vk_pipeline_t {
  uint64_t hash;
  cJSON *config;
  char const *vertex_shader;
  char const *task_shader;
  char const *mesh_shader;
  char const *ray_gen_shader;
  char const *ray_miss_shader;
  char const *ray_closest_hit_shader;
  char const *ray_intersect_shader;
  char const *fragment_shader;
  char const *compute_shader;
  uint32_t push_constant_range_count; // TODO
  uint32_t vertex_input_binding_description_count;
  uint32_t vertex_input_attribute_description_count;
  uint32_t descriptor_pool_size_count;
  uint32_t descriptor_set_layout_binding_count;
  VkPushConstantRange *push_constant_range; // TODO
  VkVertexInputBindingDescription *vertex_input_binding_description;
  VkVertexInputAttributeDescription *vertex_input_attribute_description;
  VkDescriptorPoolSize *descriptor_pool_size;
  VkDescriptorSetLayoutBinding *descriptor_set_layout_binding;
  VkDescriptorSetLayout *descriptor_set_layout;
  VkDescriptorSet *descriptor_set;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSetLayout descriptor_set_layout_base;
  VkPipelineLayout pipeline_layout;
  VkPipeline pipeline;
  VkBuffer sbt_buffer;
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
  asset_handle_t *renderpass_hdl;
} vk_pipeline_t;
typedef struct vk_font_t {
  uint64_t hash;
  cJSON *config;
} vk_font_t;
typedef struct vk_descriptor_binding_t {
  uint64_t hash;
  cJSON *config;
} vk_descriptor_binding_t;
typedef struct vk_image_t {
  uint64_t hash;
  cJSON *config;
  VkImageView image_view;
  VkDeviceMemory device_memory;
  VkSampler sampler;
  VkImage image;
} vk_image_t;
typedef struct vk_framebuffer_t {
  uint64_t hash;
  cJSON *config;
  uint8_t is_dirty;
  VkFramebuffer framebuffer;
  asset_handle_t **color_attachment_hdl;
  asset_handle_t *depth_attachment_hdl;
} vk_framebuffer_t;
typedef struct vk_renderpass_t {
  uint64_t hash;
  cJSON *config;
  VkRenderPass renderpass;
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
  fvec4_t frustum_plane[CP_FRUSTUM_PLANE_COUNT];
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
  uint64_t hash;
  cJSON *config;
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
  asset_handle_t *main_renderpass_hdl;
  asset_handle_t *imgui_renderpass_hdl;
  asset_handle_t *main_framebuffer_hdl;
  asset_handle_t *imgui_framebuffer_hdl;
  asset_handle_t *debug_line_vertex_buffer_hdl;
  asset_handle_t *debug_line_index_buffer_hdl;
  asset_handle_t *full_screen_vertex_buffer_hdl;
  asset_handle_t *full_screen_index_buffer_hdl;
  asset_handle_t *debug_line_pipeline_hdl;
} vk_renderer_t;

// TODO: move this into the editor..
typedef struct vk_viewport_t {
  uint32_t width;
  uint32_t height;
  uint32_t mouse_position_x;
  uint32_t mouse_position_y;
  VkDescriptorSet color_attachment;
  VkDescriptorSet depth_attachment;
} vk_viewport_t;

#endif // TI_VK_FWD_H
