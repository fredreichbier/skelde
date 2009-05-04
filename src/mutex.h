#ifndef MUTEX_H_INCLUDED
#define MUTEX_H_INCLUDED

#include "object.h"
#include "vm.h"

void sk_mutex_init(SkObject *self);
SkObject *sk_mutex_create_proto(SkVM *vm);
DECLARE_LAZY_CLONE_FUNC(sk_mutex_clone);

SkObject *sk_mutex__lock(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_mutex__unlock(SkObject *slot, SkObject *self, SkObject *msg);

#endif
