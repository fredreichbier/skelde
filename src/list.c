#include <assert.h>

#include "cvector.h"
#include "object.h"
#include "list.h"

SkObject *sk_list_clone(SkObject *self);

void sk_list_init(SkObject *self) {
    CVector *data;
    cvector_create(
            &data,
            sizeof(SkObject *),
            10);
    sk_object_set_data(self, data);
    sk_object_set_clone_func(self, &sk_list_clone);
}

SkObject *sk_list_create_proto(SkVM *vm) {
    SkObject *self = sk_object_new(vm);
    sk_object_set_init_func(self, &sk_list_init);
    sk_object_set_clone_func(self, &sk_list_clone);

    SkObject *object;
    assert(sk_object_get_slot(vm->lobby, "Object", (void **)&object) == MAP_OK);

    sk_object_put_slot(self, "proto", object);
    return self;
}

void sk_list_append(SkObject *self, SkObject *item) {
    assert(cvector_add_element(sk_object_get_data(self), &item) == 0); 
}

SkObject *sk_list_get_at(SkObject *self, int index) {
    SkObject *item;
    assert(cvector_get_element(sk_object_get_data(self), &item, index) == 0);
    return item;
}

DEFINE_LAZY_CLONE_FUNC(sk_list_clone);
