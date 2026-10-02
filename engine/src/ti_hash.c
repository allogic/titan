#include <ti_hash.h>

uint64_t hash64(uint64_t v) {
  v ^= v >> 30;
  v *= 0xbf58476d1ce4e5b9ULL;
  v ^= v >> 27;
  v *= 0x94d049bb133111ebULL;
  v ^= v >> 31;

  return v;
}
