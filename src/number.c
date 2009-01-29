#include <assert.h>

#include "mem.h"
#include "hashmap.h"
#include "vm.h"
#include "number.h"

SkObject *sk_number_clone(SkObject *self);

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

    SkObject *object;
    assert(sk_object_get_slot(vm->lobby, "Object", (void **)&object) == MAP_OK);

    sk_object_put_slot(self, "proto", object);
    return self;
}

DEFINE_LAZY_CLONE_FUNC(sk_number_clone); // isn't that evil?

