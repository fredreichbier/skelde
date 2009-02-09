#ifndef OBJLIST_H_INCLUDED
#define OBJLIST_H_INCLUDED

#include <assert.h>

#include "object.h"
#include "cvector.h"

inline SkObject *objlist_get_at(CVector *vec, size_t index) {
    SkObject *obj;
    assert(cvector_get_element(vec, &obj, index) == 0);
    return obj;
}

#endif
