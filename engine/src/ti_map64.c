#include <ti_map64.h>

static bool32_t table_expand(map64_t *map);
static void pool_expand(map64_t *map);

static map64_record_t *pool_alloc(map64_t *map);
static void pool_free(map64_t *map, map64_record_t *record);

void map64_create(map64_t *map) {
  uint64_t table_size = sizeof(map64_record_t *) * TI_MAP_TABLE_CAPACITY;

  map->table = (map64_record_t **)TI_ALLOC(table_size, 1, 0);
  map->table_capacity = TI_MAP_TABLE_CAPACITY;
  map->table_count = 0;
  map->pool = 0;
  map->pool_capacity = 0;
  map->pool_count = 0;
  map->pool_free = 0;

  memset(map->table, 0, table_size);

  pool_expand(map);
}
bool32_t map64_insert(map64_t *map, uint64_t key, uint64_t value) {
  uint64_t threshold = 0;

  if (TI_MAP_LOAD_FACTOR >= 100) {
    threshold = map->table_capacity;
  } else {
    threshold = (map->table_capacity * TI_MAP_LOAD_FACTOR) / 100;
  }

  if ((map->table_count + 1) > threshold) {
    table_expand(map);
  }

  uint64_t hash = mix64(key) % map->table_capacity;

  map64_record_t *curr = map->table[hash];

  while (curr) {

    if (curr->key == key) {
      return 0;
    }

    curr = curr->next;
  }

  curr = pool_alloc(map);

  curr->key = key;
  curr->value = value;
  curr->next = map->table[hash];

  map->table[hash] = curr;
  map->table_count++;

  return 1;
}
bool32_t map64_remove(map64_t *map, uint64_t key, uint64_t *value) {
  uint64_t hash = mix64(key) % map->table_capacity;

  map64_record_t *curr = map->table[hash];
  map64_record_t *prev = 0;

  while (curr) {

    if (curr->key == key) {

      if (prev) {
        prev->next = curr->next;
      } else {
        map->table[hash] = curr->next;
      }

      if (value) {
        *value = curr->value;
      }

      pool_free(map, curr);

      map->table_count--;

      return 1;
    }

    prev = curr;
    curr = curr->next;
  }

  return 0;
}
bool32_t map64_contains(map64_t *map, uint64_t key) {
  uint64_t hash = mix64(key) % map->table_capacity;

  map64_record_t *curr = map->table[hash];

  while (curr) {

    if (curr->key == key) {
      return 1;
    }

    curr = curr->next;
  }

  return 0;
}
uint64_t map64_count(map64_t *map) {
  return map->table_count;
}
uint64_t *map64_at(map64_t *map, uint64_t key) {
  uint64_t hash = mix64(key) % map->table_capacity;

  map64_record_t *curr = map->table[hash];

  while (curr) {

    if (curr->key == key) {
      return &curr->value;
    }

    curr = curr->next;
  }

  return 0;
}
map64_iter_t map64_iter(map64_t *map) {
  map64_iter_t it = {
    .table = map->table,
    .table_capacity = map->table_capacity,
    .first_step = 0,
  };

  uint64_t table_index = 0;
  uint64_t table_count = map->table_capacity;

  while (table_index < table_count) {

    map64_record_t *curr = map->table[table_index];

    if (curr) {

      it.table_index = table_index;
      it.table_record = curr;

      break;
    }

    table_index++;
  }

  return it;
}
void map64_clear(map64_t *map) {
  memset(map->table, 0, sizeof(map64_record_t *) * map->table_capacity);

  map->table_count = 0;
  map->pool_free = 0;
  map->pool_count = 0;

  map64_pool_chunk_t *chunk = map->pool_chunks;

  map64_record_t *curr = 0;

  uint64_t pool_index = 0;
  uint64_t pool_count = 0;

  while (chunk) {

    pool_index = 0;
    pool_count = chunk->capacity;

    while (pool_index < pool_count) {

      curr = &chunk->records[pool_index];
      curr->next = map->pool_free;

      map->pool_free = curr;

      pool_index++;
    }

    chunk = chunk->next;
  }
}
void map64_destroy(map64_t *map) {
  map64_pool_chunk_t *chunk = 0;
  map64_pool_chunk_t *next = 0;

  TI_FREE(map->table);

  chunk = map->pool_chunks;

  while (chunk) {

    next = chunk->next;

    TI_FREE(chunk->records);
    TI_FREE(chunk);

    chunk = next;
  }

  map->table = 0;
  map->table_capacity = 0;
  map->table_count = 0;
  map->pool = 0;
  map->pool_chunks = 0;
  map->pool_capacity = 0;
  map->pool_count = 0;
  map->pool_free = 0;
}

bool32_t map64_next(map64_iter_t *it) {
  if (it->first_step) {

    if (it->table_record) {
      it->table_record = it->table_record->next;
    }

    if (it->table_record == 0) {

      it->table_index++;

      while (it->table_index < it->table_capacity) {

        it->table_record = it->table[it->table_index];

        if (it->table_record) {
          break;
        }

        it->table_index++;
      }
    }
  } else {

    it->first_step = 1;
  }

  return (it->table_index < it->table_capacity) && it->table_record;
}

static bool32_t table_expand(map64_t *map) {
  uint64_t new_table_capacity = map->table_capacity * 2;
  uint64_t table_size = sizeof(map64_record_t *) * new_table_capacity;

  map64_record_t **new_table = (map64_record_t **)TI_ALLOC(table_size, 1, 0);

  memset(new_table, 0, table_size);

  map64_record_t *curr = 0;
  map64_record_t *next = 0;

  uint64_t hash = 0;

  uint64_t table_index = 0;
  uint64_t table_count = map->table_capacity;

  while (table_index < table_count) {

    curr = map->table[table_index];

    while (curr) {

      next = curr->next;

      hash = mix64(curr->key) % new_table_capacity;

      curr->next = new_table[hash];
      new_table[hash] = curr;

      curr = next;
    }

    table_index++;
  }

  TI_FREE(map->table);

  map->table = new_table;
  map->table_capacity = new_table_capacity;

  return 1;
}
static void pool_expand(map64_t *map) {
  map64_pool_chunk_t *chunk = (map64_pool_chunk_t *)TI_ALLOC(sizeof(map64_pool_chunk_t), 1, 0);

  chunk->next = map->pool_chunks;
  chunk->records = (map64_record_t *)TI_ALLOC(sizeof(map64_record_t) * TI_MAP_POOL_CAPACITY, 1, 0);
  chunk->capacity = TI_MAP_POOL_CAPACITY;

  map->pool_chunks = chunk;
  map->pool_capacity = map->pool_capacity + TI_MAP_POOL_CAPACITY;

  uint64_t chunk_index = 0;
  uint64_t chunk_count = TI_MAP_POOL_CAPACITY;

  while (chunk_index < chunk_count) {

    map64_record_t *curr = &chunk->records[chunk_index];

    curr->next = map->pool_free;
    map->pool_free = curr;

    chunk_index++;
  }
}

static map64_record_t *pool_alloc(map64_t *map) {
  if (map->pool_free == 0) {
    pool_expand(map);
  }

  map64_record_t *record = map->pool_free;

  map->pool_free = record->next;
  map->pool_count++;

  return record;
}
static void pool_free(map64_t *map, map64_record_t *record) {
  record->next = map->pool_free;

  map->pool_free = record;
  map->pool_count--;
}
