#ifndef TI_ARCHIVE_H
#define TI_ARCHIVE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void archive_create(archive_t *archive);
uint8_t archive_insert(archive_t *archive, char const *name, void *buffer, uint64_t buffer_size);
uint8_t archive_remove(archive_t *archive, char const *name);
void archive_load(archive_t *archive);
void archive_store(archive_t *archive);
void archive_destroy(archive_t *archive);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //  TI_ARCHIVE_H
