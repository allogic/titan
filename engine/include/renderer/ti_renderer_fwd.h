#ifndef TI_RENDERER_FWD_H
#define TI_RENDERER_FWD_H

typedef struct time_info_t {
  float time;
  float delta_time;
} time_info_t;
typedef struct screen_info_t {
  ivec2_t resolution;
} screen_info_t;
typedef struct mouse_info_t {
  ivec2_t position;
} mouse_info_t;
typedef struct camera_info_t {
  fvec4_t position;
  fvec4_t direction;
  fmat4x4_t view;
  fmat4x4_t view_inv;
  fmat4x4_t projection;
  fmat4x4_t projection_inv;
  fmat4x4_t view_projection;
  fmat4x4_t view_projection_inv;
  fvec4_t frustum_plane[FRUSTUM_PLANE_COUNT];
} camera_info_t;

TI_STATIC_ASSERT(TI_ALIGN_OF(time_info_t) == 4);
TI_STATIC_ASSERT(TI_ALIGN_OF(screen_info_t) == 4);
TI_STATIC_ASSERT(TI_ALIGN_OF(camera_info_t) == 4);

typedef struct full_screen_vertex_t {
  fvec4_t position;
} full_screen_vertex_t;
typedef struct debug_line_vertex_t {
  fvec4_t position;
  fvec4_t color;
} debug_line_vertex_t;

TI_STATIC_ASSERT(TI_ALIGN_OF(full_screen_vertex_t) == 4);
TI_STATIC_ASSERT(TI_ALIGN_OF(debug_line_vertex_t) == 4);

typedef uint32_t full_screen_index_t;
typedef uint32_t debug_line_index_t;

typedef struct renderer_t {
  uint32_t is_debug_enabled;
  uint32_t image_index;
  time_info_t *time_info;
  screen_info_t *screen_info;
  mouse_info_t *mouse_info;
  camera_info_t *camera_info;
  VkDescriptorBufferInfo time_info_descriptor_buffer_info;
  VkDescriptorBufferInfo screen_info_descriptor_buffer_info;
  VkDescriptorBufferInfo mouse_info_descriptor_buffer_info;
  VkDescriptorBufferInfo camera_info_descriptor_buffer_info;
  VkCommandPool command_pool;
  VkCommandBuffer command_buffer;
} renderer_t;

#endif // TI_RENDERER_FWD_H
