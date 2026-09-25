#ifndef TI_FS_IMPORT_H
#define TI_FS_IMPORT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint8_t fs_import_model(fs_asset_t *asset, char const *model_file);
uint8_t fs_import_pipeline(fs_asset_t *asset, fs_pipeline_type_t pipeline_type, char const *vertex_file, char const *fragment_file);
uint8_t fs_import_font(fs_asset_t *asset, char const *font_file);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_IMPORT_H
