#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include <string.h>
#include <stdio.h>

#include "hashmap.h"

/* Sorry for the forward declaration! */
struct _SkObject;
struct _SkVM;

typedef void (*SkInitFunction)(struct _SkObject *);
typedef struct _SkObject *(*SkCloneFunction)(struct _SkObject *);

typedef struct _SkObject {
    struct _SkVM *vm;
    Hashmap *slots;
    void *data;
    /* customizable function pointers */
    SkInitFunction init_func;
    SkCloneFunction clone_func;
} SkObject;

SkObject *sk_object_new(struct _SkVM *vm);
SkObject *sk_object_clone(SkObject *self);
SkObject *sk_object_clone_base(SkObject *self);
SkObject *sk_object_create_proto(struct _SkVM *vm);
SkObject *sk_object_get_slot_lazy(SkObject *self, const char *name);
SkObject *sk_object_dispatch_message(SkObject *self, SkObject *message);

SkObject *sk_object__set_slot(SkObject *self, SkObject *message);
SkObject *sk_object__get_slot(SkObject *self, SkObject *message);

#define sk_object_set_data(obj, _data) \
    ((SkObject *)(obj))->data = (_data)

#define sk_object_get_data(obj) \
    ((SkObject *)(obj))->data

#define sk_object_set_init_func(obj, _func) \
    ((SkObject *)(obj))->init_func = (_func);

#define sk_object_put_slot(obj, name, value) \
    hashmap_put((obj)->slots, hashmap_hash_string((name), strlen((name))), (value))

#define sk_object_set_slot sk_object_put_slot
#define sk_object_set_slot_bstring(obj, name, value) \
    hashmap_put((obj)->slots, hashmap_hash_bstring(name), (value))

#define sk_object_get_slot(obj, name, out) \
    hashmap_get((obj)->slots, hashmap_hash_string((name), strlen((name))), (out))

#define sk_object_get_slot_bstring(obj, name, out) \
    hashmap_get((obj)->slots, hashmap_hash_bstring(name), (out))

#define sk_object_set_clone_func(obj, func) \
    ((SkObject *)(obj))->clone_func = (func);

#define SK_VM \
    (((SkObject *)self)->vm)

#define SK_NIL \
    SK_VM->nil

#define DECLARE_LAZY_CLONE_FUNC(NAME) \
    SkObject *NAME(SkObject *self)

#define DEFINE_LAZY_CLONE_FUNC(NAME) \
    DECLARE_LAZY_CLONE_FUNC(NAME) { \
        SkObject *other = sk_object_new(self->vm); \
        sk_object_put_slot(other, "proto", self); \
        if(self->init_func) { \
            (self->init_func)(other); \
        } \
        other->init_func = self->init_func; \
        return other; \
    }

#define sk_object_set_method(self, name, meth) \
    sk_object_set_slot(self, name, sk_callable_from_cfunction((self)->vm, meth))

#endif
