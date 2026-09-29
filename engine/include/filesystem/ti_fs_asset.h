#ifndef TI_FS_ASSET_H
#define TI_FS_ASSET_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fs_asset_create(fs_asset_t *asset);
uint8_t fs_asset_exists(fs_asset_t *asset);
void fs_asset_load(fs_asset_t *asset);
void fs_asset_store(fs_asset_t *asset);
void fs_asset_destroy(fs_asset_t *asset);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_ASSET_H
