#ifndef METHOD_H_INCLUDED
#define METHOD_H_INCLUDED

#include "object.h"
#include "vm.h"

void sk_method_init(SkObject *self);
void sk_method_init2(SkObject *self);
SkObject *sk_method_create_proto(SkVM *vm);
DECLARE_LAZY_CLONE_FUNC(sk_method_clone);
SkObject *sk_method_call(SkObject *self, SkObject *parentctx, SkObject *msg);

SkObject *sk_method__clone(SkObject *slot, SkObject *self, SkObject *msg);

#define sk_method_get_message(SELF) sk_object_get_slot_recursive(SELF, "message")
#define sk_method_set_message(SELF, VALUE) sk_object_set_slot(SELF, "message", VALUE)
#define sk_method_get_argument_names(SELF) sk_object_get_slot_recursive(SELF, "argument_names")
#define sk_method_set_argument_names(SELF, VALUE) sk_object_set_slot(SELF, "argument_names", VALUE)
#endif
