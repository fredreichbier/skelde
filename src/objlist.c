#include "objlist.h"
#include "mem.h"

SkObject *objlist_get_at(CVector *vec, size_t index) {
    SkObject *obj;
    assert(cvector_get_element(vec, &obj, index) == 0);
    return obj;
}

CVector *cvector_copy(CVector *vec) {
    size_t array_size = vec->elementsize * vec->capacity;
    CVector *dup = sk_malloc(sizeof(CVector));
    memcpy(dup, vec, sizeof(CVector));
    /* copy `array` */
    dup->array = sk_malloc(array_size);
    memcpy(dup->array, vec->array, array_size);
    return dup;
}
