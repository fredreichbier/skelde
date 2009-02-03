#include <stdio.h>

#include "mem.h"
#include "hashmap.h"
#include "skstring.h"
#include "object.h"
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
    printf("CLONED ... %d %d\n", (int)self, (int)other);
    printf("The proto is %d ...", (int)sk_object_get_slot_lazy(self, "proto"));
    sk_object_put_slot(other, "proto", self); // TODO: nicer?
    printf("... and now %d!\n", (int)sk_object_get_slot_lazy(self, "proto"));
    if(self->init_func) {
        (self->init_func)(other);
    }
    other->init_func = self->init_func;
    return other;
}

SkObject *sk_object_clone(SkObject *self) {
    return (self->clone_func)(self);
}

SkObject *sk_object_create_proto(SkVM *vm) {
    return sk_object_new(vm);
}

SkObject *sk_object_get_slot_lazy(SkObject *self, const char *name) {
    SkObject *slot;
    assert(sk_object_get_slot(self, name, (void **)&slot) == MAP_OK);
    return slot;
}

SkObject *sk_object_dispatch_message(SkObject *self, SkObject *message) {
    // TODO: type checking
    bstring name = sk_string_get_bstring(sk_object_get_slot_lazy(message, "name"));
    printf("searching for %s in %d\n", bstr2cstr(name, '\0'), (int)self);
    SkObject *slot;
    int ret = sk_object_get_slot_bstring(self, name, (void **)&slot);
    if(ret == MAP_OK) {
        // we are having the slot. return it.
        printf("gotslot: %d\n", (int)slot);
        return slot; 
    } else if(ret == MAP_MISSING) {
        // no slot. search recursively in the proto.
        SkObject *proto;
        printf("prototototo: %d\n", (int)(proto));
        assert(sk_object_get_slot(self, "proto", (void **)&slot) == MAP_OK);
        if(!proto) {
            // we have no proto. return nil. we're sorry.
            printf("nilly\n");
            return SK_NIL;
        } else {
            return sk_object_dispatch_message(proto, message);
        }
    } else {
        // THAT WILL NEVER RETURN SOMETHING ELSE!
        assert(0);
    }
}
