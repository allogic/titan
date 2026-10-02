#ifndef TI_PHYSIC_DEMO_H
#define TI_PHYSIC_DEMO_H

#include <ti_engine.h>

typedef enum ti_physic_demo_shape_t {
  TI_PHYSIC_DEMO_SHAPE_BOX,
  TI_PHYSIC_DEMO_SHAPE_SPHERE,
  TI_PHYSIC_DEMO_SHAPE_CAPSULE,
  TI_PHYSIC_DEMO_SHAPE_CYLINDER,
} ti_physic_demo_shape_t;

typedef struct ti_physic_demo_item_t {
  JPH_BodyID body;
  ti_physic_demo_shape_t shape;
  JPH_RVec3 position;
  JPH_Quat rotation;
  JPH_Vec3 size;
  float radius;
  float height;
} ti_physic_demo_item_t;

typedef struct ti_physic_demo_t {
  JPH_BodyID boundaries[6];
  JPH_Vec3 room_size;
  JPH_Quat room_rotation;
  JPH_DebugRenderer *debug_renderer;
  ti_physic_demo_item_t *items;
  uint32_t item_count;
  int32_t active;
} ti_physic_demo_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern ti_physic_demo_t g_ph_physic_demo;

void ti_physic_demo_create(void);
void ti_physic_demo_room_edit(JPH_Vec3 *size, JPH_Quat *rotation);
void ti_physic_demo_add(ti_physic_demo_item_t *item);
void ti_physic_demo_edit(ti_physic_demo_item_t *item, ti_physic_demo_item_t *settings);
void ti_physic_demo_remove(uint32_t item_index);
void ti_physic_demo_destroy(void);
void ti_physic_demo_reset(void);
void ti_physic_demo_draw(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_PHYSIC_DEMO_H
