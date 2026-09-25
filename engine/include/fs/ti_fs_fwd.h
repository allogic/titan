#ifndef TI_FS_FWD_H
#define TI_FS_FWD_H

typedef enum fs_asset_type_t {
  FS_ASSET_TYPE_NONE = 0,
  FS_ASSET_TYPE_MODEL,
  FS_ASSET_TYPE_PIPELINE,
  FS_ASSET_TYPE_FONT,
  FS_ASSET_TYPE_DESCRIPTOR_BINDING,
  FS_ASSET_TYPE_BUFFER,
  FS_ASSET_TYPE_IMAGE,
  FS_ASSET_TYPE_FRAMEBUFFER,
  FS_ASSET_TYPE_SWAPCHAIN,
  FS_ASSET_TYPE_RENDERPASS,
  FS_ASSET_TYPE_RENDERER,
  FS_ASSET_TYPE_COUNT,
} fs_asset_type_t;

typedef enum fs_pipeline_type_t {
  FS_PIPELINE_TYPE_DFLT = 0,
  FS_PIPELINE_TYPE_MESH,
  FS_PIPELINE_TYPE_RAY,
  FS_PIPELINE_TYPE_COMP,
} fs_pipeline_type_t;

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
  char name[TI_PATH_SIZE];
  uint32_t width;
  uint32_t height;
  uint32_t depth;
  uint32_t mip_levels;
  VkFormat format;
  VkImageLayout image_layout;
  VkImageType image_type;
  VkImageTiling image_tiling;
  VkImageViewType image_view_type;
  VkImageUsageFlags image_usage_flags;
  VkImageAspectFlags image_aspect_flags;
  VkMemoryPropertyFlags memory_property_flags;
  VkMemoryAllocateFlags memory_allocate_flags;
} fs_image_t;
typedef struct fs_framebuffer_t {
  char name[TI_PATH_SIZE];
  char color_attachment_image[TI_PATH_SIZE];
  char depth_attachment_image[TI_PATH_SIZE];
} fs_framebuffer_t;
typedef struct fs_block_variable_t {
  char name[TI_PATH_SIZE];
  uint32_t offset;
  uint32_t size;
} fs_block_variable_t;
typedef struct fs_descriptor_binding_t {
  char name[TI_PATH_SIZE];
  uint32_t set;
  uint32_t binding;
  int32_t descriptor_type;
  int32_t block_size;
  int32_t block_variable_count;
  fs_block_variable_t *block_variables;
} fs_descriptor_binding_t;
typedef struct fs_input_variable_t {
  char name[TI_PATH_SIZE];
  uint32_t location;
  uint32_t format;
  uint32_t built_in;
} fs_input_variable_t;
typedef struct fs_pipeline_t {
  char name[TI_PATH_SIZE];
  fs_pipeline_type_t pipeline_type;
  uint8_t enable_blending;
  uint8_t enable_depth_test;
  uint8_t enable_depth_write;
  uint64_t input_variable_count;
  uint64_t descriptor_binding_count;
  uint64_t spirv_vertex_word_count;
  uint64_t spirv_fragment_word_count;
  uint32_t *spirv_vertex_words;
  uint32_t *spirv_fragment_words;
  uint32_t descriptor_set_count;
  VkPrimitiveTopology primitive_topology;
  VkPolygonMode polygon_mode;
  VkCullModeFlags cull_mode_flags;
  fs_input_variable_t *input_variables;
  fs_descriptor_binding_t *descriptor_bindings;
} fs_pipeline_t;
typedef struct fs_font_t {
  char name[TI_PATH_SIZE];
  void *buffer;
  uint64_t buffer_size;
} fs_font_t;
typedef struct fs_swapchain_t {
  char name[TI_PATH_SIZE];
} fs_swapchain_t;
typedef struct fs_renderpass_t {
  char name[TI_PATH_SIZE];
  VkImageLayout initial_color_attachment_layout;
  VkImageLayout initial_depth_attachment_layout;
  VkImageLayout final_color_attachment_layout;
  VkImageLayout final_depth_attachment_layout;
} fs_renderpass_t;
typedef struct fs_renderer_t {
  char name[TI_PATH_SIZE];
  char debug_line_vertex_buffer[TI_PATH_SIZE];
  char debug_line_index_buffer[TI_PATH_SIZE];
  char full_screen_vertex_buffer[TI_PATH_SIZE];
  char full_screen_index_buffer[TI_PATH_SIZE];
} fs_renderer_t;

typedef struct fs_asset_t {
  uint64_t magic;
  fs_asset_type_t type;
  char const *path;
  void *instance;
} fs_asset_t;

#endif // TI_FS_FWD_H
