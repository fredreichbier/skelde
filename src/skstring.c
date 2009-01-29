#include <assert.h>

#include "skstring.h"

SkObject *sk_string_clone(SkObject *self);

void sk_string_init(SkObject *self) {
    bstring data;
    sk_object_set_data(self, data);
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

DEFINE_LAZY_CLONE_FUNC(sk_string_clone);
