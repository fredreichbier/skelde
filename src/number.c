#include <assert.h>

#include "mem.h"
#include "vm.h"
#include "message.h"
#include "list.h"
#include "number.h"
#include "skstring.h"

void sk_number_init(SkObject *self) {
    int *data = sk_malloc(sizeof(int));
    *data = 5;
    sk_object_set_data(self, data);
    self->clone_func = &sk_number_clone;
}

SkObject *sk_number_create_proto(SkVM *vm) {
    SkObject *self = sk_object_new(vm);
    sk_object_set_init_func(self, &sk_number_init);
    sk_object_set_clone_func(self, &sk_number_clone);

    sk_object_put_slot(self, "proto", sk_vm_get_proto(vm, "Object"));

    /* methods */
    sk_object_bind_method(self, "to_string", &sk_number__to_string);
    sk_object_bind_method(self, "==", &sk_number__equals);
    sk_object_bind_method(self, "+", &sk_number__add);
    sk_object_bind_method(self, "times", &sk_number__times);

    return self;
}

void sk_number_set_int(SkObject *self, int value) {
    pthread_mutex_lock(&self->data_mutex);
    *(int *)self->data = value;
    pthread_mutex_unlock(&self->data_mutex);
}

SkObject *sk_number_create(SkVM *vm, int value) {
    SkObject *self = sk_object_clone(sk_vm_get_proto(vm, "Number"));
    sk_number_set_int(self, value);
    return self;
}

SkObject *sk_number__to_string(SkObject *slot, SkObject *self, SkObject *msg) {
    return sk_string_from_bstring(SK_VM, bformat("%d", sk_number_get_int(self)));
}

SkObject *sk_number__times(SkObject *slot, SkObject *self, SkObject *msg) {
    int i;
    _Bool break_ = 0;
    SkObject *result = SK_VM->nil;
    for(i = 0; i < sk_number_get_int(self); i++) {
        sk_exc_try(SK_VM) {
            result = sk_message_eval_arg_at(msg, 0);
        } sk_exc_except(SK_JUMP_CODE_BREAK) {
            /* break, but break after `sk_exc_end_try` was called,
             * otherwise we get an orphan jump context */
            break_ = 1;
        } sk_exc_except(SK_JUMP_CODE_CONTINUE) {
            /* do nothing ... */
        } sk_exc_else {
            /* pass the exception */
            sk_exc_pass(SK_VM);
        } sk_exc_end_try(SK_VM);
        if(break_) {
            return result;
        }
    }
    return result;
}

DEFINE_LAZY_CLONE_FUNC(sk_number_clone); // isn't that evil?

SkObject *sk_number__equals(SkObject *slot, SkObject *self, SkObject *msg) {
    return sk_vm_bool_to_skelde(SK_VM,
            sk_number_get_int(self) == sk_number_get_int(sk_message_eval_arg_at(msg, 0)));
}

SkObject *sk_number__add(SkObject *slot, SkObject *self, SkObject *msg) {
    return sk_number_create(SK_VM,
            sk_number_get_int(self) + sk_number_get_int(sk_message_eval_arg_at(msg, 0)));
}
