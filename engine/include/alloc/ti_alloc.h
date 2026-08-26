#ifndef TI_ALLOC_H
#define TI_ALLOC_H

#ifdef BUILD_DEBUG
#  define TI_ALLOC(SIZE, ZERO, REF) dmalloc_alloc(__FILE__, __func__, __LINE__, SIZE, ZERO, REF)
#  define TI_FREE(DATA) dmalloc_free(__FILE__, __func__, __LINE__, DATA)
#else
#  define TI_ALLOC(SIZE, ZERO, REF)
#  define TI_FREE(DATA)
#endif // BUILD_DEBUG

#include <alloc/ti_alloc_debug.h>

#endif // TI_ALLOC_H
