#include <physic/ti_ph_world.h>

ti_physic_t g_ph_world = {0};

void ph_world_create(void) {
  int32_t initialized = JPH_Init();

  JPH_ObjectLayerPairFilter *object_layers = JPH_ObjectLayerPairFilterTable_Create(TI_PHYSIC_LAYER_COUNT);
  JPH_ObjectLayerPairFilterTable_EnableCollision(object_layers, TI_PHYSIC_LAYER_STATIC, TI_PHYSIC_LAYER_DYNAMIC);
  JPH_ObjectLayerPairFilterTable_EnableCollision(object_layers, TI_PHYSIC_LAYER_DYNAMIC, TI_PHYSIC_LAYER_DYNAMIC);

  JPH_BroadPhaseLayerInterface *broad_phase_layers = JPH_BroadPhaseLayerInterfaceTable_Create(TI_PHYSIC_LAYER_COUNT, TI_PHYSIC_LAYER_COUNT);
  JPH_BroadPhaseLayerInterfaceTable_MapObjectToBroadPhaseLayer(broad_phase_layers, TI_PHYSIC_LAYER_STATIC, TI_PHYSIC_LAYER_STATIC);
  JPH_BroadPhaseLayerInterfaceTable_MapObjectToBroadPhaseLayer(broad_phase_layers, TI_PHYSIC_LAYER_DYNAMIC, TI_PHYSIC_LAYER_DYNAMIC);

  JPH_PhysicsSystemSettings settings = {
    .maxBodies = 1024,
    .maxBodyPairs = 1024,
    .maxContactConstraints = 1024,
    .broadPhaseLayerInterface = broad_phase_layers,
    .objectLayerPairFilter = object_layers,
    .objectVsBroadPhaseLayerFilter = JPH_ObjectVsBroadPhaseLayerFilterTable_Create(broad_phase_layers, TI_PHYSIC_LAYER_COUNT, object_layers, TI_PHYSIC_LAYER_COUNT),
  };

  g_ph_world.system = JPH_PhysicsSystem_Create(&settings);
  assert(g_ph_world.system != 0);
  g_ph_world.body_interface = JPH_PhysicsSystem_GetBodyInterface(g_ph_world.system);

  JobSystemThreadPoolConfig job_settings = {
    .numThreads = 2,
  };

  g_ph_world.job_system = JPH_JobSystemThreadPool_Create(&job_settings);
  assert(g_ph_world.job_system != 0);
}
void ph_world_destroy(void) {
  if (g_ph_world.system == 0) {
    return;
  }

  JPH_PhysicsSystem_Destroy(g_ph_world.system);
  JPH_JobSystem_Destroy(g_ph_world.job_system);
  JPH_Shutdown();

  g_ph_world = (ti_physic_t){0};
}
void ph_world_update(float delta_time) {
  if (g_ph_world.running == 0) {
    g_ph_world.accumulator = 0.0F;
    return;
  }

  g_ph_world.accumulator += delta_time;

  while (g_ph_world.accumulator >= TI_PHYSIC_TIME_STEP) {
    ph_world_step();
    g_ph_world.accumulator -= TI_PHYSIC_TIME_STEP;
  }
}
void ph_world_step(void) {
  JPH_PhysicsUpdateError result = JPH_PhysicsSystem_Update(g_ph_world.system, TI_PHYSIC_TIME_STEP, 1, g_ph_world.job_system);
  assert(result == JPH_PhysicsUpdateError_None);
}
