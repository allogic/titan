#include <ti_pch.h>

static void resolve_material_refs(ecs_iter_t *it);
static void resolve_mesh_refs(ecs_iter_t *it);
static void resolve_skeleton_refs(ecs_iter_t *it);
static void resolve_script_refs(ecs_iter_t *it);

static void script_on_create(ecs_iter_t *it);
// static void script_on_play(ecs_iter_t *it);
// static void script_on_stop(ecs_iter_t *it);
static void script_on_destroy(ecs_iter_t *it);

static void update_velocity(ecs_iter_t *it);
static void update_editor_camera_controller(ecs_iter_t *it);

scene_t g_scene = {0};

ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_transform_t, TI_CP_TRANSFORM_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_camera_t, TI_CP_CAMERA_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_velocity_t, TI_CP_VELOCITY_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_editor_camera_controller_t, TI_CP_EDITOR_CAMERA_CONTROLLER_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_material_t, TI_CP_MATERIAL_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_mesh_t, TI_CP_MESH_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_skeleton_t, TI_CP_SKELETON_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_script_t, TI_CP_SCRIPT_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_material_ref_t, TI_CP_MATERIAL_REF_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_mesh_ref_t, TI_CP_MESH_REF_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_skeleton_ref_t, TI_CP_SKELETON_REF_DESC);
ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, cp_script_ref_t, TI_CP_SCRIPT_REF_DESC);

void scene_create(scene_t *scene) {
  scene->world = ecs_init();

  ECS_COMPONENT_DEFINE(scene->world, cp_transform_t);
  ECS_COMPONENT_DEFINE(scene->world, cp_camera_t);
  ECS_COMPONENT_DEFINE(scene->world, cp_velocity_t);
  ECS_COMPONENT_DEFINE(scene->world, cp_editor_camera_controller_t);
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
  ecs_meta_from_desc(scene->world, ecs_id(cp_velocity_t), EcsStructType, TI_CP_VELOCITY_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_editor_camera_controller_t), EcsStructType, TI_CP_EDITOR_CAMERA_CONTROLLER_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_material_t), EcsStructType, TI_CP_MATERIAL_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_mesh_t), EcsStructType, TI_CP_MESH_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_skeleton_t), EcsStructType, TI_CP_SKELETON_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_script_t), EcsStructType, TI_CP_SCRIPT_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_material_ref_t), EcsStructType, TI_CP_MATERIAL_REF_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_mesh_ref_t), EcsStructType, TI_CP_MESH_REF_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_skeleton_ref_t), EcsStructType, TI_CP_SKELETON_REF_DESC);
  ecs_meta_from_desc(scene->world, ecs_id(cp_script_ref_t), EcsStructType, TI_CP_SCRIPT_REF_DESC);

  ECS_SYSTEM(scene->world, resolve_material_refs, EcsOnStart, cp_material_ref_t);
  ECS_SYSTEM(scene->world, resolve_mesh_refs, EcsOnStart, cp_mesh_ref_t);
  ECS_SYSTEM(scene->world, resolve_skeleton_refs, EcsOnStart, cp_skeleton_ref_t);
  ECS_SYSTEM(scene->world, resolve_script_refs, EcsOnStart, cp_script_ref_t);

  ECS_SYSTEM(scene->world, update_velocity, EcsOnUpdate, cp_velocity_t, cp_transform_t);
  ECS_SYSTEM(scene->world, update_editor_camera_controller, EcsOnUpdate, cp_editor_camera_controller_t, cp_transform_t, cp_velocity_t);

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
  ecs_add(scene->world, scene->main_camera_entity, cp_velocity_t);
  ecs_add(scene->world, scene->main_camera_entity, cp_editor_camera_controller_t);
  // ecs_add(scene->world, scene->main_camera_entity, cp_script_ref_t);

  cp_transform_t *transform = ecs_get_mut(scene->world, scene->main_camera_entity, cp_transform_t);
  cp_camera_t *camera = ecs_get_mut(scene->world, scene->main_camera_entity, cp_camera_t);
  cp_velocity_t *velocity = ecs_get_mut(scene->world, scene->main_camera_entity, cp_velocity_t);
  cp_editor_camera_controller_t *editor_camera_controller = ecs_get_mut(scene->world, scene->main_camera_entity, cp_editor_camera_controller_t);
  // cp_script_ref_t *script_ref = ecs_get_mut(scene->world, scene->main_camera_entity, cp_script_ref_t);

  cp_transform_init(transform);
  cp_camera_init(camera);
  cp_velocity_init(velocity);
  cp_editor_camera_controller_init(editor_camera_controller);

  transform->position_z = -10.0F;

  velocity->linear_drag = 25.0F;
  velocity->angular_drag = 35.0F;

  // strcpy(script_ref->module_path, "asset/script/camera_controller.pak");
}
void scene_update(scene_t *scene) {
  ecs_progress(scene->world, g_window.delta_time);
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
void scene_pause(scene_t *scene) {
  // TODO
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
  cp_script_ref_t *script_references = ecs_field(it, cp_script_ref_t, 0);

  uint32_t index = 0;
  uint32_t count = it->count;

  while (index < count) {

    ecs_set(it->world, it->entities[index], cp_script_t, {
                                                           .module_instance = (void *)0xDEADBEEFDEADBEEF,
                                                         });

    ecs_remove(it->world, it->entities[index], cp_script_ref_t);

    index++;
  }
}

static void script_on_create(ecs_iter_t *it) {
  cp_script_t *scripts = ecs_field(it, cp_script_t, 0);

  uint32_t index = 0;
  uint32_t count = it->count;

  while (index < count) {

    cp_script_t *script = &scripts[index];
    cl_module_t *module = (cl_module_t *)script->module_instance;

    // module->on_create_proc();

    index++;
  }
}
/*
static void script_on_play(ecs_iter_t *it) {
  cp_script_t *script = ecs_field(it, cp_script_t, 0);

  uint32_t script_index = 0;
  uint32_t script_count = it->count;

  while (script_index < script_count) {

    cl_module_t *module = (cl_module_t *)script[script_index].module_instance;

    module->on_play_proc();

    script_index++;
  }
}
static void script_on_stop(ecs_iter_t *it) {
  cp_script_t *script = ecs_field(it, cp_script_t, 0);

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
  cp_script_t *scripts = ecs_field(it, cp_script_t, 0);

  uint32_t index = 0;
  uint32_t count = it->count;

  while (index < count) {

    cp_script_t *script = &scripts[index];
    cl_module_t *module = (cl_module_t *)script->module_instance;

    // module->on_destroy_proc();

    index++;
  }
}

static void update_velocity(ecs_iter_t *it) {
  cp_velocity_t *velocities = ecs_field(it, cp_velocity_t, 0);
  cp_transform_t *transforms = ecs_field(it, cp_transform_t, 1);

  uint32_t index = 0;
  uint32_t count = it->count;

  while (index < count) {

    cp_transform_t *transform = &transforms[index];
    cp_velocity_t *velocity = &velocities[index];

    cp_velocity_update(velocity, transform);

    index++;
  }
}
static void update_editor_camera_controller(ecs_iter_t *it) {
  cp_editor_camera_controller_t *editor_camera_controllers = ecs_field(it, cp_editor_camera_controller_t, 0);
  cp_transform_t *transforms = ecs_field(it, cp_transform_t, 1);
  cp_velocity_t *velocities = ecs_field(it, cp_velocity_t, 2);

  uint32_t index = 0;
  uint32_t count = it->count;

  while (index < count) {

    cp_editor_camera_controller_t *editor_camera_controller = &editor_camera_controllers[index];
    cp_transform_t *transform = &transforms[index];
    cp_velocity_t *velocity = &velocities[index];

    cp_editor_camera_controller_update(editor_camera_controller, transform, velocity);

    index++;
  }
}
