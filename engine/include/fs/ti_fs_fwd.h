#ifndef TI_FS_FWD_H
#define TI_FS_FWD_H

typedef enum fs_asset_type_t {
  FS_ASSET_TYPE_NONE = 0,
  FS_ASSET_TYPE_MODEL,
  FS_ASSET_TYPE_PIPELINE,
  FS_ASSET_TYPE_FONT,               // OK
  FS_ASSET_TYPE_INPUT_VARIABLE,     // OK
  FS_ASSET_TYPE_DESCRIPTOR_BINDING, // OK
  FS_ASSET_TYPE_BUFFER,             // OK
  FS_ASSET_TYPE_IMAGE,              // OK
  FS_ASSET_TYPE_FRAMEBUFFER,        // OK
  FS_ASSET_TYPE_SWAPCHAIN,          // OK
  FS_ASSET_TYPE_RENDERPASS,         // OK
  FS_ASSET_TYPE_RENDERER,           // OK
  FS_ASSET_TYPE_COUNT,
} fs_asset_type_t;

typedef enum fs_pipeline_type_t {
  FS_PIPELINE_TYPE_DEFAULT = 0,
  FS_PIPELINE_TYPE_MESH,
  FS_PIPELINE_TYPE_RAY_TRACING,
  FS_PIPELINE_TYPE_COMPUTE,
} fs_pipeline_type_t;

typedef struct fs_asset_reference_t {
  char reference_path[TI_PATH_SIZE];
} fs_asset_reference_t;

typedef struct fs_primitive_t {
  char name[TI_PATH_SIZE];
  uint64_t position_count;
  uint64_t normal_count;
  uint64_t tangent_count;
  uint64_t texcoord_count;
  uint64_t color_count;
  uint64_t joint_count;
  uint64_t weight_count;
  uint64_t position_stride;
  uint64_t normal_stride;
  uint64_t tangent_stride;
  uint64_t texcoord_stride;
  uint64_t color_stride;
  uint64_t joint_stride;
  uint64_t weight_stride;
  void *positions;
  void *normals;
  void *tangents;
  void *texcoords;
  void *colors;
  void *joints;
  void *weights;
} fs_primitive_t;
typedef struct fs_mesh_t {
  char name[TI_PATH_SIZE];
  uint64_t primitive_count;
  fs_primitive_t *primitives;
} fs_mesh_t;
typedef struct fs_joint_t {
  char name[TI_PATH_SIZE];
  uint64_t child_count;
  struct fs_joint_t *children;
} fs_joint_t;
typedef struct fs_skin_t {
  char name[TI_PATH_SIZE];
  fs_joint_t *root_joint;
} fs_skin_t;
typedef struct fs_model_t {
  char name[TI_PATH_SIZE];
  uint64_t mesh_count;
  fs_mesh_t *meshes;
  uint64_t skin_count;
  fs_skin_t *skins;
} fs_model_t;
typedef struct fs_buffer_t {
  uint8_t zero_data;
  uint64_t size;
  VkBufferUsageFlags buffer_usage_flags;
  VkMemoryPropertyFlags memory_property_flags;
  VkMemoryAllocateFlags memory_allocate_flags;
} fs_buffer_t;
typedef struct fs_image_t {
  uint32_t width;
  uint32_t height;
  uint32_t depth;
  uint32_t mip_levels;
  uint64_t format_index;
  uint64_t image_layout_index;
  uint64_t image_type_index;
  uint64_t image_tiling_index;
  uint64_t image_view_type_index;
  VkImageUsageFlags image_usage_flags;
  VkImageAspectFlags image_aspect_flags;
  VkMemoryPropertyFlags memory_property_flags;
  VkMemoryAllocateFlags memory_allocate_flags;
} fs_image_t;
typedef struct fs_framebuffer_t {
  uint64_t color_attachment_count;
  fs_asset_reference_t *color_attachments;
  fs_asset_reference_t depth_attachment;
} fs_framebuffer_t;
typedef struct fs_block_variable_t {
  char name[TI_PATH_SIZE];
  uint32_t offset;
  uint32_t size;
} fs_block_variable_t;
typedef struct fs_descriptor_binding_t {
  char name[TI_PATH_SIZE];
  uint64_t descriptor_type_index;
  uint32_t set;
  uint32_t binding;
  int32_t block_size;
  int32_t block_variable_count;
  fs_block_variable_t *block_variables;
} fs_descriptor_binding_t;
typedef struct fs_input_variable_t {
  char name[TI_PATH_SIZE];
  uint32_t location;
  uint8_t is_built_in;
  uint64_t format_index;
} fs_input_variable_t;
typedef struct fs_pipeline_t {
  fs_pipeline_type_t pipeline_type;
  uint8_t enable_blending;
  uint8_t enable_depth_test;
  uint8_t enable_depth_write;
  uint32_t descriptor_set_count;
  uint64_t input_variable_count;
  uint64_t descriptor_binding_count;
  uint64_t primitive_topology_index;
  uint64_t polygon_mode_index;
  uint64_t spirv_vertex_word_count;
  uint64_t spirv_fragment_word_count;
  uint64_t spirv_task_word_count;
  uint64_t spirv_mesh_word_count;
  uint64_t spirv_ray_gen_word_count;
  uint64_t spirv_ray_miss_word_count;
  uint64_t spirv_ray_intersect_word_count;
  uint64_t spirv_ray_closest_hit_word_count;
  uint64_t spirv_compute_word_count;
  uint64_t glsl_vertex_shader_size;
  uint64_t glsl_fragment_shader_size;
  uint64_t glsl_task_shader_size;
  uint64_t glsl_mesh_shader_size;
  uint64_t glsl_ray_gen_shader_size;
  uint64_t glsl_ray_miss_shader_size;
  uint64_t glsl_ray_intersect_shader_size;
  uint64_t glsl_ray_closest_hit_shader_size;
  uint64_t glsl_compute_shader_size;
  void *spirv_vertex_words;
  void *spirv_fragment_words;
  void *spirv_task_words;
  void *spirv_mesh_words;
  void *spirv_ray_gen_words;
  void *spirv_ray_miss_words;
  void *spirv_ray_intersect_words;
  void *spirv_ray_closest_hit_words;
  void *spirv_compute_words;
  void *glsl_vertex_shader;
  void *glsl_fragment_shader;
  void *glsl_task_shader;
  void *glsl_mesh_shader;
  void *glsl_ray_gen_shader;
  void *glsl_ray_miss_shader;
  void *glsl_ray_intersect_shader;
  void *glsl_ray_closest_hit_shader;
  void *glsl_compute_shader;
  VkCullModeFlags cull_mode_flags;
  fs_asset_reference_t *input_variables;
  fs_asset_reference_t *descriptor_bindings;
} fs_pipeline_t;
typedef struct fs_font_t {
  void *buffer;
  uint64_t buffer_size;
} fs_font_t;
typedef struct fs_swapchain_t {
  uint32_t image_count;
} fs_swapchain_t;
typedef struct fs_renderpass_t {
  uint64_t initial_color_attachment_layout_index;
  uint64_t initial_depth_attachment_layout_index;
  uint64_t final_color_attachment_layout_index;
  uint64_t final_depth_attachment_layout_index;
} fs_renderpass_t;
typedef struct fs_renderer_t {
  fs_asset_reference_t debug_line_vertex_buffer;
  fs_asset_reference_t debug_line_index_buffer;
  fs_asset_reference_t full_screen_vertex_buffer;
  fs_asset_reference_t full_screen_index_buffer;
} fs_renderer_t;

typedef struct fs_asset_t {
  uint64_t magic;
  fs_asset_type_t type;
  char const *path;
  void *instance;
} fs_asset_t;

#endif // TI_FS_FWD_H
