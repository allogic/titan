#ifndef TI_FWD_H
#define TI_FWD_H

typedef struct scene_t {
  char file_name[TI_PATH_SIZE];
  char file_path[TI_PATH_SIZE];
  ecs_world_t *world;
  ecs_entity_t root_entity;
  ecs_query_t *root_children;
} scene_t;

#endif // TI_FWD_H
