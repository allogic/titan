#ifndef TI_ADB_ASSET_H
#define TI_ADB_ASSET_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void adb_asset_load(adb_asset_t *asset, fs_file *file);
void adb_asset_store(adb_asset_t *asset, fs_file *file);
void adb_asset_destroy(adb_asset_t *asset);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_ADB_ASSET_H
