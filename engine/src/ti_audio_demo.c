#include <ti_pch.h>

int32_t g_audio_demo_available = 0;
int32_t g_audio_demo_door_open = 1;

static IPLStaticMesh s_rooms = 0;
static IPLStaticMesh s_door = 0;

void ti_audio_demo_create(void) {
  if (g_audio.scene == 0 || s_rooms != 0) {
    return;
  }

  IPLVector3 vertices[] = {
    {-3.0F, -1.5F, -3.0F},
    {9.0F, -1.5F, -3.0F},
    {9.0F, -1.5F, 3.0F},
    {-3.0F, -1.5F, 3.0F},
    {-3.0F, 1.5F, -3.0F},
    {9.0F, 1.5F, -3.0F},
    {9.0F, 1.5F, 3.0F},
    {-3.0F, 1.5F, 3.0F},
    {3.0F, -1.5F, -3.0F},
    {3.0F, 1.5F, -3.0F},
    {3.0F, 1.5F, -0.75F},
    {3.0F, -1.5F, -0.75F},
    {3.0F, -1.5F, 0.75F},
    {3.0F, 1.5F, 0.75F},
    {3.0F, 1.5F, 3.0F},
    {3.0F, -1.5F, 3.0F},
    {3.0F, 1.0F, -0.75F},
    {3.0F, 1.5F, -0.75F},
    {3.0F, 1.5F, 0.75F},
    {3.0F, 1.0F, 0.75F},
  };
  IPLTriangle triangles[] = {
    {{0, 2, 1}},
    {{0, 3, 2}},
    {{4, 5, 6}},
    {{4, 6, 7}},
    {{0, 1, 5}},
    {{0, 5, 4}},
    {{3, 7, 6}},
    {{3, 6, 2}},
    {{0, 4, 7}},
    {{0, 7, 3}},
    {{1, 2, 6}},
    {{1, 6, 5}},
    {{8, 9, 10}},
    {{8, 10, 11}},
    {{12, 13, 14}},
    {{12, 14, 15}},
    {{16, 17, 18}},
    {{16, 18, 19}},
  };
  IPLMaterial stone = {
    .absorption = {0.13F, 0.20F, 0.24F},
    .scattering = 0.05F,
    .transmission = {0.015F, 0.002F, 0.001F},
  };
  IPLint32 material_indices[18] = {0};
  IPLStaticMeshSettings settings = {
    .numVertices = 20,
    .numTriangles = 18,
    .numMaterials = 1,
    .vertices = vertices,
    .triangles = triangles,
    .materialIndices = material_indices,
    .materials = &stone,
  };

  if (iplStaticMeshCreate(g_audio.scene, &settings, &s_rooms) != IPL_STATUS_SUCCESS) {
    return;
  }

  IPLVector3 door_vertices[] = {
    {3.0F, -1.5F, -0.75F},
    {3.0F, 1.0F, -0.75F},
    {3.0F, 1.0F, 0.75F},
    {3.0F, -1.5F, 0.75F},
  };
  IPLTriangle door_triangles[] = {{{0, 1, 2}}, {{0, 2, 3}}};
  IPLMaterial wood = {
    .absorption = {0.11F, 0.07F, 0.06F},
    .scattering = 0.05F,
    .transmission = {0.070F, 0.014F, 0.005F},
  };
  settings.numVertices = 4;
  settings.numTriangles = 2;
  settings.vertices = door_vertices;
  settings.triangles = door_triangles;
  settings.materials = &wood;

  if (iplStaticMeshCreate(g_audio.scene, &settings, &s_door) != IPL_STATUS_SUCCESS) {
    iplStaticMeshRelease(&s_rooms);
    return;
  }

  iplStaticMeshAdd(s_rooms, g_audio.scene);
  iplSceneCommit(g_audio.scene);

  g_audio_demo_door_open = 1;
  g_audio_demo_available = 1;
}

void ti_audio_demo_destroy(void) {
  if (s_door != 0) {
    if (g_audio_demo_door_open == 0) {
      iplStaticMeshRemove(s_door, g_audio.scene);
    }

    iplStaticMeshRelease(&s_door);
  }

  if (s_rooms != 0) {
    iplStaticMeshRemove(s_rooms, g_audio.scene);
    iplStaticMeshRelease(&s_rooms);
    iplSceneCommit(g_audio.scene);
  }

  g_audio_demo_door_open = 1;
  g_audio_demo_available = 0;
}

void ti_audio_demo_door(int open) {
  if (g_audio_demo_available == 0 || open == g_audio_demo_door_open) {
    return;
  }

  if (open != 0) {
    iplStaticMeshRemove(s_door, g_audio.scene);
  } else {
    iplStaticMeshAdd(s_door, g_audio.scene);
  }

  iplSceneCommit(g_audio.scene);
  g_audio_demo_door_open = open;
}
