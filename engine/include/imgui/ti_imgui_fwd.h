#ifndef TI_IMGUI_FWD_H
#define TI_IMGUI_FWD_H

typedef enum inspector_type_t {
  INSPECTOR_TYPE_NONE = 0,
  INSPECTOR_TYPE_FILE,
  INSPECTOR_TYPE_ENTITY,
  INSPECTOR_TYPE_COUNT,
} inspector_type_t;

typedef struct fs_entry {
  char name[TI_PATH_SIZE];
  uint8_t is_directory;
  uint64_t size;
} fs_entry;

#endif // TI_IMGUI_FWD_H
