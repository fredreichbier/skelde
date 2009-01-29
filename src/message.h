#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include "vm.h"
#include "object.h"
#include "skstring.h"

void sk_message_init(SkObject *self);
SkObject *sk_message_clone(SkObject *self);
SkObject *sk_message_create_proto(SkVM *vm);

#endif
