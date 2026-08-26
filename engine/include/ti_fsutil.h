#ifndef TI_FSUTIL_H
#define TI_FSUTIL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fsutil_load_text(uint8_t **buffer, uint64_t *buffer_size, char const *file_path);
void fsutil_load_binary(uint8_t **buffer, uint64_t *buffer_size, char const *file_path);

void fsutil_save_text(uint8_t *buffer, uint64_t buffer_size, char const *file_path);
void fsutil_save_binary(uint8_t *buffer, uint64_t buffer_size, char const *file_path);

fs_result fs_mkdir_recursive(fs *fs, const char *file_path, int32_t options);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FSUTIL_H
