#ifndef TI_FWD_H
#define TI_FWD_H

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

typedef struct map_t {
  struct map_record_t **table;
  uint64_t table_size;
  uint64_t table_count;
  uint64_t record_count;
} map_t;
typedef struct map_record_t {
  struct map_record_t *next;
  uint8_t *key;
  uint64_t key_size;
  uint8_t *value;
  uint64_t value_size;
} map_record_t;
typedef struct map_iter_t {
  struct map_record_t **table;
  struct map_record_t *table_record;
  uint64_t table_index;
  uint64_t table_count;
  uint8_t first_step;
} map_iter_t;

typedef struct archive_record_t {
  char name[TI_PATH_SIZE];
  void *buffer;
  uint64_t buffer_size;
  uint64_t global_offset;
} archive_record_t;
typedef struct archive_t {
  char file_path[TI_PATH_SIZE];
  map_t records;
} archive_t;

typedef struct scene_t {
  char file_name[TI_PATH_SIZE];
  char file_path[TI_PATH_SIZE];
  ecs_world_t *world;
  ecs_entity_t root_entity;
  ecs_query_t *root_children;
} scene_t;

#endif // TI_FWD_H
