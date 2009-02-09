#include <assert.h>
#include <stdio.h>

#include "block.h"

void sk_block_init(SkObject *self) {
    sk_object_set_data(self, NULL);
    self->clone_func = &sk_block_clone;
}

SkObject *sk_block_create_proto(SkVM *vm) {
    SkObject *self = sk_object_new(vm);
    sk_object_set_init_func(self, &sk_block_init);
    sk_object_set_clone_func(self, &sk_block_clone);

    SkObject *object;
    assert(sk_object_get_slot(vm->lobby, "Object", (void **)&object) == MAP_OK);

    sk_object_put_slot(self, "proto", object);
    return self;
}

void sk_block_set_block(SkObject *self, SkNodeBlock *block) {
    self->data = (void *)block;
}

SkObject *sk_block_call(SkObject *self, SkObject *receiver, SkObject *message) {
    printf("calling\n");
    return NULL;
}

DEFINE_LAZY_CLONE_FUNC(sk_block_clone); // isn't that evil?
