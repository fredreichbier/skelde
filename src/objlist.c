#include "objlist.h"

SkObject *objlist_get_at(CVector *vec, size_t index) {
    SkObject *obj;
    assert(cvector_get_element(vec, &obj, index) == 0);
    return obj;
}
