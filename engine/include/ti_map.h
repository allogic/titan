#ifndef TI_MAP_H
#define TI_MAP_H

#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void map64_create(map64_t *map);
bool32_t map64_insert(map64_t *map, uint64_t key, uint64_t value);
bool32_t map64_remove(map64_t *map, uint64_t key, uint64_t *value);
bool32_t map64_contains(map64_t *map, uint64_t key);
uint64_t map64_count(map64_t *map);
uint64_t *map64_at(map64_t *map, uint64_t key);
map64_iter_t map64_iter(map64_t *map);
void map64_clear(map64_t *map);
void map64_destroy(map64_t *map);

bool32_t map64_next(map64_iter_t *it);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_MAP_H
