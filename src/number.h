#ifndef NUMBER_H_INCLUDED
#define NUMBER_H_INCLUDED

#include "object.h"

void sk_number_init(SkObject *self);
SkObject *sk_number_create_proto(SkVM *vm);

#define sk_number_get_int(obj) \
    (*(int *)sk_object_get_data(obj))

#endif
