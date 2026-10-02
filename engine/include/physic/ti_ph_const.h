#ifndef TI_PH_CONST_H
#define TI_PH_CONST_H

#define TI_PHYSIC_TIME_STEP (1.0F / 60.0F)
#define TI_PHYSIC_DEMO_ITEM_LIMIT (32U)

typedef enum ti_physic_layer_t {
  TI_PHYSIC_LAYER_STATIC,
  TI_PHYSIC_LAYER_DYNAMIC,
  TI_PHYSIC_LAYER_COUNT,
} ti_physic_layer_t;

#endif // TI_PH_CONST_H
