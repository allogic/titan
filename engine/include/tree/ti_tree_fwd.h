#ifndef TI_STD_FWD_H
#define TI_STD_FWD_H

typedef struct trie_node_t {
  void *value;
  struct trie_node_t *children[TI_TRIE_NODE_CHILD_COUNT];
} trie_node_t;

typedef void (*trie_store_proc_t)(FILE *file, trie_node_t *node);
typedef void (*trie_destroy_proc_t)(trie_node_t *node);

typedef struct trie_t {
  trie_node_t *node_pool;
  uint64_t alloc_nodes;
  uint64_t value_size;
  uint64_t pool_capacity;
  uint64_t node_count;
  trie_node_t *root_node;
} trie_t;

#endif // TI_STD_FWD_H
