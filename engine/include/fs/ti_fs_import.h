#ifndef TI_FS_IMPORT_H
#define TI_FS_IMPORT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint8_t fs_import_model(char const *asset_file, char const *model_file);
uint8_t fs_import_pipeline(fs_pipeline_type_t pipeline_type, char const *asset_file, char const *vertex_file, char const *fragment_file);
uint8_t fs_import_font(char const *asset_file, char const *font_file);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_IMPORT_H
