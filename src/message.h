#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include "vm.h"
#include "object.h"
#include "skstring.h"

void sk_message_init(SkObject *self);
SkObject *sk_message_clone(SkObject *self);
SkObject *sk_message_create_proto(SkVM *vm);
DECLARE_LAZY_CLONE_FUNC(sk_message_clone);

#define sk_message_set_name(self, _name) \
    sk_object_put_slot(self, "name", _name)
#define sk_message_set_receiver(self, _receiver) \
    sk_object_put_slot(self, "receiver", _receiver)
#define sk_message_set_arguments(self, _arguments) \
    sk_object_put_slot(self, "arguments", _arguments)
#define sk_message_get_receiver(self) \
    sk_object_get_slot_lazy(self, "receiver")
#define sk_message_get_name(self) \
    sk_object_get_slot_lazy(self, "name")
#define sk_message_get_arguments(self) \
    sk_object_get_slot_lazy(self, "arguments")

#endif
