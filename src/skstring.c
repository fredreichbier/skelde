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

    SkObject *object;
    sk_object_get_slot(vm->lobby, "Object", (void **)&object);
    sk_object_put_slot(self, "proto", object);

    return self;
}

void sk_string_set_bstring(SkObject *self, bstring string) {
    sk_object_set_data(self, string);
}

SkObject *sk_string_from_bstring(SkVM *vm, bstring string) {
    SkObject *self = sk_object_clone(sk_vm_get_proto(vm, "String"));
    printf("making '%s' (%d)...\n", bstr2cstr(string, '~'), string->slen);
    sk_string_set_bstring(self, string);
    return self;
}

DEFINE_LAZY_CLONE_FUNC(sk_string_clone);
