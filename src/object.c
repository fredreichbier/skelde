#include <stdio.h>

#include "mem.h"
#include "hashmap.h"
#include "skstring.h"
#include "list.h"
#include "object.h"
#include "message.h"
#include "vm.h"
#include "number.h"

/* creates a new skelde object and returns it. */
SkObject *sk_object_new(SkVM *vm) {
    SkObject *obj = sk_malloc(sizeof(SkObject));
    obj->vm = vm;
    obj->slots = hashmap_new();
    sk_object_put_slot(obj, "proto", NULL); // TODO: nicer?
    obj->data = NULL;
    obj->init_func = NULL;
    obj->clone_func = &sk_object_clone_base;
    return obj;
}

SkObject *sk_object_clone_base(SkObject *self) {
    SkObject *other = sk_object_new(self->vm);
    sk_object_put_slot(other, "proto", self); // TODO: nicer?
    if(self->init_func) {
        (self->init_func)(other);
    }
    return other;
}

SkObject *sk_object_clone(SkObject *self) {
    return (self->clone_func)(self);
}

SkObject *sk_object_create_proto(SkVM *vm) {
    /* set methods */
    SkObject *self = sk_object_new(vm);
    sk_object_set_method(self, "set_slot", &sk_object__set_slot);
    sk_object_set_method(self, "get_slot", &sk_object__get_slot);
    return self;
}

SkObject *sk_object_get_slot_lazy(SkObject *self, const char *name) {
    SkObject *slot;
    assert(sk_object_get_slot(self, name, (void **)&slot) == MAP_OK);
    return slot;
}

SkObject *sk_object_dispatch_message(SkObject *self, SkObject *message) {
    // TODO: type checking
    bstring name = sk_string_get_bstring(sk_object_get_slot_lazy(message, "name"));
    SkObject *slot;
    int ret = sk_object_get_slot_bstring(self, name, (void **)&slot);
    if(ret == MAP_OK) {
        // we are having the slot. return it.
        SkObject *ret = slot;
        /* TODO: call */
        return ret;
    } else if(ret == MAP_MISSING) {
        // no slot. search recursively in the proto.
        SkObject *proto;
        assert(sk_object_get_slot(self, "proto", (void **)&proto) == MAP_OK);
        if(!proto) {
            // we have no proto. return nil. we're sorry.
            return SK_NIL;
        } else {
            return sk_object_dispatch_message(proto, message);
        }
    } else {
        // THAT WILL NEVER RETURN SOMETHING ELSE!
        assert(0);
    }
}

SkObject *sk_object__set_slot(SkObject *self, SkObject *message) {
    SkObject *arguments = sk_message_get_arguments(message);
    bstring name = sk_string_get_bstring(sk_list_get_at(arguments, 0));
    sk_object_set_slot_bstring(self, name, sk_list_get_at(arguments, 1));
    return self;
}

SkObject *sk_object__get_slot(SkObject *self, SkObject *message) {
    SkObject *arguments = sk_message_get_arguments(message);
    bstring name = sk_string_get_bstring(sk_list_get_at(arguments, 0));
    
    SkObject *slot;
    assert(sk_object_get_slot_bstring(self, name, (void **)&slot) == MAP_OK); // uh ... make that nicer
    return slot;
}
