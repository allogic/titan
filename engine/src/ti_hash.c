#include <ti_hash.h>

uint64_t mix64(uint64_t v) {
  v ^= v >> 30;
  v *= 0xBf58476D1CE4E5B9ULL;
  v ^= v >> 27;
  v *= 0x94D049BB133111EBULL;
  v ^= v >> 31;
  return v;
}
uint64_t fnv1a64(char const *v) {
  uint64_t h = 0xCBF29CE484222325ULL;
  while (*v) {
    h ^= (uint8_t)*v++;
    h *= 0x00000100000001B3ULL;
  }
  return h;
}
