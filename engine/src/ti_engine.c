#include <ti_pch.h>

// TODO: make adb_ to fs_
// TODO: make imgui_ to im_

int32_t main(int32_t argc, char **argv) {
  dmalloc_init();

  fs_setup();

  __try {

    adb_create();
    scene_create(&g_scene, "test", "asset/scene/test.bin");

    // TODO: move this somewhere else..
    import_font("asset/font/commit_mono_latin_400_normal.pak", "static/font/commit_mono_latin_400_normal.ttf");
    import_font("asset/font/material_symbols_rounded_fill.pak", "static/font/material_symbols_rounded_fill.ttf");

    window_create(1920, 1080, "Titan");
    window_run();
    window_destroy();

    scene_destroy(&g_scene);
    adb_destroy();

  } __except (EXCEPTION_EXECUTE_HANDLER) {

    printf("Something went wrong..\n"); // TODO
  }

  fs_close();

  dmalloc_cleanup();

  return 0;
}
