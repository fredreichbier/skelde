#ifndef CALL_H_INCLUDED
#define CALL_H_INCLUDED

#include "object.h"
#include "vm.h"

void sk_call_init(SkObject *self);
SkObject *sk_call_create_proto(SkVM *vm);
DECLARE_LAZY_CLONE_FUNC(sk_call_clone);

#define sk_call_set_message(SELF, MSG) sk_object_set_slot(SELF, "message", MSG)

#endif
