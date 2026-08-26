#ifndef TI_TREE_TRIE_H
#define TI_TREE_TRIE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void trie_create(trie_t *trie, uint64_t value_size);
void trie_expand(trie_t *trie);
void *trie_at(trie_t *trie, const char *path);
trie_node_t *trie_root(trie_t *trie);
uint64_t trie_count(trie_t *trie);
void trie_store(trie_t *trie, trie_store_proc_t store_proc, FILE *file);
void trie_destroy(trie_t *trie, trie_destroy_proc_t destroy_proc);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_TREE_TRIE_H
