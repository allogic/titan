#include <ti_pch.h>

void fsutil_load_text(uint8_t **buffer, uint64_t *buffer_size, char const *file_path) {
  FILE *file = 0;

  fopen_s(&file, file_path, "r");

  if (file) {

    fseek(file, 0, SEEK_END);

    (*buffer_size) = (uint64_t)ftell(file);
    (*buffer) = (uint8_t *)TI_ALLOC((*buffer_size), 0, 0);

    fseek(file, 0, SEEK_SET);
    uint64_t read_count = fread(*buffer, sizeof(uint8_t), *buffer_size, file);

    (*buffer)[read_count] = 0;

    fclose(file);
  }
}
void fsutil_load_binary(uint8_t **buffer, uint64_t *buffer_size, char const *file_path) {
  FILE *file = 0;

  fopen_s(&file, file_path, "rb");

  if (file) {

    fseek(file, 0, SEEK_END);

    (*buffer_size) = (uint64_t)ftell(file);
    (*buffer) = (uint8_t *)TI_ALLOC(*buffer_size, 0, 0);

    fseek(file, 0, SEEK_SET);
    fread(*buffer, sizeof(uint8_t), *buffer_size, file);

    fclose(file);
  }
}

void fsutil_save_text(uint8_t *buffer, uint64_t buffer_size, char const *file_path) {
  FILE *file = 0;

  fopen_s(&file, file_path, "w");

  if (file) {

    fseek(file, 0, SEEK_SET);
    fwrite(buffer, sizeof(uint8_t), buffer_size, file);

    fclose(file);
  }
}
void fsutil_save_binary(uint8_t *buffer, uint64_t buffer_size, char const *file_path) {
  FILE *file = 0;

  fopen_s(&file, file_path, "wb");

  if (file) {

    fseek(file, 0, SEEK_SET);
    fwrite(buffer, sizeof(uint8_t), buffer_size, file);

    fclose(file);
  }
}

fs_result fs_mkdir_recursive(fs *fs, const char *file_path, int32_t options) {
  static char buffer[TI_PATH_SIZE] = {0};

  uint64_t length = strlen(file_path);

  if (length >= TI_PATH_SIZE) {
    return FS_PATH_TOO_LONG;
  }

  memcpy(buffer, file_path, length + 1);

  char *p = buffer;

  while (*p++) {

    if (*p != '/') {
      continue;
    }

    *p = 0;

    if (fs_mkdir(fs, buffer, options) != FS_SUCCESS) {

      // return FS_ALREADY_EXISTS;
    }

    *p = '/';
  }

  if (fs_mkdir(fs, buffer, options) != FS_SUCCESS) {

    // return FS_ALREADY_EXISTS;
  }

  return FS_SUCCESS;
}
