#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

static void on_create(void);
static void on_destroy(void);
static void on_update(float delta_time);

mod_instance_t reference = {
  .on_create = on_create,
  .on_destroy = on_destroy,
  .on_update = on_update,
};

static void on_create(void) {
}
static void on_destroy(void) {
}
static void on_update(float delta_time) {
}

#ifdef __cplusplus
}
#endif // __cplusplus

DECLARE_MOD(&reference);
