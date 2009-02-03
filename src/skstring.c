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
    assert(sk_object_get_slot(vm->lobby, "Object", (void **)&object) == MAP_OK);

    sk_object_put_slot(self, "proto", object);
    return self;
}

void sk_string_set_bstring(SkObject *self, bstring string) {
    sk_object_set_data(self, string);
}

DEFINE_LAZY_CLONE_FUNC(sk_string_clone);
