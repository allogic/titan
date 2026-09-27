#ifndef TI_MAP_H
#define TI_MAP_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void map_create(map_t *map);
uint8_t map_insert(map_t *map, void const *key, uint64_t key_size, void const *value, uint64_t value_size);
uint8_t map_remove(map_t *map, void const *key, uint64_t key_size, void *value, uint64_t value_size);
uint8_t map_contains(map_t *map, void const *key, uint64_t key_size);
uint64_t map_count(map_t *map);
void *map_at(map_t *map, void const *key, uint64_t key_size);
map_iter_t map_iter(map_t *map);
uint8_t map_next(map_iter_t *it);
void *map_key(map_iter_t *it);
uint64_t map_key_size(map_iter_t *it);
void *map_value(map_iter_t *it);
uint64_t map_value_size(map_iter_t *it);
void map_clear(map_t *map);
void map_destroy(map_t *map);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_MAP_H
