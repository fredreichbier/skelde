#ifndef SKSTRING_H_INCLUDED
#define SKSTRING_H_INCLUDED

#include "bstrlib.h"

#include "object.h"
#include "vm.h"

void sk_string_init(SkObject *self);
SkObject *sk_string_create_proto(SkVM *vm);
void sk_string_set_bstring(SkObject *self, bstring string);
SkObject *sk_string_from_bstring(SkVM *vm, bstring string);
DECLARE_LAZY_CLONE_FUNC(sk_string_clone);

SkObject *sk_string__to_string(SkObject *slot, SkObject *self, SkObject *msg);

#define sk_string_get_bstring(obj) \
    ((SkObject*)(obj))->data

#define sk_string_as_charp(obj) \
    bstr2cstr(sk_string_get_bstring(obj), '\0')

#endif
