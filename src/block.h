#ifndef BLOCK_H_INCLUDED
#define BLOCK_H_INCLUDED

#include "object.h"
#include "ast.h"
#include "vm.h"

void sk_block_init(SkObject *self);
SkObject *sk_block_create_proto(SkVM *vm);
void sk_block_set_block(SkObject *self, SkNodeBlock *block);
SkObject *sk_block_call(SkObject *self, SkObject *receiver, SkObject *message);
DECLARE_LAZY_CLONE_FUNC(sk_block_clone);

#define sk_number_get_int(obj) \
    (*(int *)sk_object_get_data(obj))

#endif
