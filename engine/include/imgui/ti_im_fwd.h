#ifndef TI_IM_FWD_H
#define TI_IM_FWD_H

typedef enum sidebar_tab_t {
  SIDEBAR_TAB_NONE = 0,
  SIDEBAR_TAB_HIERARCHY,
  SIDEBAR_TAB_FILESYSTEM,
  SIDEBAR_TAB_HANDLE,
} sidebar_tab_t;

typedef enum inspector_type_t {
  INSPECTOR_TYPE_NONE = 0,
  INSPECTOR_TYPE_ENTITY,
  INSPECTOR_TYPE_FILE,
  INSPECTOR_TYPE_COUNT,
} inspector_type_t;

typedef struct fs_entry_t {
  char path[TI_PATH_SIZE];
  char name[TI_PATH_SIZE];
  uint64_t size;
  uint64_t child_count;
  uint8_t is_directory;
  uint8_t is_symlink;
  uint8_t is_open;
  struct fs_entry_t *children;
} fs_entry_t;

#endif // TI_IM_FWD_H
