#ifndef TI_ADB_H
#define TI_ADB_H

#include <adb/ti_adb_primitive.h>
#include <adb/ti_adb_mesh.h>
#include <adb/ti_adb_model.h>
#include <adb/ti_adb_joint.h>
#include <adb/ti_adb_skin.h>
#include <adb/ti_adb_pipeline.h>
#include <adb/ti_adb_font.h>
#include <adb/ti_adb_asset.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern map_t g_assets;

void adb_create(void);
adb_asset_t *adb_asset(char const *asset_path);
void adb_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_ADB_H
