#include <ti_pch.h>

static scene_resolve_all_references(void);

scene_t g_scene = {0};

ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, transform_t, TI_TRANSFORM_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, camera_t, TI_CAMERA_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, material_t, TI_MATERIAL_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, mesh_t, TI_MESH_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, skeleton_t, TI_SKELETON_DESC);

void scene_create(scene_t *scene, char const *file_name, char const *file_path) {
  strcpy(scene->file_name, file_name);
  strcpy(scene->file_path, file_path);

  scene->world = ecs_init();

  ECS_COMPONENT_DEFINE(scene->world, transform_t);
  ECS_COMPONENT_DEFINE(scene->world, camera_t);
  ECS_COMPONENT_DEFINE(scene->world, material_t);
  ECS_COMPONENT_DEFINE(scene->world, mesh_t);
  ECS_COMPONENT_DEFINE(scene->world, skeleton_t);

  ecs_meta_from_desc(scene->world, ecs_id(transform_t), EcsStructType, TI_TRANSFORM_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(camera_t), EcsStructType, TI_CAMERA_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(material_t), EcsStructType, TI_MATERIAL_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(mesh_t), EcsStructType, TI_MESH_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(skeleton_t), EcsStructType, TI_SKELETON_DESC);

  scene->root_entity = ecs_entity(scene->world, {
                                                  .name = "root",
                                                  .parent = 0,
                                                });
}
void scene_load(scene_t *scene) {
  uint8_t *buffer = 0;
  uint64_t buffer_size = 0;
  fsutil_load_binary(&buffer, &buffer_size, scene->file_path);

  if (buffer) {

    ecs_world_from_json(scene->world, buffer, 0);

    scene_resolve_all_references();

    TI_FREE(buffer);
  }
}
void scene_store(scene_t *scene) {
  uint8_t *buffer = ecs_world_to_json(scene->world, 0);
  uint64_t buffer_size = strlen(buffer);

  if (buffer) {

    fsutil_save_binary(buffer, buffer_size, scene->file_path);

    ecs_os_free(buffer);
  }
}
void scene_destroy(scene_t *scene) {
  ecs_query_fini(scene->root_children);

  ecs_fini(scene->world);
}

ecs_entity_t entity_create(scene_t *scene, char const *name, ecs_entity_t parent) {
  if (parent == 0) {
    parent = g_scene.root_entity;
  }

  return ecs_entity(scene->world, {
                                    .name = name,
                                    .parent = parent,
                                  });
}
void entity_destroy(scene_t *scene, ecs_entity_t entity) {
  scene->root_children = 0;

  ecs_delete(scene->world, entity);
}

ecs_query_t *scene_root_children(scene_t *scene, ecs_entity_t entity) {
  if (scene->root_children == 0) {

    scene->root_children = ecs_query(scene->world, {
                                                     .terms = {
                                                       {
                                                         .id = ecs_pair(EcsChildOf, scene->root_entity),
                                                       },
                                                     },
                                                   });
  }

  return scene->root_children;
}

static scene_resolve_all_references(void) {
}
