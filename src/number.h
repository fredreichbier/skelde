#ifndef NUMBER_H_INCLUDED
#define NUMBER_H_INCLUDED

#include "object.h"
#include "vm.h"

void sk_number_init(SkObject *self);
SkObject *sk_number_create_proto(SkVM *vm);
void sk_number_set_int(SkObject *self, int value);
DECLARE_LAZY_CLONE_FUNC(sk_number_clone);
SkObject *sk_number_create(SkVM *vm, int value);

SkObject *sk_number__to_string(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_number__times(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_number__equals(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_number__add(SkObject *slot, SkObject *self, SkObject *msg);

#define sk_number_get_int(obj) \
    (*(int *)sk_object_get_data(obj))

#endif
