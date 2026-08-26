#include <ti_pch.h>

static void *trie_node_insert(trie_t *trie, trie_node_t *node, char const *path);
static void trie_node_store(trie_t *trie, trie_node_t *node, uint64_t prev_index, trie_store_proc_t store_proc, FILE *file);
static void trie_node_destroy(trie_t *trie, trie_node_t *node, trie_destroy_proc_t destroy_proc);

void trie_create(trie_t *trie, uint64_t value_size) {
  uint64_t pool_capacity = TI_ALIGN_UP_BY(TI_TRIE_NODE_POOL_CAPACITY, TI_TRIE_NODE_POOL_ALIGNMENT);

  trie->node_pool = (trie_node_t *)TI_ALLOC(pool_capacity * sizeof(trie_node_t), 1, 0);
  trie->value_size = value_size;
  trie->pool_capacity = pool_capacity;
  trie->node_count = 1;
  trie->root_node = trie->node_pool;
}
void trie_expand(trie_t *trie) {
  uint64_t pool_capacity = TI_ALIGN_UP_BY(trie->node_count, TI_TRIE_NODE_POOL_ALIGNMENT) * 2;

  trie_node_t *node_pool = (trie_node_t *)TI_ALLOC(pool_capacity * sizeof(trie_node_t), 1, 0);

  memcpy(node_pool, trie->node_pool, trie->node_count * sizeof(trie_node_t));

  TI_FREE(trie->node_pool);

  trie->node_pool = node_pool;
  trie->pool_capacity = pool_capacity;
  trie->root_node = node_pool;
}
void *trie_at(trie_t *trie, const char *path) {
  return trie_node_insert(trie, trie->root_node, path);
}
trie_node_t *trie_root(trie_t *trie) {
  return trie->root_node;
}
uint64_t trie_count(trie_t *trie) {
  return trie->alloc_nodes;
}
void trie_store(trie_t *trie, trie_store_proc_t store_proc, FILE *file) {
  trie_node_store(trie, trie->root_node, 0, store_proc, file);
}
void trie_destroy(trie_t *trie, trie_destroy_proc_t destroy_proc) {
  trie_node_destroy(trie, trie->root_node, destroy_proc);

  TI_FREE(trie->node_pool);
}

static void *trie_node_insert(trie_t *trie, trie_node_t *node, char const *path) {
  if (*path == 0) {

    if (node->value == 0) {

      node->value = TI_ALLOC(trie->value_size, 1, 0);

      trie->alloc_nodes++;
    }

    return node->value;
  }

  if (node->children[*path] == 0) {
    node->children[*path] = &trie->node_pool[trie->node_count++];
  }

  if (trie->node_count >= trie->pool_capacity) {
    trie_expand(trie);
  }

  return trie_node_insert(trie, node->children[*path], path + 1);
}
static void trie_node_store(trie_t *trie, trie_node_t *node, uint64_t prev_index, trie_store_proc_t store_proc, FILE *file) {
  static char file_path[TI_PATH_SIZE] = {0};
  static char file_name[TI_PATH_SIZE] = {0};

  static uint64_t file_path_index = 0;

  uint64_t child_index = 0;
  uint64_t child_count = TI_TRIE_NODE_CHILD_COUNT;

  while (child_index < child_count) {

    trie_node_t *child = node->children[child_index];

    if (child) {

      file_path[file_path_index] = (char)child_index;
      file_path_index++;

      if ((child_index == '/') || child->value) {

        memset(file_name, 0, TI_PATH_SIZE);
        memcpy(file_name, file_path + prev_index, file_path_index - 1);

        if (child->value) {
          store_proc(file, child);
        }

        trie_node_store(trie, child, file_path_index, store_proc, file);

      } else {

        trie_node_store(trie, child, prev_index, store_proc, file);
      }

      file_path_index--;
      file_path[file_path_index] = 0;
    }

    child_index++;
  }
}
static void trie_node_destroy(trie_t *trie, trie_node_t *node, trie_destroy_proc_t destroy_proc) {
  if (node == 0) {
    return;
  }

  if (node->value) {

    destroy_proc(node);

    TI_FREE(node->value);
  }

  uint64_t child_index = 0;
  uint64_t child_count = TI_TRIE_NODE_CHILD_COUNT;

  while (child_index < child_count) {

    trie_node_destroy(trie, node->children[child_index], destroy_proc);

    child_index++;
  }
}
