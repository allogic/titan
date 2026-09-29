#include <ti_pch.h>

static void resolve_material_refs(ecs_iter_t *it);
static void resolve_mesh_refs(ecs_iter_t *it);
static void resolve_skeleton_refs(ecs_iter_t *it);
static void resolve_script_refs(ecs_iter_t *it);

static void script_on_create(ecs_iter_t *it);
// static void script_on_play(ecs_iter_t *it);
// static void script_on_stop(ecs_iter_t *it);
static void script_on_destroy(ecs_iter_t *it);

scene_t g_scene = {0};

ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_transform_t, TI_CP_TRANSFORM_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_camera_t, TI_CP_CAMERA_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_material_t, TI_CP_MATERIAL_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_mesh_t, TI_CP_MESH_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_skeleton_t, TI_CP_SKELETON_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_script_t, TI_CP_SCRIPT_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_material_ref_t, TI_CP_MATERIAL_REF_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_mesh_ref_t, TI_CP_MESH_REF_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_skeleton_ref_t, TI_CP_SKELETON_REF_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_script_ref_t, TI_CP_SCRIPT_REF_DESC);

void scene_create(scene_t *scene, char const *file_name, char const *file_path) {
  scene->world = ecs_init();

  ECS_COMPONENT_DEFINE(scene->world, cp_transform_t);
  ECS_COMPONENT_DEFINE(scene->world, cp_camera_t);
  ECS_COMPONENT_DEFINE(scene->world, cp_material_t);
  ECS_COMPONENT_DEFINE(scene->world, cp_mesh_t);
  ECS_COMPONENT_DEFINE(scene->world, cp_skeleton_t);
  ECS_COMPONENT_DEFINE(scene->world, cp_script_t);
  ECS_COMPONENT_DEFINE(scene->world, cp_material_ref_t);
  ECS_COMPONENT_DEFINE(scene->world, cp_mesh_ref_t);
  ECS_COMPONENT_DEFINE(scene->world, cp_skeleton_ref_t);
  ECS_COMPONENT_DEFINE(scene->world, cp_script_ref_t);

  ecs_meta_from_desc(scene->world, ecs_id(cp_transform_t), EcsStructType, TI_CP_TRANSFORM_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_camera_t), EcsStructType, TI_CP_CAMERA_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_material_t), EcsStructType, TI_CP_MATERIAL_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_mesh_t), EcsStructType, TI_CP_MESH_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_skeleton_t), EcsStructType, TI_CP_SKELETON_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_script_t), EcsStructType, TI_CP_SCRIPT_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_material_ref_t), EcsStructType, TI_CP_MATERIAL_REF_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_mesh_ref_t), EcsStructType, TI_CP_MESH_REF_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_skeleton_ref_t), EcsStructType, TI_CP_SKELETON_REF_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_script_ref_t), EcsStructType, TI_CP_SCRIPT_REF_DESC);

  scene->root_entity = ecs_entity(scene->world, {
                                                  .name = "root",
                                                  .parent = 0,
                                                });

  scene->main_camera_entity = ecs_entity(scene->world, {
                                                         .name = "main_camera",
                                                         .parent = scene->root_entity,
                                                       });

  ecs_add(scene->world, scene->main_camera_entity, cp_transform_t);
  ecs_add(scene->world, scene->main_camera_entity, cp_camera_t);
  ecs_add(scene->world, scene->main_camera_entity, cp_script_ref_t);

  cp_transform_t *transform = ecs_get_mut(scene->world, scene->main_camera_entity, cp_transform_t);
  cp_camera_t *camera = ecs_get_mut(scene->world, scene->main_camera_entity, cp_camera_t);
  cp_script_ref_t *script_ref = ecs_get_mut(scene->world, scene->main_camera_entity, cp_script_ref_t);

  cp_transform_init(transform);
  cp_camera_init(camera);

  transform->position_z = -10.0F;

  strcpy(script_ref->module_path, "asset/script/camera_controller.pak");

  ECS_SYSTEM(scene->world, resolve_material_refs, EcsOnStart, cp_material_ref_t);
  ECS_SYSTEM(scene->world, resolve_mesh_refs, EcsOnStart, cp_mesh_ref_t);
  ECS_SYSTEM(scene->world, resolve_skeleton_refs, EcsOnStart, cp_skeleton_ref_t);
  ECS_SYSTEM(scene->world, resolve_script_refs, EcsOnStart, cp_script_ref_t);

  ecs_observer(scene->world, {
                               .query.terms = {{.id = ecs_id(cp_script_t)}},
                               .events = {EcsOnAdd},
                               .callback = script_on_create,
                             });

  ecs_observer(scene->world, {
                               .query.terms = {{.id = ecs_id(cp_script_t)}},
                               .events = {EcsOnRemove},
                               .callback = script_on_destroy,
                             });
}
// TODO
// void scene_load(scene_t *scene, fs_file *file) {
//   uint8_t *buffer = 0;
//   uint64_t buffer_size = 0;
//
//   fs_file_read(file, &buffer, &buffer_size);
//
//     ecs_world_from_json(scene->world, buffer, 0);
//
//     fs_free(buffer, 0);
//   }
// }
// void scene_store(scene_t *scene, fs_file *file) {
//   uint8_t *buffer = ecs_world_to_json(scene->world, 0);
//   uint64_t buffer_size = strlen(buffer);
//
//   if (buffer) {
//
//     fs_file_open_and_write(g_fs, scene->file_path, buffer, buffer_size);
//
//     ecs_os_free(buffer);
//   }
// }
void scene_play(scene_t *scene) {
  if (scene->snapshot) {

    ecs_os_free(scene->snapshot);

    scene->snapshot = 0;
  }

  scene->snapshot = ecs_world_to_json(scene->world, 0);
  scene->is_running = 1;
}
void scene_stop(scene_t *scene) {
  if (scene->snapshot) {

    ecs_world_from_json(scene->world, scene->snapshot, 0);

    ecs_os_free(scene->snapshot);

    scene->snapshot = 0;
  }

  scene->is_running = 0;
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
void scene_destroy(scene_t *scene) {
  if (scene->snapshot) {
    ecs_os_free(scene->snapshot);
  }

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

static void resolve_material_refs(ecs_iter_t *it) {
}
static void resolve_mesh_refs(ecs_iter_t *it) {
}
static void resolve_skeleton_refs(ecs_iter_t *it) {
}
static void resolve_script_refs(ecs_iter_t *it) {
  cp_script_ref_t *reference = ecs_field(it, cp_script_ref_t, 1);

  uint32_t entity_index = 0;
  uint32_t entity_count = it->count;

  while (entity_index < entity_count) {

    ecs_set(it->world, it->entities[entity_index], cp_script_t, {
                                                                  .module_instance = (void *)0xDEADBEEFDEADBEEF,
                                                                });

    ecs_remove(it->world, it->entities[entity_index], cp_script_ref_t);

    entity_index++;
  }
}

static void script_on_create(ecs_iter_t *it) {
  cp_script_t *script = ecs_field(it, cp_script_t, 1);

  uint32_t script_index = 0;
  uint32_t script_count = it->count;

  while (script_index < script_count) {

    cl_module_t *module = (cl_module_t *)script[script_index].module_instance;

    module->on_create_proc();

    script_index++;
  }
}
/*
static void script_on_play(ecs_iter_t *it) {
  cp_script_t *script = ecs_field(it, cp_script_t, 1);

  uint32_t script_index = 0;
  uint32_t script_count = it->count;

  while (script_index < script_count) {

    cl_module_t *module = (cl_module_t *)script[script_index].module_instance;

    module->on_play_proc();

    script_index++;
  }
}
static void script_on_stop(ecs_iter_t *it) {
  cp_script_t *script = ecs_field(it, cp_script_t, 1);

  uint32_t script_index = 0;
  uint32_t script_count = it->count;

  while (script_index < script_count) {

    cl_module_t *module = (cl_module_t *)script[script_index].module_instance;

    module->on_stop_proc();

    script_index++;
  }
}
*/
static void script_on_destroy(ecs_iter_t *it) {
  cp_script_t *script = ecs_field(it, cp_script_t, 1);

  uint32_t script_index = 0;
  uint32_t script_count = it->count;

  while (script_index < script_count) {

    cl_module_t *module = (cl_module_t *)script[script_index].module_instance;

    module->on_destroy_proc();

    script_index++;
  }
}
