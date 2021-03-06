#include <stdio.h>
#include <dlfcn.h>

#include "object.h"
#include "mem.h"
#include "skstring.h"
#include "list.h"
#include "message.h"
#include "vm.h"
#include "number.h"
#include "exception.h"

/* creates a new skelde object and returns it. */
SkObject *sk_object_new(SkVM *vm) {
    SkObject *obj = sk_malloc(sizeof(SkObject));
    obj->vm = vm;
    obj->slots = kh_init(Slots);
    pthread_mutex_init(&obj->data_mutex, NULL);
    pthread_mutex_init(&obj->slots_mutex, NULL);
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
    sk_object_bind_method(self, "to_string", &sk_object__to_string);
    sk_object_bind_method(self, "to_repr", &sk_object__to_repr);
    sk_object_bind_method(self, "print", &sk_object__print);
    sk_object_bind_method(self, "println", &sk_object__println);
    sk_object_bind_method(self, "set_slot", &sk_object__set_slot);
    sk_object_bind_method(self, "get_slot", &sk_object__get_slot);
    sk_object_bind_method(self, "break", &sk_object__break);
    sk_object_bind_method(self, "continue", &sk_object__continue);
    sk_object_bind_method(self, "==", &sk_object__equals);
    sk_object_bind_method(self, "if", &sk_object__if);
    sk_object_bind_method(self, "try", &sk_object__try);
    sk_object_bind_method(self, "clone", &sk_object__clone);
    sk_object_bind_method(self, "message", &sk_object__message);
    sk_object_bind_method(self, "update_slot", &sk_object__update_slot);
    sk_object_bind_method(self, "load_shared_module", &sk_object__load_shared_module);
    return self;
}

SkObject *sk_object_get_slot(SkObject *self, const char *name) {
    pthread_mutex_lock(&self->slots_mutex);
    SkObject *obj = NULL;
    khiter_t k = kh_get(Slots, self->slots, name);
    if(k == kh_end(self->slots)) {
        obj = NULL;
    } else {
        obj = kh_value(self->slots, k);
    }
    pthread_mutex_unlock(&self->slots_mutex);
    return obj;
}

SkObject *sk_object_get_slot_bstring(SkObject *self, const_bstring name) {
    return sk_object_get_slot(self, (const char *)name->data); /* TODO: bstr2cstr? */
}

void sk_object_set_slot(SkObject *self, const char *name, SkObject *value) {
    pthread_mutex_lock(&self->slots_mutex);
    int rv;
    /* TODO: seems like i have to copy the key to prevent segfaults. crazy stuff. */
    char *copied = sk_malloc((strlen(name) + 1) * sizeof(char));
    strcpy(copied, name);
    khiter_t k = kh_put(Slots, self->slots, copied, &rv);
    kh_value(self->slots, k) = value;
    pthread_mutex_unlock(&self->slots_mutex);
}

void sk_object_set_data(SkObject *self, void *data) {
    pthread_mutex_lock(&self->data_mutex);
    self->data = data;
    pthread_mutex_unlock(&self->data_mutex);
}

SkObject *sk_object_get_slot_lazy(SkObject *self, const char *name) {
    SkObject *slot = sk_object_get_slot(self, name);
    if(slot == NULL) {
        sk_printf("Couldn't find slot '%s'.\n", name);
        abort();
    }
    return slot;
}

SkObject *sk_object_get_slot_recursive(SkObject *self, const char *name) {
    SkObject *slot = NULL;
    while(self != NULL) {
        slot = sk_object_get_slot(self, name);
        if(slot != NULL) {
            return slot;
        }
        self = sk_object_get_slot(self, "proto");
    }
    return NULL;
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
    return bformat("<%s at %p>",
            sk_object_get_tag_recursive(self)->data,
            self);
}

bstring sk_object_get_tag_recursive(SkObject *self) {
    SkObject *root = self;
    unsigned int count = 0, i = 0;
    /* will never fail, because all objects are descendants of Object,
     * and it has `name` set. */
    while(!sk_object_has_slot(root, "tag")) {
        root = sk_object_get_slot_lazy(root, "proto");
        count++;
    }
    bstring name = bstrcpy(sk_string_get_bstring(
                sk_object_get_slot_lazy(root, "tag")));
    if(i > count) {
        /* blubblubblubbunderflow? */
        abort();
    }
    binsertch(name, name->slen, count, '^');
    return name;
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

bstring sk_object_to_string(SkObject *self) {
    return sk_string_get_bstring(
            sk_object_send_message_simple(self,
                    sk_message_create_simple(
                        SK_VM,
                        "to_string"
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
    sk_vm_callstack_pop(SK_VM) = NULL; /* to make the gcc happy */
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

SkObject *sk_object_create_child_context(SkObject *self) {
    SkObject *ctx = sk_object_new(SK_VM); /* TODO: clone `Object`? */
    sk_object_set_slot(ctx, "self", self);
    return ctx;
}

void sk_object_load_module(SkObject *self, const char *filename) {
    /* open the library */
    void *lib = dlopen(filename, RTLD_LAZY | RTLD_GLOBAL);
    if(!lib) {
        /* TODO: exception */
        sk_printf("Error loading shared module: %s\n", dlerror());
        return;
    }
    /* load the sk_init_module function pointer ... */
    SkInitModuleFunction init_func = (SkInitModuleFunction)dlsym(lib, "sk_init_module");
    if(!init_func) {
        /* TODO: exception */
        sk_printf("Error loading the module init function: %s\n", dlerror());
        return;
    }
    /* execute it, and we're done. */
    init_func(self);
    /* close the library. */
    dlclose(lib);
}

SkObject *sk_object_dispatch_message(SkObject *self, SkObject *msg) {
    return (self->dispatch_func)(self, self, msg);
}

_Bool sk_object_is_derived_from(SkObject *self, SkObject *proto) {
    while(sk_object_has_proto(self)) {
        self = sk_object_get_proto(self);
        if(self == proto) {
            return TRUE;
        }
    }
    return FALSE;
}

SkObject *sk_object_dispatch_message_simple(SkObject *self, SkObject *ctx, SkObject *message) {
    // TODO: type checking
    bstring name = sk_string_get_bstring(sk_object_get_slot_lazy(message, "name"));
    SkObject *slot = sk_object_get_slot_bstring(self, name);
    if(slot != NULL) {
        /* if activatable: call it. */
        if(sk_object_get_activatable(slot)) {
            slot = sk_object_call(slot, ctx, message);
        }
        return slot;
    } else {
        // no slot. search recursively in the proto.
        if(!sk_object_has_slot(self, "proto")) {
            // we have no proto. return NULL.
            return NULL;
        } else {
            return sk_object_dispatch_message_simple(sk_object_get_slot_lazy(self, "proto"),
                    ctx, message);
        }
    }
}

SkObject *sk_object__set_slot(SkObject *slot, SkObject *self, SkObject *msg) {
    sk_message_check_argcount(msg, "Object set_slot", 2);
    bstring name = sk_string_get_bstring(sk_message_eval_arg_at(msg, 0));
    sk_object_set_slot(self, bstr2cstr(name, '\\'), sk_message_eval_arg_at(msg, 1));
    return self;
}

SkObject *sk_object__get_slot(SkObject *_slot, SkObject *self, SkObject *msg) {
    sk_message_check_argcount(msg, "Object get_slot", 1);
    bstring name = sk_string_get_bstring(sk_message_eval_arg_at(msg, 0));
    SkObject *slot = sk_object_get_slot_recursive(self, bstr2cstr(name, '\\'));
    if(!slot) {
        sk_exc_raise(SK_VM, sk_exception_create_lazy(SK_VM,
                    "SlotError", bformat("The slot '%s' couldn't be found.",
                        name->data)));
    }
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
    sk_printf("%s", bstr2cstr(sk_object_to_string(self), '\\'));
    return self;
}

SkObject *sk_object__println(SkObject *slot, SkObject *self, SkObject *msg) {
    sk_object__print(slot, self, msg);
    sk_printf("\n");
    return self;
}

SkObject *sk_object__to_string(SkObject *slot, SkObject *self, SkObject *msg) {
    return sk_object__to_repr(slot, self, msg);
}

SkObject *sk_object__break(SkObject *slot, SkObject *self, SkObject *msg) {
    sk_exc_jump(SK_VM, SK_JUMP_CODE_BREAK);
    return self;
}

SkObject *sk_object__continue(SkObject *slot, SkObject *self, SkObject *msg) {
    sk_exc_jump(SK_VM, SK_JUMP_CODE_CONTINUE);
    return self;
}


/* try(
 *  block to try,
 *  [exception to catch,
 *  exception message name,
 *  exception catching block,
 *      [exception to catch,
 *      exception message name,
 *      exception catching block,
 *      ...]
 *  ],
 *  [exception name,
 *  exception catching block]
 * )
 */
SkObject *sk_object__try(SkObject *slot, SkObject *self, SkObject *msg) {
    ArgCount argcount = sk_message_argcount(msg);
    ArgCount regular_blocks = (argcount - 1) / 3;
    ArgCount i;
    _Bool handled = FALSE;
    sk_exc_try(SK_VM) {
        sk_message_eval_arg_at(msg, 0);
    } sk_exc_except(SK_JUMP_CODE_EXCEPTION) {
        /* exception caught! */
        SkObject *exc = sk_vm_exc(SK_VM);
        for(i = 0; i < regular_blocks; i++) {
            if(sk_object_is_derived_from(exc, sk_message_eval_arg_at(msg, i * 3 + 1))) {
                SkObject *ctx = sk_object_create_child_context(self);
                sk_object_set_slot(ctx,
                        sk_string_as_charp(
                            sk_message_get_name(
                                sk_message_arg_at(
                                    msg,
                                    i * 3 + 2
                                    )
                                )
                            ),
                        exc
                        );
                sk_vm_callstack_push(SK_VM, ctx);
                sk_message_eval_arg_at(msg, i * 3 + 3);
                sk_vm_callstack_pop(SK_VM) = NULL;
                handled = TRUE;
                break;
            }
        }
        /* no regular block matched ... */
        if(!handled && argcount > regular_blocks * 3 + 1) {
            SkObject *ctx = sk_object_create_child_context(self);
            sk_object_set_slot(ctx,
                    sk_string_as_charp(
                        sk_message_get_name(
                            sk_message_arg_at(
                                msg,
                                argcount - 2
                                )
                            )
                        ),
                    exc
                    );
            sk_vm_callstack_push(SK_VM, ctx);
            sk_message_eval_arg_at(msg, argcount - 1);
            sk_vm_callstack_pop(SK_VM) = NULL;
            handled = TRUE;
        }
    } sk_exc_else {
        sk_exc_pass(SK_VM);
    } sk_exc_end_try(SK_VM);
    return self;
}

/* The `if` message:
 * if(condition,
 *      block,
 *    condition,
 *      block,
 *    else_block)
 */
SkObject *sk_object__if(SkObject *slot, SkObject *self, SkObject *msg) {
    ArgCount argcount = sk_message_argcount(msg);
    ArgCount blocks = argcount / 2;
    ArgCount i;
    sk_message_check_argcount(msg, "Object if", 1);
    /* if we have only one argument, return it as a bool */
    if(argcount == 1) {
        return sk_vm_bool_to_skelde(SK_VM, sk_object_to_bool(sk_message_eval_arg_at(msg, 0)));
    }
    for(i = 0; i < blocks; i++) {
        if(sk_vm_skelde_to_bool(SK_VM,
                    sk_message_eval_arg_at(msg, i * 2))) {
            /* argument is true */
            return sk_message_eval_arg_at(msg, i * 2 + 1);
        }
    }
    /* we have an else block */
    if(blocks * 2 < argcount) {
        return sk_message_eval_arg_at(msg, argcount - 1);
    } else {
        /* no condition was true. return `false`. */
        return SK_VM->false;
    }
}

/* Object == checks for identitiy. */
SkObject *sk_object__equals(SkObject *slot, SkObject *self, SkObject *msg) {
    /* TODO: error check */
    return sk_vm_bool_to_skelde(SK_VM, self == sk_message_eval_arg_at(msg, 0));
}

SkObject *sk_object__clone(SkObject *slot, SkObject *self, SkObject *msg) {
    return sk_object_clone(self);
}

SkObject *sk_object__message(SkObject *slot, SkObject *self, SkObject *msg) {
    /* TODO: error check */
    return sk_message_arg_at(msg, 0);
}

SkObject *sk_object__update_slot(SkObject *slot, SkObject *self, SkObject *msg) {
    sk_message_check_argcount(msg, "Object update_slot", 2);
    bstring name = sk_string_get_bstring(sk_message_eval_arg_at(msg, 0));
    if(sk_object_has_slot(self, bstr2cstr(name, '\\'))) {
        sk_object_set_slot(self, bstr2cstr(name, '\\'), sk_message_eval_arg_at(msg, 1));
        return self;
    } else {
        return NULL; /* Oooo-haaaa! That is DIRTY! */
    }
}

SkObject *sk_object__load_shared_module(SkObject *slot, SkObject *self, SkObject *msg) {
    sk_message_check_argcount(msg, "Object load_shared_module", 1);
    bstring name = sk_string_get_bstring(sk_message_eval_arg_at(msg, 0));
    sk_object_load_module(self, (char *)name->data);
    return self;
}
