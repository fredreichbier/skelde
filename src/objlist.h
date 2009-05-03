#ifndef OBJLIST_H_INCLUDED
#define OBJLIST_H_INCLUDED

#include <assert.h>

#include "object.h"
#include "cvector.h"

SkObject *objlist_get_at(CVector *vec, size_t index);
CVector *cvector_copy(CVector *vec);
#define objlist_top(VEC) objlist_get_at(VEC, cvector_size(VEC) - 1)

#endif
