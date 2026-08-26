#ifndef TI_ADB_H
#define TI_ADB_H

#include <adb/ti_adb_primitive.h>
#include <adb/ti_adb_mesh.h>
#include <adb/ti_adb_model.h>
#include <adb/ti_adb_joint.h>
#include <adb/ti_adb_skin.h>
#include <adb/ti_adb_skeleton.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint8_t adb_import_model(char const *virtual_path, char const *model_file);
uint8_t adb_import_dflt_shader(char const *virtual_path, char const *vertex_file, char const *fragment_file);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_ADB_H
