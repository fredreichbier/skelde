#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

#include "object.h"
#include "vm.h"

void sk_list_init(SkObject *self);
SkObject *sk_list_clone(SkObject *self);
SkObject *sk_list_create_proto(SkVM *vm);
void sk_list_append(SkObject *self, SkObject *item);
SkObject *sk_list_get_at(SkObject *self, int index);
void sk_list_set_at(SkObject *self, int index, SkObject *value);
SkObject *sk_list_create(SkVM *vm);

SkObject *sk_list__get_at(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_list__append(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_list__set_at(SkObject *slot, SkObject *self, SkObject *msg);

#define sk_list_get_data(obj) \
    ((CVector *)sk_object_get_data(obj))
#define sk_list_size(obj) \
    cvector_size(sk_list_get_data(obj))

#endif
