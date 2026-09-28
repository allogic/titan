#include <ti_pch.h>

ti_physic_t g_physic = {0};

void ti_physic_create(void) {
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

  g_physic.system = JPH_PhysicsSystem_Create(&settings);
  assert(g_physic.system != 0);
  g_physic.body_interface = JPH_PhysicsSystem_GetBodyInterface(g_physic.system);

  JobSystemThreadPoolConfig job_settings = {
    .numThreads = 2,
  };

  g_physic.job_system = JPH_JobSystemThreadPool_Create(&job_settings);
  assert(g_physic.job_system != 0);
}

void ti_physic_destroy(void) {
  if (g_physic.system == 0) {
    return;
  }

  JPH_PhysicsSystem_Destroy(g_physic.system);
  JPH_JobSystem_Destroy(g_physic.job_system);
  JPH_Shutdown();

  g_physic = (ti_physic_t){0};
}

void ti_physic_update(float delta_time) {
  if (g_physic.running == 0) {
    g_physic.accumulator = 0.0F;
    return;
  }

  g_physic.accumulator += delta_time;

  while (g_physic.accumulator >= TI_PHYSIC_TIME_STEP) {
    ti_physic_step();
    g_physic.accumulator -= TI_PHYSIC_TIME_STEP;
  }
}

void ti_physic_step(void) {
  JPH_PhysicsUpdateError result = JPH_PhysicsSystem_Update(g_physic.system, TI_PHYSIC_TIME_STEP, 1, g_physic.job_system);
  assert(result == JPH_PhysicsUpdateError_None);
}
