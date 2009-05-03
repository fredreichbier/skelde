#include "exception.h"
#include "skstring.h"

void sk_exception_init(SkObject *self) {

}

SkObject *sk_exception_create_proto(SkVM *vm) {
    SkObject *self = sk_object_clone(sk_vm_get_proto(vm, "Object"));
    sk_object_set_clone_func(self, &sk_exception_clone);
    sk_object_set_init_func(self, &sk_exception_init);
    sk_object_set_slot(self, "message",
            sk_string_from_bstring(vm, bfromcstr("No Message.")));
    return self;
}

void sk_exception_load_protos(SkObject *proto, SkObject *ctx) {
    SkObject *type_error = sk_object_clone(proto);
    sk_object_set_slot(ctx, "TypeError", type_error);

    sk_object_set_slot(ctx, "ArgumentError", sk_object_clone(proto));
    sk_object_set_slot(ctx, "SlotError", sk_object_clone(proto));
}

SkObject *sk_exception_create_lazy(SkVM *vm, char *protoname, bstring message) {
    SkObject *self = sk_object_clone(sk_vm_get_proto(vm, protoname));
    sk_object_set_slot(self, "message", sk_string_from_bstring(vm, message));
    return self;
}

DEFINE_LAZY_CLONE_FUNC(sk_exception_clone);
