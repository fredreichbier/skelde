#ifndef SKSTRING_H_INCLUDED
#define SKSTRING_H_INCLUDED

#include "bstrlib.h"

#include "object.h"
#include "vm.h"

void sk_string_init(SkObject *self);
SkObject *sk_string_create_proto(SkVM *vm);
void sk_string_set_bstring(SkObject *self, bstring string);
DECLARE_LAZY_CLONE_FUNC(sk_string_clone);

#define sk_string_get_bstring(obj) \
    ((SkObject*)(obj))->data

#endif
