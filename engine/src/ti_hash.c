#include <ti_hash.h>

uint64_t mix64(uint64_t value) {
  uint64_t hash = value;

  hash ^= hash >> 30;
  hash *= TI_MIX64_MUL_1;
  hash ^= hash >> 27;
  hash *= TI_MIX64_MUL_2;
  hash ^= hash >> 31;

  return hash;
}
uint64_t fnv1a64(uint64_t value, uint8_t *buffer, uint64_t size) {
  uint64_t hash = value ? value : TI_FNV1A64_OFFSET;
  uint64_t index = 0;

  while (index < size) {

    hash ^= buffer[index];
    hash *= TI_FNV1A64_PRIME;

    index++;
  }

  return hash;
}
