#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

#include "object.h"
#include "vm.h"

void sk_list_init(SkObject *self);
SkObject *sk_list_clone(SkObject *self);
SkObject *sk_list_create_proto(SkVM *vm);
void sk_list_append(SkObject *self, SkObject *item);
SkObject *sk_list_get_at(SkObject *self, int index);

#define sk_list_get_data(obj) \
    ((CVector *)sk_object_get_data(obj))
#define sk_list_size(obj) \
    cvector_size(sk_list_get_data(obj))

#endif
