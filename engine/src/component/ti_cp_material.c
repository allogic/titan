#include <component/ti_cp_material.h>

void cp_material_init(cp_material_t *material) {
  memset(material, 0, sizeof(cp_material_t));
}
