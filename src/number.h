#ifndef NUMBER_H_INCLUDED
#define NUMBER_H_INCLUDED

#include "object.h"
#include "vm.h"

void sk_number_init(SkObject *self);
SkObject *sk_number_create_proto(SkVM *vm);
void sk_number_set_int(SkObject *self, int value);
DECLARE_LAZY_CLONE_FUNC(sk_number_clone);

#define sk_number_get_int(obj) \
    (*(int *)sk_object_get_data(obj))

#endif
