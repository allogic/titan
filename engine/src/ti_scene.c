#include <ti_pch.h>

static resolve_all_links(scene_t *scene);

scene_t g_scene = {0};

ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_transform_t, TI_CP_TRANSFORM_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_camera_t, TI_CP_CAMERA_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_material_t, TI_CP_MATERIAL_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_mesh_t, TI_CP_MESH_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_skeleton_t, TI_CP_SKELETON_DESC);

void scene_create(scene_t *scene, char const *file_name, char const *file_path) {
  strcpy(scene->file_name, file_name);
  strcpy(scene->file_path, file_path);

  scene->world = ecs_init();

  ECS_COMPONENT_DEFINE(scene->world, cp_transform_t);
  ECS_COMPONENT_DEFINE(scene->world, cp_camera_t);
  ECS_COMPONENT_DEFINE(scene->world, cp_material_t);
  ECS_COMPONENT_DEFINE(scene->world, cp_mesh_t);
  ECS_COMPONENT_DEFINE(scene->world, cp_skeleton_t);

  ecs_meta_from_desc(scene->world, ecs_id(cp_transform_t), EcsStructType, TI_CP_TRANSFORM_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_camera_t), EcsStructType, TI_CP_CAMERA_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_material_t), EcsStructType, TI_CP_MATERIAL_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_mesh_t), EcsStructType, TI_CP_MESH_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_skeleton_t), EcsStructType, TI_CP_SKELETON_DESC);

  scene->root_entity = ecs_entity(scene->world, {
                                                  .name = "root",
                                                  .parent = 0,
                                                });

  scene->editor_camera_entity = ecs_entity(scene->world, {
                                                           .name = "main_camera",
                                                           .parent = scene->root_entity,
                                                         });

  ecs_add(scene->world, scene->editor_camera_entity, cp_transform_t);
  ecs_add(scene->world, scene->editor_camera_entity, cp_camera_t);

  cp_transform_t *transform = ecs_get_mut(scene->world, scene->editor_camera_entity, cp_transform_t);

  cp_transform_init(transform);

  transform->position_z = -10.0F;

  cp_camera_t *camera = ecs_get_mut(scene->world, scene->editor_camera_entity, cp_camera_t);

  cp_camera_init(camera);
}
void scene_load(scene_t *scene) {
  uint8_t *buffer = 0;
  uint64_t buffer_size = 0;

  if (fs_file_open_and_read(g_fs, scene->file_path, FS_FORMAT_TEXT, &buffer, &buffer_size) == FS_SUCCESS) {

    ecs_world_from_json(scene->world, buffer, 0);

    resolve_all_links(scene);

    fs_free(buffer, 0);
  }
}
void scene_store(scene_t *scene) {
  uint8_t *buffer = ecs_world_to_json(scene->world, 0);
  uint64_t buffer_size = strlen(buffer);

  if (buffer) {

    fs_file_open_and_write(g_fs, scene->file_path, buffer, buffer_size);

    ecs_os_free(buffer);
  }
}
void scene_destroy(scene_t *scene) {
  if (scene->root_children) {

    ecs_query_fini(scene->root_children);
  }

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

static resolve_all_links(scene_t *scene) {
  ecs_query_t *root_children = ecs_query(scene->world, {
                                                         .terms = {
                                                           {
                                                             .id = ecs_pair(EcsChildOf, scene->root_entity),
                                                           },
                                                         },
                                                       });

  ecs_iter_t root_it = ecs_query_iter(scene->world, root_children);

  while (ecs_query_next(&root_it)) {

    uint32_t entity_index = 0;
    uint32_t entity_count = root_it.count;

    while (entity_index < entity_count) {

      ecs_entity_t child_entity = root_it.entities[entity_index];

      cp_transform_t const *transform = ecs_get(scene->world, child_entity, cp_transform_t);
      cp_camera_t const *camera = ecs_get(scene->world, child_entity, cp_camera_t);
      cp_material_t const *material = ecs_get(scene->world, child_entity, cp_material_t);
      cp_mesh_t const *mesh = ecs_get(scene->world, child_entity, cp_mesh_t);
      cp_skeleton_t const *skeleton = ecs_get(scene->world, child_entity, cp_skeleton_t);

      // TODO

      if (material) {

        // fs_pipeline(material->pipeline);
      }

      entity_index++;
    }
  }

  ecs_query_fini(root_children);
}
