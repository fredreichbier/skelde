#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include <string.h>
#include <stdio.h>

#include "mem.h"
#include "khash.h"
#include "kvec.h"
#include "stuff.h"
#include "bstrlib.h"

/* Sorry for the forward declaration! */
struct _SkObject;
struct _SkVM;

typedef void (*SkInitFunction)(struct _SkObject *);
typedef void (*SkInitModuleFunction)(struct _SkObject *);
typedef struct _SkObject *(*SkCloneFunction)(struct _SkObject *);
typedef struct _SkObject *(*SkCallFunction)(struct _SkObject *, struct _SkObject *, struct _SkObject *);
typedef struct _SkObject *(*SkDispatchFunction)(struct _SkObject *, struct _SkObject *, struct _SkObject *);

KHASH_MAP_INIT_STR(Slots, struct _SkObject*);

typedef struct _SkObject {
    struct _SkVM *vm;
    khash_t(Slots) *slots;
    void *data;
    pthread_mutex_t data_mutex, slots_mutex;
    /* customizable function pointers */
    SkInitFunction init_func;
    SkCloneFunction clone_func;
    SkCallFunction call_func;
    SkDispatchFunction dispatch_func;
} SkObject;

typedef kvec_t(SkObject *) SkObjectList;

SkObject *sk_object_new(struct _SkVM *vm);
SkObject *sk_object_clone(SkObject *self);
SkObject *sk_object_clone_base(SkObject *self);
SkObject *sk_object_create_proto(struct _SkVM *vm);
SkObject *sk_object_call(SkObject *self, SkObject *ctx, SkObject *message);
_Bool sk_object_get_activatable(SkObject *self);
void sk_object_set_data(SkObject *self, void *data);
SkObject *sk_object_get_slot(SkObject *self, const char *name);
void sk_object_set_slot(SkObject *self, const char *name, SkObject *value);
SkObject *sk_object_get_slot_bstring(SkObject *self, const_bstring name);

SkObject *sk_object_get_slot_lazy(SkObject *self, const char *name);
SkObject *sk_object_dispatch_message(SkObject *self, SkObject *msg);
SkObject *sk_object_dispatch_message_simple(SkObject *self, SkObject *ctx, SkObject *message);
SkObject *sk_callable_create(struct _SkVM *vm, SkCallFunction func);
void sk_object_bind_method(SkObject *self, char *name, SkCallFunction func);
_Bool sk_object_to_bool(SkObject *self);
SkObject *sk_object_send_message_simple(SkObject *self, SkObject *msg);
SkObject *sk_object_get_slot_recursive(SkObject *self, const char *name);
bstring sk_object_to_repr_simple(SkObject *self);
bstring sk_object_to_repr(SkObject *self);
bstring sk_object_to_string(SkObject *self);
bstring sk_object_get_tag_recursive(SkObject *self);
SkObject *sk_object_create_child_context(SkObject *self);
void sk_object_load_module(SkObject *self, const char *filename);
_Bool sk_object_is_derived_from(SkObject *self, SkObject *proto);

SkObject *sk_object__set_slot(SkObject *self, SkObject *ctx, SkObject *msg);
SkObject *sk_object__get_slot(SkObject *self, SkObject *ctx, SkObject *msg);
SkObject *sk_object__to_bool(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_object__to_repr(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_object__to_string(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_object__print(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_object__println(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_object__break(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_object__continue(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_object__if(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_object__equals(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_object__clone(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_object__message(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_object__update_slot(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_object__load_shared_module(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_object__try(SkObject *slot, SkObject *self, SkObject *msg);

#define sk_object_has_slot(obj, name) \
    (sk_object_get_slot(obj, name) != NULL)

#define sk_object_get_data(obj) \
    ((SkObject *)obj)->data

#define sk_object_set_activatable(obj, VALUE) \
    sk_object_set_slot(obj, "activatable", sk_vm_bool_to_skelde(obj->vm, VALUE))

#define sk_object_set_init_func(obj, _func) \
    ((SkObject *)(obj))->init_func = (_func);

#define sk_object_put_slot sk_object_set_slot

#define sk_object_set_clone_func(obj, func) \
    ((SkObject *)(obj))->clone_func = (func)

#define sk_object_set_dispatch_func(obj, func) \
    ((SkObject *)(obj))->dispatch_func = (func)

#define sk_object_set_call_func(obj, func) \
    ((SkObject *)(obj))->call_func = (func)

#define sk_object_set_proto(obj, proto) \
    sk_object_set_slot(obj, "proto", proto)

#define sk_object_get_proto(obj) \
    sk_object_get_slot_lazy(obj, "proto")

#define sk_object_has_proto(obj) \
    sk_object_has_slot(obj, "proto")

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
        sk_object_set_dispatch_func(other, self->dispatch_func); \
        sk_object_set_call_func(other, self->call_func); \
        sk_object_set_clone_func(other, self->clone_func); \
        other->init_func = self->init_func; \
        if(self->init_func) { \
            (self->init_func)(other); \
        } \
        return other; \
    }

#define sk_object_set_tag(OBJ, TAG) \
    sk_object_set_slot(OBJ, "tag", sk_string_from_bstring(OBJ->vm, TAG))
#define sk_object_get_tag(OBJ) \
    sk_string_get_bstring(sk_object_get_slot_recursive(OBJ, "tag"))

#endif
