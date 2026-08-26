#include <ti_pch.h>

fs *g_fs = 0;

int32_t main(int32_t argc, char **argv) {
  dmalloc_create();

  if (fs_init(0, &g_fs) != FS_SUCCESS) {
    return 1;
  }

  if (fs_mount(g_fs, ROOT_DIR "/asset", "asset", FS_READ | FS_WRITE) != FS_SUCCESS) {
    return 1;
  }

  __try {

    scene_create(&g_scene, "Test", ROOT_DIR "/asset/scene/test.dat");

    window_create(1920, 1080, "Titan");
    window_run();
    window_destroy();

    scene_destroy(&g_scene);

  } __except (EXCEPTION_EXECUTE_HANDLER) {

    printf("Something went wrong..\n"); // TODO
  }

  fs_uninit(g_fs);

  dmalloc_destroy();

  return 0;
}
