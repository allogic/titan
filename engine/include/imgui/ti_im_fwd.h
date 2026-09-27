#ifndef TI_IM_FWD_H
#define TI_IM_FWD_H

typedef enum im_file_type_t {
  IM_FILE_TYPE_NONE = 0,
  IM_FILE_TYPE_ASSET,
  IM_FILE_TYPE_FOLDER,
  IM_FILE_TYPE_COUNT,
} im_file_type_t;

typedef enum im_sidebar_tab_t {
  IM_SIDEBAR_TAB_NONE = 0,
  IM_SIDEBAR_TAB_HIERARCHY,
  IM_SIDEBAR_TAB_FILESYSTEM,
  IM_SIDEBAR_TAB_HANDLE,
  IM_SIDEBAR_TAB_RENDERER,
  IM_SIDEBAR_TAB_COUNT,
} im_sidebar_tab_t;

typedef enum im_inspector_type_t {
  IM_INSPECTOR_TYPE_NONE = 0,
  IM_INSPECTOR_TYPE_ENTITY,
  IM_INSPECTOR_TYPE_ASSET,
  IM_INSPECTOR_TYPE_COUNT,
} im_inspector_type_t;

typedef struct im_file_entry_t {
  char path[TI_PATH_SIZE];
  char name[TI_PATH_SIZE];
  uint64_t size;
  uint64_t child_count;
  uint8_t is_directory;
  uint8_t is_symlink;
  uint8_t is_open;
  uint8_t should_open;
  struct im_file_entry_t *parent;
  struct im_file_entry_t *children;
} im_file_entry_t;

#endif // TI_IM_FWD_H
