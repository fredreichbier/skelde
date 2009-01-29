#include <assert.h>

#include "message.h"

SkObject *sk_message_clone(SkObject *self);

/* a message has the following slots:
   * name
   * sender
 */

void sk_message_init(SkObject *self) {
    sk_object_set_clone_func(self, &sk_message_clone);
}

SkObject *sk_message_create_proto(SkVM* vm) {
    SkObject *self = sk_object_new(vm);
    sk_object_set_init_func(self, &sk_message_init);
    sk_object_set_clone_func(self, &sk_message_clone);

    SkObject *object;
    assert(sk_object_get_slot(vm->lobby, "Object", (void **)&object) == MAP_OK);

    sk_object_put_slot(self, "proto", object);
    return self;
}

DEFINE_LAZY_CLONE_FUNC(sk_message_clone);

