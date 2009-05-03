#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include "vm.h"
#include "object.h"
#include "skstring.h"

void sk_message_init(SkObject *self);
void sk_message_init2(SkObject *self);
SkObject *sk_message_clone(SkObject *self);
SkObject *sk_message_create_proto(SkVM *vm);
SkObject *sk_message_start_dispatch(SkObject *self);
SkObject *sk_message_dispatch_simple(SkObject *self);
SkObject *sk_message_dispatch_avalanche(SkObject *self);
SkObject *sk_message_create_simple(SkVM *vm, char *name);
DECLARE_LAZY_CLONE_FUNC(sk_message_clone);

#define sk_message_set_name(self, _name) \
    sk_object_put_slot(self, "name", _name)
#define sk_message_set_arguments(self, _arguments) \
    sk_object_put_slot(self, "arguments", _arguments)
#define sk_message_get_name(self) \
    sk_object_get_slot_lazy(self, "name")
#define sk_message_get_arguments(self) \
    sk_object_get_slot_lazy(self, "arguments")
#define sk_message_set_next(self, _next) \
    sk_object_put_slot(self, "next", _next)
#define sk_message_get_next(self) \
    sk_object_get_slot_lazy(self, "next")
#define sk_message_set_previous(self, _previous) \
    sk_object_put_slot(self, "previous", _previous)
#define sk_message_get_previous(self) \
    sk_object_get_slot_lazy(self, "previous")
#define sk_message_append_argument(self, ARG) \
    sk_list_append(sk_message_get_arguments(self), ARG)
#define sk_message_arg_at(self, idx) \
    sk_list_get_at(sk_message_get_arguments(self), idx)
#define sk_message_eval_arg_at(self, idx) \
    sk_message_dispatch_avalanche(sk_message_arg_at(self, idx))
#define sk_message_argcount(self) \
    sk_list_size(sk_message_get_arguments(self))

#endif
