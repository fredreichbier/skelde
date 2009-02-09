#include <assert.h>
#include <stdio.h>

#include "mem.h"
#include "message.h"
#include "callable.h"
#include "vm.h"
#include "list.h"

void sk_callable_init(SkObject *self) {
    sk_object_set_data(self, sk_malloc(sizeof(SkCallableData)));
    self->clone_func = &sk_callable_clone;
}

SkObject *sk_callable_create_proto(SkVM *vm) {
    SkObject *self = sk_object_new(vm);
    sk_object_set_init_func(self, &sk_callable_init);
    sk_object_set_clone_func(self, &sk_callable_clone);

    SkObject *object;
    assert(sk_object_get_slot(vm->lobby, "Object", (void **)&object) == MAP_OK);

    sk_object_put_slot(self, "proto", object);
    return self;
}

void sk_callable_set_cfunction(SkObject *self, SkCFunction func) {
    sk_callable_get_data(self)->type = SK_CALLABLE_TYPE_CFUNCTION;
    sk_callable_get_data(self)->c_function = func;
}

SkObject *sk_callable_from_cfunction(SkVM *vm, SkCFunction func) {
    SkObject *self = sk_callable_clone(sk_vm_get_proto(vm, "Callable"));
    sk_callable_set_cfunction(self, func);
    return self;
}

SkObject *sk_callable_call(SkObject *self, SkObject *message) {
    SkObject *receiver = sk_message_get_receiver(message);
    SkObject *ctx = sk_object_clone(self);
    sk_object_set_slot(ctx, "message", message);
    sk_vm_callstack_push(SK_VM, ctx);
    /* evaluate ... */
    SkCallableData *data = sk_callable_get_data(self);
    switch(data->type) {
        case SK_CALLABLE_TYPE_BLOCK:
            sk_block_call(data->block, receiver, message);
            break;
        case SK_CALLABLE_TYPE_CFUNCTION:
            (data->c_function)(receiver, message);
            break;
        default:
            printf("Unknown callable type: %d\n", data->type);
    }
    /* cleanup */
    sk_vm_callstack_pop(SK_VM);
    return self; // TODO: return value
}

DEFINE_LAZY_CLONE_FUNC(sk_callable_clone); // isn't that evil?
