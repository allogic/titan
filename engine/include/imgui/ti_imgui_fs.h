#ifndef TI_IMGUI_FS_H
#define TI_IMGUI_FS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern char g_curr_dir[TI_PATH_SIZE];

extern uint64_t g_selected_file;

void fs_draw(void);
void fs_reset(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_IMGUI_FS_H
