#include <ti_pch.h>

map_t g_assets = {0};

void adb_create(void) {
  map_create(&g_assets);
}
adb_asset_t *adb_asset(char const *asset_path) {
  uint64_t asset_path_size = strlen(asset_path);

  if (map_contains(&g_assets, asset_path, asset_path_size) == 0) {

    fs_file *file = 0;

    if (fs_file_open(g_fs, asset_path, FS_READ, &file) == FS_SUCCESS) {

      adb_asset_t asset = {0};

      adb_asset_load(&asset, file);

      map_insert(&g_assets, asset_path, asset_path_size, &asset, sizeof(adb_asset_t));

      fs_file_close(file);
    }
  }

  return map_at(&g_assets, asset_path, asset_path_size);
}
void adb_destroy(void) {
  map_iter_t asset_it = map_iter(&g_assets);

  while (map_next(&asset_it)) {

    adb_asset_t *asset = (adb_asset_t *)map_value(&asset_it);

    adb_asset_destroy(asset);
  }

  map_destroy(&g_assets);
}
