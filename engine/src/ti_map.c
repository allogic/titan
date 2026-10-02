#include <ti_map.h>

static void expand(map64_t *map);

void map64_create(map64_t *map) {
  map->table = (map64_record_t **)TI_ALLOC(TI_MAP_TABLE_COUNT * sizeof(map64_record_t *), 1, 0);
  map->table_size = TI_MAP_TABLE_COUNT * sizeof(map64_record_t *);
  map->table_count = TI_MAP_TABLE_COUNT;
  map->record_count = 0;
}
bool32_t map64_insert(map64_t *map, uint64_t key, uint64_t value) {
  uint64_t load_factor = ((map->record_count + 1) / map->table_count) * 100;

  if (load_factor > TI_MAP_LOAD_FACTOR) {
    expand(map);
  }

  uint64_t hash = hash64(key) % map->table_count;

  map64_record_t *curr = map->table[hash];

  while (curr) {

    if (curr->key == key) {
      return 0;
    }

    curr = curr->next;
  }

  curr = (map64_record_t *)TI_ALLOC(sizeof(map64_record_t), 1, 0);

  curr->next = map->table[hash];
  curr->key = key;
  curr->value = value;

  map->table[hash] = curr;
  map->record_count++;

  return 1;
}
bool32_t map64_remove(map64_t *map, uint64_t key, uint64_t *value) {
  uint64_t hash = hash64(key) % map->table_count;

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

      TI_FREE(curr);

      map->record_count--;

      return 1;
    }

    prev = curr;
    curr = curr->next;
  }

  return 0;
}
bool32_t map64_contains(map64_t *map, uint64_t key) {
  uint64_t hash = hash64(key) % map->table_count;

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
  return map->record_count;
}
uint64_t *map64_at(map64_t *map, uint64_t key) {
  uint64_t hash = hash64(key) % map->table_count;

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
    .table_count = map->table_count,
    .first_step = 0,
  };

  uint64_t table_index = 0;
  uint64_t table_count = map->table_count;

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
  uint64_t table_index = 0;
  uint64_t table_count = map->table_count;

  while (table_index < table_count) {

    map64_record_t *curr = map->table[table_index];

    while (curr) {

      map64_record_t *tmp = curr;

      curr = curr->next;

      TI_FREE(tmp);
    }

    table_index++;
  }

  memset(map->table, 0, map->table_size);

  map->record_count = 0;
}
void map64_destroy(map64_t *map) {
  uint64_t table_index = 0;
  uint64_t table_count = map->table_count;

  while (table_index < table_count) {

    map64_record_t *curr = map->table[table_index];

    while (curr) {

      map64_record_t *tmp = curr;

      curr = curr->next;

      TI_FREE(tmp);
    }

    table_index++;
  }

  TI_FREE(map->table);
}

bool32_t map64_next(map64_iter_t *it) {
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

static void expand(map64_t *map) {
  uint64_t table_index = 0;
  uint64_t table_count = map->table_count;

  uint64_t new_table_size = map->table_size * 2;
  uint64_t new_table_count = map->table_count * 2;

  map64_record_t **new_table = (map64_record_t **)TI_ALLOC(new_table_size, 1, 0);

  while (table_index < table_count) {

    map64_record_t *curr = map->table[table_index];
    map64_record_t *next = 0;

    while (curr) {

      uint64_t hash = hash64(curr->key) % new_table_count;

      next = curr->next;
      curr->next = new_table[hash];
      new_table[hash] = curr;
      curr = next;
    }

    table_index++;
  }

  TI_FREE(map->table);

  map->table = new_table;
  map->table_size = new_table_size;
  map->table_count = new_table_count;
}
