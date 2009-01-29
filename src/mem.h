#ifndef MEM_H_INCLUDED
#define MEM_H_INCLUDED

#include <assert.h>
#include <memory.h>

#include "gc.h"

static inline void *sk_malloc(size_t size) {
    return GC_MALLOC(size);
}

static inline void *sk_realloc(void *ptr, size_t size) {
    return GC_REALLOC(ptr, size);
}

static inline void *sk_calloc (size_t nelem, size_t size) {
  void* p = sk_malloc (nelem * size);
  assert(p != NULL);
  memset (p, 0, nelem * size);
  return p;
}

static inline void sk_free(void *ptr) {
    // that's a no-op here.
}

#endif
