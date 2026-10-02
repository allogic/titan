#ifndef TI_FWD_H
#define TI_FWD_H

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

typedef struct map64_t {
  struct map64_record_t **table;
  uint64_t table_size;
  uint64_t table_count;
  uint64_t record_count;
} map64_t;
typedef struct map64_record_t {
  struct map64_record_t *next;
  uint64_t key;
  uint64_t value;
} map64_record_t;
typedef struct map64_iter_t {
  struct map64_record_t **table;
  struct map64_record_t *table_record;
  uint64_t table_index;
  uint64_t table_count;
  bool32_t first_step;
} map64_iter_t;

typedef struct archive_t {
  char file_path[TI_PATH_SIZE];
  map64_t records;
} archive_t;
typedef struct archive_record_t {
  char name[TI_PATH_SIZE];
  void *buffer;
  uint64_t buffer_size;
  uint64_t global_offset;
} archive_record_t;

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
