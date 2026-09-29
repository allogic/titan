#include <ti_pch.h>

static void JPH_API_CALL draw_line(void *user_data, JPH_RVec3 const *from, JPH_RVec3 const *to, JPH_Color color);
static JPH_Shape *create_shape(ti_physic_demo_item_t *item);
static int constrain_item(JPH_Shape const *shape, JPH_Vec3 *room_size, JPH_Quat *room_rotation, JPH_RVec3 *position, JPH_Quat *rotation);

static JPH_DebugRenderer_Procs s_draw_procs = {
  .DrawLine = draw_line,
};

ti_physic_demo_t g_physic_demo = {0};

void ti_physic_demo_create(void) {
  if (g_physic_demo.active != 0) {
    return;
  }

  JPH_DebugRenderer_SetProcs(&s_draw_procs);
  g_physic_demo.debug_renderer = JPH_DebugRenderer_Create(0);

  JPH_Vec3 room_size = {10.0F, 10.0F, 10.0F};
  JPH_Quat room_rotation = {0.0F, 0.0F, 0.0F, 1.0F};
  ti_physic_demo_room_edit(&room_size, &room_rotation);

  ti_physic_demo_item_t items[] = {
    {.shape = TI_PHYSIC_DEMO_SHAPE_BOX, .position = {-2.0F, 3.0F, -1.5F}, .rotation = {0.0F, 0.0F, 0.0F, 1.0F}, .size = {1.0F, 1.0F, 1.0F}},
    {.shape = TI_PHYSIC_DEMO_SHAPE_SPHERE, .position = {2.0F, 3.0F, -1.5F}, .rotation = {0.0F, 0.0F, 0.0F, 1.0F}, .radius = 0.5F},
    {.shape = TI_PHYSIC_DEMO_SHAPE_CAPSULE, .position = {-2.0F, 3.0F, 1.5F}, .rotation = {0.0F, 0.0F, 0.0F, 1.0F}, .radius = 0.5F, .height = 1.0F},
    {.shape = TI_PHYSIC_DEMO_SHAPE_CYLINDER, .position = {2.0F, 3.0F, 1.5F}, .rotation = {0.0F, 0.0F, 0.0F, 1.0F}, .radius = 0.5F, .height = 1.0F},
  };
  uint32_t item_index = 0;

  while (item_index < TI_ARRAY_COUNT(items)) {
    ti_physic_demo_add(&items[item_index]);
    item_index++;
  }

  JPH_PhysicsSystem_OptimizeBroadPhase(g_physic.system);

  g_physic.accumulator = 0.0F;
  g_physic.running = 0;
  g_physic_demo.active = 1;
}

void ti_physic_demo_room_edit(JPH_Vec3 *size, JPH_Quat *rotation) {
  uint32_t item_index = 0;

  while (item_index < g_physic_demo.item_count) {
    ti_physic_demo_item_t *item = &g_physic_demo.items[item_index];
    JPH_Shape const *shape = JPH_BodyInterface_GetShape(g_physic.body_interface, item->body);
    JPH_RVec3 position;
    JPH_Quat item_rotation;
    JPH_BodyInterface_GetPositionAndRotation(g_physic.body_interface, item->body, &position, &item_rotation);

    if (constrain_item(shape, size, rotation, &position, &item_rotation) == 0) {
      return;
    }

    position = item->position;

    if (constrain_item(shape, size, rotation, &position, &item->rotation) == 0) {
      return;
    }

    item_index++;
  }

  g_physic_demo.room_size = *size;
  g_physic_demo.room_rotation = *rotation;

  JPH_Vec3 half_size = {size->x * 0.5F, size->y * 0.5F, size->z * 0.5F};
  JPH_Vec3 positions[] = {
    {0.0F, -half_size.y - 0.5F, 0.0F},
    {0.0F, half_size.y + 0.5F, 0.0F},
    {-half_size.x - 0.5F, 0.0F, 0.0F},
    {half_size.x + 0.5F, 0.0F, 0.0F},
    {0.0F, 0.0F, -half_size.z - 0.5F},
    {0.0F, 0.0F, half_size.z + 0.5F},
  };
  JPH_Vec3 half_extents[] = {
    {half_size.x + 1.0F, 0.5F, half_size.z + 1.0F},
    {half_size.x + 1.0F, 0.5F, half_size.z + 1.0F},
    {0.5F, half_size.y, half_size.z + 1.0F},
    {0.5F, half_size.y, half_size.z + 1.0F},
    {half_size.x + 1.0F, half_size.y, 0.5F},
    {half_size.x + 1.0F, half_size.y, 0.5F},
  };
  uint32_t boundary_index = 0;

  while (boundary_index < TI_ARRAY_COUNT(g_physic_demo.boundaries)) {
    JPH_RVec3 position;
    JPH_Quat_Rotate(rotation, &positions[boundary_index], &position);
    position.y += half_size.y;

    JPH_BoxShape *shape = JPH_BoxShape_Create(&half_extents[boundary_index], 0.0F);

    if (g_physic_demo.active == 0) {
      JPH_BodyCreationSettings *settings = JPH_BodyCreationSettings_Create3((JPH_Shape *)shape, &position, rotation, JPH_MotionType_Static, TI_PHYSIC_LAYER_STATIC);
      g_physic_demo.boundaries[boundary_index] = JPH_BodyInterface_CreateAndAddBody(g_physic.body_interface, settings, JPH_Activation_DontActivate);
      assert(g_physic_demo.boundaries[boundary_index] != UINT32_MAX);
      JPH_BodyCreationSettings_Destroy(settings);
    } else {
      JPH_BodyInterface_SetShape(g_physic.body_interface, g_physic_demo.boundaries[boundary_index], (JPH_Shape *)shape, false, JPH_Activation_DontActivate);
      JPH_BodyInterface_SetPositionAndRotation(g_physic.body_interface, g_physic_demo.boundaries[boundary_index], &position, rotation, JPH_Activation_DontActivate);
    }

    JPH_Shape_Destroy((JPH_Shape *)shape);
    boundary_index++;
  }

  JPH_Vec3 velocity = {0};
  item_index = 0;

  while (item_index < g_physic_demo.item_count) {
    ti_physic_demo_item_t *item = &g_physic_demo.items[item_index];
    JPH_Shape const *shape = JPH_BodyInterface_GetShape(g_physic.body_interface, item->body);
    JPH_RVec3 position;
    JPH_Quat item_rotation;
    JPH_BodyInterface_GetPositionAndRotation(g_physic.body_interface, item->body, &position, &item_rotation);

    constrain_item(shape, size, rotation, &position, &item_rotation);
    constrain_item(shape, size, rotation, &item->position, &item->rotation);
    JPH_BodyInterface_SetPositionAndRotation(g_physic.body_interface, item->body, &position, &item_rotation, JPH_Activation_Activate);
    JPH_BodyInterface_SetLinearAndAngularVelocity(g_physic.body_interface, item->body, &velocity, &velocity);
    item_index++;
  }
}

void ti_physic_demo_add(ti_physic_demo_item_t *item) {
  assert(g_physic_demo.item_count < TI_PHYSIC_DEMO_ITEM_LIMIT);
  JPH_Shape *shape = create_shape(item);

  if (constrain_item(shape, &g_physic_demo.room_size, &g_physic_demo.room_rotation, &item->position, &item->rotation) == 0) {
    JPH_Shape_Destroy(shape);
    return;
  }

  JPH_BodyCreationSettings *settings = JPH_BodyCreationSettings_Create3(shape, &item->position, &item->rotation, JPH_MotionType_Dynamic, TI_PHYSIC_LAYER_DYNAMIC);
  JPH_BodyCreationSettings_SetMotionQuality(settings, JPH_MotionQuality_LinearCast);
  JPH_BodyID body = JPH_BodyInterface_CreateAndAddBody(g_physic.body_interface, settings, JPH_Activation_Activate);
  assert(body != UINT32_MAX);

  JPH_BodyCreationSettings_Destroy(settings);
  JPH_Shape_Destroy(shape);

  uint32_t item_count = g_physic_demo.item_count;
  ti_physic_demo_item_t *items = TI_ALLOC(sizeof(ti_physic_demo_item_t) * (item_count + 1), 0, 0);

  if (item_count > 0) {
    memcpy(items, g_physic_demo.items, sizeof(ti_physic_demo_item_t) * item_count);
  }

  items[item_count] = *item;
  items[item_count].body = body;

  if (g_physic_demo.items != 0) {
    TI_FREE(g_physic_demo.items);
  }

  g_physic_demo.items = items;
  g_physic_demo.item_count++;
}

void ti_physic_demo_edit(ti_physic_demo_item_t *item, ti_physic_demo_item_t *settings) {
  JPH_Shape const *shape = JPH_BodyInterface_GetShape(g_physic.body_interface, item->body);
  JPH_Shape *resized = 0;

  if ((settings->size.x != item->size.x) || (settings->size.y != item->size.y) || (settings->size.z != item->size.z) ||
      (settings->radius != item->radius) || (settings->height != item->height)) {
    resized = create_shape(settings);
    shape = resized;
  }

  if (constrain_item(shape, &g_physic_demo.room_size, &g_physic_demo.room_rotation, &settings->position, &settings->rotation) == 0) {
    if (resized != 0) {
      JPH_Shape_Destroy(resized);
    }
    return;
  }

  if (resized != 0) {
    JPH_BodyInterface_SetShape(g_physic.body_interface, item->body, shape, true, JPH_Activation_Activate);
    JPH_Shape_Destroy(resized);
    item->size = settings->size;
    item->radius = settings->radius;
    item->height = settings->height;
  }

  item->position = settings->position;
  item->rotation = settings->rotation;

  JPH_Vec3 velocity = {0};
  JPH_BodyInterface_SetPositionAndRotation(g_physic.body_interface, item->body, &item->position, &item->rotation, JPH_Activation_Activate);
  JPH_BodyInterface_SetLinearAndAngularVelocity(g_physic.body_interface, item->body, &velocity, &velocity);
}

void ti_physic_demo_remove(uint32_t item_index) {
  JPH_BodyInterface_RemoveAndDestroyBody(g_physic.body_interface, g_physic_demo.items[item_index].body);

  g_physic_demo.item_count--;
  memmove(&g_physic_demo.items[item_index], &g_physic_demo.items[item_index + 1], sizeof(ti_physic_demo_item_t) * (g_physic_demo.item_count - item_index));
}

void ti_physic_demo_destroy(void) {
  if (g_physic_demo.active == 0) {
    return;
  }

  g_physic.running = 0;
  g_physic.accumulator = 0.0F;

  uint32_t item_index = 0;

  while (item_index < g_physic_demo.item_count) {
    JPH_BodyInterface_RemoveAndDestroyBody(g_physic.body_interface, g_physic_demo.items[item_index].body);
    item_index++;
  }

  uint32_t boundary_index = 0;

  while (boundary_index < TI_ARRAY_COUNT(g_physic_demo.boundaries)) {
    JPH_BodyInterface_RemoveAndDestroyBody(g_physic.body_interface, g_physic_demo.boundaries[boundary_index]);
    boundary_index++;
  }

  JPH_DebugRenderer_Destroy(g_physic_demo.debug_renderer);
  TI_FREE(g_physic_demo.items);
  g_physic_demo = (ti_physic_demo_t){0};
}

void ti_physic_demo_reset(void) {
  g_physic.running = 0;
  g_physic.accumulator = 0.0F;

  JPH_Vec3 velocity = {0};
  uint32_t item_index = 0;

  while (item_index < g_physic_demo.item_count) {
    ti_physic_demo_item_t *item = &g_physic_demo.items[item_index];
    JPH_BodyInterface_SetPositionAndRotation(g_physic.body_interface, item->body, &item->position, &item->rotation, JPH_Activation_Activate);
    JPH_BodyInterface_SetLinearAndAngularVelocity(g_physic.body_interface, item->body, &velocity, &velocity);
    item_index++;
  }
}

void ti_physic_demo_draw(void) {
  if (g_physic_demo.active == 0) {
    return;
  }

  g_vk_renderer.is_debug_enabled = 1;

  JPH_Vec3 room_size = g_physic_demo.room_size;
  float radius = 0.5F * sqrtf((room_size.x + 2.0F) * (room_size.x + 2.0F) + (room_size.y + 2.0F) * (room_size.y + 2.0F) + (room_size.z + 2.0F) * (room_size.z + 2.0F));
  float aspect_ratio = 1.0F; // (float)g_vk_main_framebuffer.width / g_vk_main_framebuffer.height;
  float half_fov = atanf(tanf(deg_to_rad(22.5F)) * fminf(aspect_ratio, 1.0F));
  float distance = radius / sinf(half_fov) * 1.05F;
  fvec3_t camera_target = {0.0F, room_size.y * 0.5F, 0.0F};
  fvec3_t camera_position = fvec3_add(camera_target, fvec3_muls(fvec3_norm((fvec3_t){16.0F, 8.0F, -20.0F}), distance));
  fvec3_t camera_direction = fvec3_norm(fvec3_sub(camera_target, camera_position));
  vk_camera_info_t *camera = g_vk_camera_info_buffer.device_data;

  camera->position = (fvec4_t){camera_position.x, camera_position.y, camera_position.z, 0.0F};
  camera->direction = (fvec4_t){camera_direction.x, camera_direction.y, camera_direction.z, 0.0F};
  camera->view = fmat4x4_look_at(camera_position, camera_target, (fvec3_t){0.0F, 1.0F, 0.0F});
  camera->view_inv = fmat4x4_inverse(camera->view);
  camera->projection = fmat4x4_persp(deg_to_rad(45.0F), aspect_ratio, 0.1F, distance + radius * 2.0F);
  camera->projection.m11 *= -1.0F;
  camera->projection_inv = fmat4x4_inverse(camera->projection);
  camera->view_projection = fmat4x4_mul(camera->view, camera->projection);
  camera->view_projection_inv = fmat4x4_inverse(camera->view_projection);

  JPH_RVec3 debug_camera = {camera_position.x, camera_position.y, camera_position.z};
  JPH_DrawSettings settings = {
    .drawShape = true,
    .drawShapeWireframe = true,
    .drawShapeColor = JPH_BodyManager_ShapeColor_MotionTypeColor,
  };

  JPH_DebugRenderer_SetCameraPos(g_physic_demo.debug_renderer, &debug_camera);
  JPH_PhysicsSystem_DrawBodies(g_physic.system, &settings, g_physic_demo.debug_renderer, 0);
  JPH_DebugRenderer_NextFrame(g_physic_demo.debug_renderer);
}

static JPH_Shape *create_shape(ti_physic_demo_item_t *item) {
  JPH_Shape *shape = 0;

  switch (item->shape) {
    case TI_PHYSIC_DEMO_SHAPE_BOX: {
      JPH_Vec3 half_extent = {item->size.x * 0.5F, item->size.y * 0.5F, item->size.z * 0.5F};
      shape = (JPH_Shape *)JPH_BoxShape_Create(&half_extent, JPH_DEFAULT_CONVEX_RADIUS);
      break;
    }
    case TI_PHYSIC_DEMO_SHAPE_SPHERE: {
      shape = (JPH_Shape *)JPH_SphereShape_Create(item->radius);
      break;
    }
    case TI_PHYSIC_DEMO_SHAPE_CAPSULE: {
      shape = (JPH_Shape *)JPH_CapsuleShape_Create(item->height * 0.5F, item->radius);
      break;
    }
    case TI_PHYSIC_DEMO_SHAPE_CYLINDER: {
      shape = (JPH_Shape *)JPH_CylinderShape_Create(item->height * 0.5F, item->radius);
      break;
    }
  }

  assert(shape != 0);
  return shape;
}

static int constrain_item(JPH_Shape const *shape, JPH_Vec3 *room_size, JPH_Quat *room_rotation, JPH_RVec3 *position, JPH_Quat *rotation) {
  JPH_Quat room_inverse;
  JPH_Quat local_rotation;
  JPH_RMat4 transform;
  JPH_AABox bounds;
  JPH_Vec3 scale = {1.0F, 1.0F, 1.0F};
  JPH_Quat_Inversed(room_rotation, &room_inverse);
  JPH_Quat_Multiply(&room_inverse, rotation, &local_rotation);
  JPH_Mat4_Rotation(&transform, &local_rotation);
  JPH_Shape_GetWorldSpaceBounds(shape, &transform, &scale, &bounds);

  if ((bounds.max.x - bounds.min.x > room_size->x) ||
      (bounds.max.y - bounds.min.y > room_size->y) ||
      (bounds.max.z - bounds.min.z > room_size->z)) {
    return 0;
  }

  JPH_Vec3 half_size = {room_size->x * 0.5F, room_size->y * 0.5F, room_size->z * 0.5F};
  JPH_Vec3 offset = {position->x, position->y - half_size.y, position->z};
  JPH_Vec3 local_position;
  JPH_Quat_InverseRotate(room_rotation, &offset, &local_position);
  local_position.x = clampf(local_position.x, -half_size.x - bounds.min.x, half_size.x - bounds.max.x);
  local_position.y = clampf(local_position.y, -half_size.y - bounds.min.y, half_size.y - bounds.max.y);
  local_position.z = clampf(local_position.z, -half_size.z - bounds.min.z, half_size.z - bounds.max.z);
  JPH_Quat_Rotate(room_rotation, &local_position, position);
  position->y += half_size.y;
  return 1;
}

static void JPH_API_CALL draw_line(void *user_data, JPH_RVec3 const *from, JPH_RVec3 const *to, JPH_Color color) {
  fvec4_t line_color = {
    (color & 255) / 255.0F,
    ((color >> 8) & 255) / 255.0F,
    ((color >> 16) & 255) / 255.0F,
    ((color >> 24) & 255) / 255.0F,
  };

  vk_renderer_draw_debug_line(&g_vk_renderer, (fvec3_t){from->x, from->y, from->z}, (fvec3_t){to->x, to->y, to->z}, line_color);
}
