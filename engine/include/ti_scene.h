#ifndef TI_SCENE_H
#define TI_SCENE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern scene_t g_scene;

void scene_create(scene_t *scene, char const *file_name, char const *file_path);
void scene_load(scene_t *scene);
void scene_store(scene_t *scene);
void scene_destroy(scene_t *scene);

ecs_entity_t entity_create(scene_t *scene, char const *name, ecs_entity_t parent);
void entity_destroy(scene_t *scene, ecs_entity_t entity);

ecs_query_t *scene_root_children(scene_t *scene, ecs_entity_t entity);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_SCENE_H
