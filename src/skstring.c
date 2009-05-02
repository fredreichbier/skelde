#include <assert.h>

#include "skstring.h"

void sk_string_init(SkObject *self) {
    bstring data;
    sk_object_set_data(self, data);
    sk_object_set_clone_func(self, &sk_string_clone);
}

SkObject *sk_string_create_proto(SkVM *vm) {
    SkObject *self = sk_object_new(vm);
    sk_object_set_init_func(self, &sk_string_init);
    sk_object_set_clone_func(self, &sk_string_clone);

    sk_object_set_proto(self, sk_vm_get_proto(vm, "Object"));
    
    /* methods */
    sk_object_bind_method(self, "to_string", &sk_string__to_string);

    return self;
}

void sk_string_set_bstring(SkObject *self, bstring string) {
    sk_object_set_data(self, string);
}

SkObject *sk_string_from_bstring(SkVM *vm, bstring string) {
    SkObject *self = sk_object_clone(sk_vm_get_proto(vm, "String"));
    sk_string_set_bstring(self, string);
    return self;
}

SkObject *sk_string__to_string(SkObject *slot, SkObject *self, SkObject *msg) {
    return self;
}

DEFINE_LAZY_CLONE_FUNC(sk_string_clone);
