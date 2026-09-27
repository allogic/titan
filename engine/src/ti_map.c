#include <ti_pch.h>

static void expand(map_t *map);
static uint64_t hash(map_t *map, void const *key, uint64_t key_size, uint64_t modulus);
static uint8_t load_factor(map_t *map);

void map_create(map_t *map) {
  map->table = (map_record_t **)TI_ALLOC(TI_MAP_TABLE_COUNT * sizeof(map_record_t *), 1, 0);
  map->table_size = TI_MAP_TABLE_COUNT * sizeof(map_record_t *);
  map->table_count = TI_MAP_TABLE_COUNT;
  map->record_count = 0;
}
uint8_t map_insert(map_t *map, void const *key, uint64_t key_size, void const *value, uint64_t value_size) {
  uint8_t key_exists = 0;
  uint8_t load = load_factor(map);

  if (load > TI_MAP_LOAD_FACTOR) {
    expand(map);
  }

  uint64_t h = hash(map, key, key_size, map->table_count);

  map_record_t *curr = map->table[h];

  while (curr) {

    if (memcmp(curr->key, key, TI_MIN(curr->key_size, key_size)) == 0) {

      key_exists = 1;

      break;
    }

    curr = curr->next;
  }

  if (key_exists == 0) {

    // TODO: map_record_t pool optimization..

    curr = (map_record_t *)TI_ALLOC(sizeof(map_record_t), 1, 0);

    curr->next = map->table[h];
    curr->key = (uint8_t *)TI_ALLOC(key_size, 0, key);
    curr->key_size = key_size;
    curr->value = (uint8_t *)TI_ALLOC(value_size, 0, value);
    curr->value_size = value_size;

    map->table[h] = curr;
    map->record_count++;
  }

  return key_exists;
}
uint8_t map_remove(map_t *map, void const *key, uint64_t key_size, void *value, uint64_t value_size) {
  uint64_t h = hash(map, key, key_size, map->table_count);

  map_record_t *curr = map->table[h];
  map_record_t *prev = 0;

  while (curr) {

    if (memcmp(curr->key, key, TI_MIN(curr->key_size, key_size)) == 0) {

      if (prev) {
        prev->next = curr->next;
      } else {
        map->table[h] = curr->next;
      }

      if (value) {
        memcpy(value, curr->value, TI_MIN(value_size, curr->value_size));
      }

      TI_FREE(curr->key);
      TI_FREE(curr->value);
      TI_FREE(curr);

      map->record_count--;

      return 1;
    }

    prev = curr;
    curr = curr->next;
  }

  return 0;
}
uint8_t map_contains(map_t *map, void const *key, uint64_t key_size) {
  uint64_t h = hash(map, key, key_size, map->table_count);

  map_record_t *curr = map->table[h];

  while (curr) {

    if (memcmp(curr->key, key, TI_MIN(curr->key_size, key_size)) == 0) {
      return 1;
    }

    curr = curr->next;
  }

  return 0;
}
uint64_t map_count(map_t *map) {
  return map->record_count;
}
void *map_at(map_t *map, void const *key, uint64_t key_size) {
  uint64_t h = hash(map, key, key_size, map->table_count);

  map_record_t *curr = map->table[h];

  while (curr) {

    if (memcmp(curr->key, key, TI_MIN(curr->key_size, key_size)) == 0) {
      return curr->value;
    }

    curr = curr->next;
  }

  return 0;
}
map_iter_t map_iter(map_t *map) {
  map_iter_t it = {
    .table = map->table,
    .table_count = map->table_count,
    .first_step = 0,
  };

  uint64_t table_index = 0;

  while (table_index < map->table_count) {

    map_record_t *curr = map->table[table_index];

    if (curr) {

      it.table_index = table_index;
      it.table_record = curr;

      break;
    }

    table_index++;
  }

  return it;
}
uint8_t map_next(map_iter_t *it) {
  if (it->first_step) {

    if (it->table_record) {
      it->table_record = it->table_record->next;
    }

    if (it->table_record == 0) {

      it->table_index++;

      while (it->table_index < it->table_count) {

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

  return (it->table_index < it->table_count) && it->table_record;
}
void *map_key(map_iter_t *it) {
  return it->table_record->key;
}
uint64_t map_key_size(map_iter_t *it) {
  return it->table_record->key_size;
}
void *map_value(map_iter_t *it) {
  return it->table_record->value;
}
uint64_t map_value_size(map_iter_t *it) {
  return it->table_record->value_size;
}
void map_clear(map_t *map) {
  uint64_t table_index = 0;

  while (table_index < map->table_count) {

    map_record_t *curr = map->table[table_index];

    while (curr) {

      map_record_t *tmp = curr;

      curr = curr->next;

      TI_FREE(tmp->key);
      TI_FREE(tmp->value);
      TI_FREE(tmp);
    }

    table_index++;
  }

  memset(map->table, 0, map->table_size);

  map->record_count = 0;
}
void map_destroy(map_t *map) {
  uint64_t table_index = 0;

  while (table_index < map->table_count) {

    map_record_t *curr = map->table[table_index];

    while (curr) {

      map_record_t *tmp = curr;

      curr = curr->next;

      TI_FREE(tmp->key);
      TI_FREE(tmp->value);
      TI_FREE(tmp);
    }

    table_index++;
  }

  TI_FREE(map->table);
}

static void expand(map_t *map) {
  uint64_t table_index = 0;
  uint64_t table_size = map->table_size * 2;
  uint64_t table_count = map->table_count * 2;

  map_record_t **table = (map_record_t **)TI_ALLOC(table_size, 1, 0);

  while (table_index < map->table_count) {

    map_record_t *curr = map->table[table_index];

    while (curr) {

      uint64_t h = hash(map, curr->key, curr->key_size, table_count);

      curr->next = table[h];
      table[h] = curr;
      curr = curr->next;
    }

    table_index++;
  }

  TI_FREE(map->table);

  map->table = table;
  map->table_size = table_size;
  map->table_count = table_count;
}
static uint64_t hash(map_t *map, void const *key, uint64_t key_size, uint64_t modulus) {
  uint64_t hash = TI_MAP_HASH_POLY;
  uint64_t key_index = 0;

  while (key_index < key_size) {

    hash = ((hash << 5) + hash) + *(((uint8_t *)key) + key_index);

    key_index++;
  }

  return hash % modulus;
}
static uint8_t load_factor(map_t *map) {
  return (uint8_t)(((map->record_count + 1) / map->table_count) * 100);
}
