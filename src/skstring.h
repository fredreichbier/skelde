#ifndef SKSTRING_H_INCLUDED
#define SKSTRING_H_INCLUDED

#include "bstrlib.h"

#include "object.h"
#include "vm.h"

SkObject *sk_string_clone(SkObject *self);
void sk_string_init(SkObject *self);

#define sk_string_get_bstring(obj) \
    ((SkObject*)(obj))->data

#endif
