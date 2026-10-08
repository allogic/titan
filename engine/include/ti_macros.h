#ifndef TI_MACROS_H
#define TI_MACROS_H

#define TI_STATIC_ASSERT(EXPRESSION) typedef uint8_t static_assert_t[(EXPRESSION) ? (1) : (-1)]

#define TI_ARRAY_COUNT(ARRAY) ((uint64_t)(sizeof(ARRAY) / sizeof((ARRAY)[0ULL])))
#define TI_OFFSET_OF(TYPE, MEMBER) ((uint64_t)&(((TYPE *)0)->MEMBER))

#define TI_MIN(A, B) (((A) < (B)) ? (A) : (B))
#define TI_MAX(A, B) (((A) > (B)) ? (A) : (B))
#define TI_CLAMP(A, LO, HI) ((A) < (LO) ? (LO) : ((A) > (HI) ? (HI) : (A)))

#define TI_ALIGN_DOWN_BY(VALUE, ALIGNMENT) (((uint64_t)(VALUE)) & ~(((uint64_t)(ALIGNMENT)) - 1ULL))
#define TI_ALIGN_UP_BY(VALUE, ALIGNMENT) ((((uint64_t)(VALUE)) + (((uint64_t)(ALIGNMENT)) - 1ULL)) & ~(((uint64_t)(ALIGNMENT)) - 1ULL))

#define TI_STRINGIFY_IMPL(V) #V
#define TI_STRINGIFY(V) TI_STRINGIFY_IMPL(V)

#ifdef COMPILER_MSVC
#  define TI_ALIGN_AS(N) __declspec(align(N))
#else
#  define TI_ALIGN_AS(N) _Alignas(N)
#endif // COMPILER_SELECTION

#ifdef COMPILER_MSVC
#  define TI_ALIGN_OF(T) __alignof(T)
#else
#  define TI_ALIGN_OF(T) _Alignof(T)
#endif // COMPILER_SELECTION

#define TI_JSON_ITEM(JSON, KEY) (cJSON_GetObjectItemCaseSensitive(JSON, KEY))
#define TI_JSON_INT(JSON, KEY) (cJSON_GetObjectItemCaseSensitive(JSON, KEY)->valueint)
#define TI_JSON_DOUBLE(JSON, KEY) (cJSON_GetObjectItemCaseSensitive(JSON, KEY)->valuedouble)
#define TI_JSON_STRING(JSON, KEY) (cJSON_GetObjectItemCaseSensitive(JSON, KEY)->valuestring)

#define TI_JSON_ARRAY_COUNT(JSON) (cJSON_GetArraySize(JSON))
#define TI_JSON_ARRAY_ITEM(JSON, INDEX) (cJSON_GetArrayItem(JSON, INDEX))
#define TI_JSON_ARRAY_INT(JSON, INDEX) (cJSON_GetArrayItem(JSON, INDEX)->valueint)
#define TI_JSON_ARRAY_DOUBLE(JSON, INDEX) (cJSON_GetArrayItem(JSON, INDEX)->valuedouble)
#define TI_JSON_ARRAY_STRING(JSON, INDEX) (cJSON_GetArrayItem(JSON, INDEX)->valuestring)

#endif // TI_MACROS_H
