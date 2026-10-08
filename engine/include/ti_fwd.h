#ifndef TI_FWD_H
#define TI_FWD_H

typedef enum asset_type_t {
  ASSET_TYPE_NONE = 0,
  ASSET_TYPE_INSTANCE,
  ASSET_TYPE_SWAPCHAIN,
  ASSET_TYPE_BUFFER,
  ASSET_TYPE_MODEL,
  ASSET_TYPE_PIPELINE,
  ASSET_TYPE_FONT,
  ASSET_TYPE_DESCRIPTOR_BINDING,
  ASSET_TYPE_IMAGE,
  ASSET_TYPE_FRAMEBUFFER,
  ASSET_TYPE_RENDERPASS,
  ASSET_TYPE_RENDERER,
  ASSET_TYPE_INPUT_VARIABLE,
  ASSET_TYPE_SCRIPT,
  ASSET_TYPE_SOUND,
  ASSET_TYPE_COUNT,
} asset_type_t;

typedef struct enum_record_t {
  int32_t value;
  char const *name;
} enum_record_t;

typedef uint32_t bool32_t;

typedef struct dalloc_t {
  char const *file_name;
  char const *function_name;
  void *mapping;
  void *data;
  void *stack[TI_DMALLOC_STACK_DEPTH];
  uint64_t line_number;
  uint64_t total_pages;
  uint64_t total_size;
  uint64_t writable_pages;
  uint64_t writable_size;
  uint64_t data_size;
  uint64_t stack_depth;
  time_t time;
} dalloc_t;

typedef struct uuid_t {
  uint8_t bytes[16];
} uuid_t;

typedef struct sha256_t {
  uint8_t bytes[32];
} sha256_t;

typedef struct map64_pool_chunk_t {
  struct map64_pool_chunk_t *next;
  struct map64_record_t *records;
  uint64_t capacity;
} map64_pool_chunk_t;
typedef struct map64_record_t {
  struct map64_record_t *next;
  uint64_t key;
  uint64_t value;
} map64_record_t;
typedef struct map64_t {
  struct map64_record_t **table;
  struct map64_record_t *pool;
  struct map64_record_t *pool_free;
  struct map64_pool_chunk_t *pool_chunks;
  uint64_t table_capacity;
  uint64_t table_count;
  uint64_t pool_capacity;
  uint64_t pool_count;
} map64_t;
typedef struct map64_iter_t {
  struct map64_record_t **table;
  struct map64_record_t *table_record;
  uint64_t table_capacity;
  uint64_t table_index;
  bool32_t first_step;
} map64_iter_t;

typedef struct asset_handle_t {
  asset_type_t type;
  char asset_path[TI_PATH_SIZE];
  uint64_t hash;
  uint64_t buffer_size;
  uint8_t *buffer;
  cJSON *json;
  void *instance;
} asset_handle_t;

typedef struct scene_t {
  char name[TI_PATH_SIZE];
  uint8_t is_running;
  uint8_t *snapshot;
  ecs_world_t *world;
  ecs_entity_t root_entity;
  ecs_entity_t main_camera_entity;
  ecs_query_t *root_children;
} scene_t;

typedef void (*on_create_proc_t)(void);
typedef void (*on_play_proc_t)(void);
typedef void (*on_stop_proc_t)(void);
typedef void (*on_destroy_proc_t)(void);

typedef struct cl_module_t {
  on_create_proc_t on_create_proc;
  on_play_proc_t on_play_proc;
  on_stop_proc_t on_stop_proc;
  on_destroy_proc_t on_destroy_proc;
} cl_module_t;

// TODO: further abstract this..
typedef struct ti_physic_t {
  JPH_PhysicsSystem *system;
  JPH_BodyInterface *body_interface;
  JPH_JobSystem *job_system;
  float accumulator;
  int32_t running;
} ti_physic_t;

#endif // TI_FWD_H
