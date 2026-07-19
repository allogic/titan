#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <ti_mod.h>

void mod_load(mod_t *mod) {
  mod->module = LoadLibraryA(mod->file_path);

  mod->create_proc = (mod_create_proc_t)GetProcAddress(mod->module, "mod_create");
  mod->destroy_proc = (mod_destroy_proc_t)GetProcAddress(mod->module, "mod_destroy");

  mod->instance = mod->create_proc();
}
void mod_unload(mod_t *mod) {
  mod->destroy_proc(mod->instance);

  FreeLibrary(mod->module);
}
