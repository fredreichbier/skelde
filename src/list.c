#include <assert.h>

#include "cvector.h"
#include "object.h"
#include "list.h"
#include "exc.h"
#include "message.h"
#include "number.h"
#include "exception.h"

SkObject *sk_list_clone(SkObject *self);

void sk_list_init(SkObject *self) {
    CVector *data;
    cvector_create(
            &data,
            sizeof(SkObject *),
            10);
    sk_object_set_data(self, data);
}

SkObject *sk_list_create_proto(SkVM *vm) {
    SkObject *self = sk_object_clone(sk_vm_get_proto(vm, "Object"));
    sk_object_set_init_func(self, &sk_list_init);
    sk_object_set_clone_func(self, &sk_list_clone);
    /* methods */
    sk_object_bind_method(self, "get_at", &sk_list__get_at);
    sk_object_bind_method(self, "set_at", &sk_list__set_at);
    sk_object_bind_method(self, "append", &sk_list__append);
    return self;
}

void sk_list_append(SkObject *self, SkObject *item) {
    if(cvector_add_element(sk_object_get_data(self), &item) != 0) {
        abort();
    }
}

SkObject *sk_list_get_at(SkObject *self, int index) {
    SkObject *item;
    if(cvector_get_element(sk_list_get_data(self), &item, index) != 0) {
        sk_exc_raise(SK_VM, sk_exception_create_lazy(SK_VM,
                    "IndexError",
                    bformat("Couldn't access index %d.", index)));
    }
    return item;
}

void sk_list_set_at(SkObject *self, int index, SkObject *value) {
    if(cvector_set_element(sk_list_get_data(self), &value, index) != 0) {
        sk_exc_raise(SK_VM, sk_exception_create_lazy(SK_VM,
                    "IndexError",
                    bformat("Couldn't access index %d.", index)));
    }
}

SkObject *sk_list_create(SkVM *vm) {
    SkObject *self = sk_object_clone(sk_vm_get_proto(vm, "List"));
    return self;
}

SkObject *sk_list__get_at(SkObject *slot, SkObject *self, SkObject *msg) {
    sk_message_check_argcount(msg, "List get_at", 1);
    return sk_list_get_at(self,
            sk_number_get_int(sk_message_eval_arg_at(msg, 0)));
}

SkObject *sk_list__append(SkObject *slot, SkObject *self, SkObject *msg) {
    sk_message_check_argcount(msg, "List append", 1);
    sk_list_append(self, sk_message_eval_arg_at(msg, 0));
    return self;
}

SkObject *sk_list__set_at(SkObject *slot, SkObject *self, SkObject *msg) {
    sk_message_check_argcount(msg, "List set_at", 1);
    sk_list_set_at(self, sk_number_get_int(sk_message_eval_arg_at(msg, 0)),
            sk_message_eval_arg_at(msg, 1));
    return self;
}

DEFINE_LAZY_CLONE_FUNC(sk_list_clone);
