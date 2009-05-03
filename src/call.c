#include "call.h"

void sk_call_init(SkObject *self) {
    
}

SkObject *sk_call_create_proto(SkVM *vm) {
    SkObject *self = sk_object_clone(sk_vm_get_proto(vm, "Object"));
    sk_object_set_init_func(self, &sk_call_init);
    sk_object_set_clone_func(self, &sk_call_clone);
    sk_call_set_message(self, SK_NIL);
    return self;
}

DEFINE_LAZY_CLONE_FUNC(sk_call_clone);
