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
    obj->call_func = NULL;
    obj->dispatch_func = &sk_object_dispatch_message_simple;
    return obj;
}

SkObject *sk_object_clone_base(SkObject *self) {
    SkObject *other = sk_object_new(self->vm);
    sk_object_put_slot(other, "proto", self); // TODO: nicer?
    other->call_func = NULL;
    other->dispatch_func = &sk_object_dispatch_message_simple;
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
    sk_object_bind_method(self, "to_bool", &sk_object__to_bool);
    sk_object_bind_method(self, "to_repr", &sk_object__to_repr);
    sk_object_bind_method(self, "print", &sk_object__print);
    sk_object_bind_method(self, "println", &sk_object__println);
    sk_object_bind_method(self, "set_slot", &sk_object__set_slot);
    sk_object_bind_method(self, "get_slot", &sk_object__get_slot);
    return self;
}

SkObject *sk_object_get_slot_lazy(SkObject *self, const char *name) {
    SkObject *slot;
    if(sk_object_get_slot(self, name, (void **)&slot) != MAP_OK) {
        printf("Couldn't find slot '%s'.\n", name);
        abort();
    }
    return slot;
}

SkObject *sk_object_call(SkObject *self, SkObject *ctx, SkObject *message) {
    return (self->call_func)(self, ctx, message);
}

SkObject *sk_callable_create(SkVM *vm, SkCallFunction func) {
    SkObject *self = sk_object_new(vm);
    sk_object_set_activatable(self, TRUE);
    sk_object_set_call_func(self, func);
    return self;
}

void sk_object_bind_method(SkObject *self, char *name, SkCallFunction func) {
    sk_object_set_slot(self, name, sk_callable_create(self->vm, func));
}

bstring sk_object_to_repr_simple(SkObject *self) {
    return bformat("<Object at 0x%x>", (unsigned int)self);
}

bstring sk_object_to_repr(SkObject *self) {
    return sk_string_get_bstring(
            sk_object_send_message_simple(self,
                    sk_message_create_simple(
                        SK_VM,
                        "to_repr"
                        )
                )
            );
}

_Bool sk_object_to_bool(SkObject *self) {
    return sk_vm_skelde_to_bool(
            SK_VM,
            sk_object_send_message_simple(self,
                    sk_message_create_simple(
                        SK_VM,
                        "to_bool"
                        )
                )
            );
}

SkObject *sk_object_send_message_simple(SkObject *self, SkObject *msg) {
    sk_vm_callstack_push(SK_VM, self);
    SkObject *result = sk_message_dispatch_simple(msg);
    sk_vm_callstack_pop(SK_VM);
    return result;
}

_Bool sk_object_get_activatable(SkObject *self) {
    /* has to be either `true` or `false`, otherwise we get into a nice
       infinite recursion here, because we call toBool, and it queries
       getActivatable, and it asks toBool, ... */
    SkObject *obj = self;
    /* This will never fail, because the `Object` proto has "activatable" set. */
    for(;;) {
        if(sk_object_has_slot(obj, "activatable")) {
            return sk_vm_skelde_to_bool(SK_VM, sk_object_get_slot_lazy(obj, "activatable"));
        }
        obj = sk_object_get_proto(obj);
    }
}

SkObject *sk_object_dispatch_message(SkObject *self, SkObject *msg) {
    return (self->dispatch_func)(self, self, msg);
}

SkObject *sk_object_dispatch_message_simple(SkObject *self, SkObject *ctx, SkObject *message) {
    // TODO: type checking
    bstring name = sk_string_get_bstring(sk_object_get_slot_lazy(message, "name"));
    SkObject *slot;
    int ret = sk_object_get_slot_bstring(self, name, (void **)&slot);
    if(ret == MAP_OK) {
        /* if activatable: call it. */
        if(sk_object_get_activatable(slot)) {
            slot = sk_object_call(slot, ctx, message);
        }
        return slot;
    } else if(ret == MAP_MISSING) {
        // no slot. search recursively in the proto.
        SkObject *proto;
        sk_object_get_slot(self, "proto", (void **)&proto);
        if(!proto) {
            // we have no proto. return NULL.
            printf("No proto.\n");
            return NULL;
        } else {
            return sk_object_dispatch_message_simple(proto, ctx, message);
        }
    } else {
        // THAT WILL NEVER RETURN SOMETHING ELSE!
        abort();
    }
}

SkObject *sk_object__set_slot(SkObject *slot, SkObject *self, SkObject *msg) {
    SkObject *arguments = sk_message_get_arguments(msg);
    bstring name = sk_string_get_bstring(sk_list_get_at(arguments, 0));
    sk_object_set_slot_bstring(self, name, sk_list_get_at(arguments, 1));
    return self;
}

SkObject *sk_object__get_slot(SkObject *_slot, SkObject *self, SkObject *msg) {
    SkObject *arguments = sk_message_get_arguments(msg);
    bstring name = sk_string_get_bstring(sk_list_get_at(arguments, 0));
    
    SkObject *slot;
    if(sk_object_get_slot_bstring(self, name, (void **)&slot) != MAP_OK) {
        abort();
    } // uh ... make that nicer
    return slot;
}

SkObject *sk_object__to_bool(SkObject *slot, SkObject *self, SkObject *msg) {
    /* every object evaluates to true as a default. */
    return SK_VM->true; 
}

SkObject *sk_object__to_repr(SkObject *slot, SkObject *self, SkObject *msg) {
    return sk_string_from_bstring(SK_VM, sk_object_to_repr_simple(self));
}

SkObject *sk_object__print(SkObject *slot, SkObject *self, SkObject *msg) {
    printf("%s", bstr2cstr(sk_object_to_repr(self), '\\'));
    return self;
}

SkObject *sk_object__println(SkObject *slot, SkObject *self, SkObject *msg) {
    sk_object__print(slot, self, msg);
    printf("\n");
    return self;
}
