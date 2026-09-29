#include <ti_pch.h>

fs *g_fs = 0;

fs_result fs_create(char const *static_path, char const *asset_path) {
  fs_result result = FS_SUCCESS;

  result = fs_init(0, &g_fs);

  if (result != FS_SUCCESS) {
    return result;
  }

  result = fs_mount(g_fs, static_path, "static", FS_READ);

  if (result != FS_SUCCESS) {

    fs_uninit(g_fs);

    return result;
  }

  result = fs_mount(g_fs, asset_path, "asset", FS_READ | FS_WRITE);

  if (result != FS_SUCCESS) {

    fs_uninit(g_fs);

    return result;
  }

  return result;
}
fs_result fs_mkdir_recursive(fs *fs, const char *file_path, int32_t options) {
  fs_result result = FS_SUCCESS;
  fs_path_iterator path_it = {0};

  uint64_t path_size = strlen(file_path);

  char buffer[TI_PATH_SIZE] = {0};

  if (path_size >= TI_PATH_SIZE) {
    return FS_PATH_TOO_LONG;
  }

  result = fs_path_first(file_path, path_size, &path_it);

  if (result != FS_SUCCESS) {
    return result;
  }

  while (result == FS_SUCCESS) {

    uint64_t end = path_it.segmentOffset + path_it.segmentLength;

    memcpy(buffer, file_path, end);
    buffer[end] = '\0';

    result = fs_mkdir(fs, buffer, options);
    result = fs_path_next(&path_it);
  }

  if (result == FS_AT_END) {
    result = FS_SUCCESS;
  }

  return result;
}
fs_result fs_remove_recursive(fs *fs, char const *file_path) {
  fs_result result = FS_SUCCESS;
  fs_file_info info;

  char buffer[TI_PATH_SIZE] = {0};

  result = fs_info(g_fs, file_path, 0, &info);

  if (result != FS_SUCCESS) {
    return result;
  }

  if (info.directory == 0) {
    return fs_remove(g_fs, file_path, 0);
  }

  fs_iterator *dir_it = fs_first(g_fs, file_path, 0);

  while (dir_it) {

    snprintf(buffer, TI_PATH_SIZE, "%s/%s", file_path, dir_it->pName);

    result = fs_remove_recursive(g_fs, buffer);

    if (result != FS_SUCCESS) {

      fs_free_iterator(dir_it);

      return result;
    }

    dir_it = fs_next(dir_it);
  }

  return fs_remove(g_fs, file_path, 0);
}
fs_result fs_path_parent(char const *file_path, char *parent_path) {
  fs_result result = FS_SUCCESS;
  fs_path_iterator path_it = {0};

  uint64_t path_size = strlen(file_path);

  result = fs_path_last(file_path, path_size, &path_it);

  if (result != FS_SUCCESS) {
    return result;
  }

  result = fs_path_prev(&path_it);

  if (result != FS_SUCCESS) {
    return result;
  }

  uint64_t end = path_it.segmentOffset + path_it.segmentLength;

  memcpy(parent_path, file_path, end);

  return result;
}
void fs_destroy(void) {
  fs_uninit(g_fs);
}
