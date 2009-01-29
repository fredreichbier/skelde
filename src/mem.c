#include "mem.h"

inline void *sk_malloc(size_t size) {
    return GC_MALLOC(size);
}

inline void *sk_realloc(void *ptr, size_t size) {
    return GC_REALLOC(ptr, size);
}

inline void *sk_calloc (size_t nelem, size_t size) {
  void* p = sk_malloc (nelem * size);
  assert(p != NULL);
  memset (p, 0, nelem * size);
  return p;
}

inline void sk_free(void *ptr) {
    // that's a no-op here.
}
