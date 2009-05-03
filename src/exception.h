#ifndef EXCEPTION_H_INCLUDED
#define EXCEPTION_H_INCLUDED

#include "object.h"
#include "vm.h"

void sk_exception_init(SkObject *self);
SkObject *sk_exception_create_proto(SkVM *vm);
DECLARE_LAZY_CLONE_FUNC(sk_exception_clone);
void sk_exception_load_protos(SkObject *proto, SkObject *ctx);
SkObject *sk_exception_create_lazy(SkVM *vm, char *protoname, bstring message);

#endif
