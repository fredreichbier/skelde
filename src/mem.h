#ifndef MEM_H_INCLUDED
#define MEM_H_INCLUDED

#include <assert.h>
#include <memory.h>
#include <stdlib.h>

#include <pthread.h>

static inline void *sk_malloc(size_t size) {
    return malloc(size);
}

static inline void *sk_realloc(void *ptr, size_t size) {
    return realloc(ptr, size);
}

static inline void *sk_calloc (size_t nelem, size_t size) {
  return calloc(nelem, size);
}

static inline void sk_free(void *ptr) {
    // that's a no-op here.
    free(ptr);
}

#endif
