#ifndef VM_H_INCLUDED
#define VM_H_INCLUDED

#include "object.h"
#include "cvector.h"
#include "hashmap.h"

typedef struct _SkVM {
    SkObject *lobby;
    SkObject *nil, *true, *false; // TODO: oh. it's rather a slot.
    CVector *callstack;
} SkVM;

SkVM *sk_vm_new();
void sk_vm_add_proto(SkVM *vm, const char *name, SkObject *obj);
SkObject *sk_vm_get_proto(SkVM *vm, const char *name);
SkObject *sk_vm_dispatch_message(SkVM *vm, SkObject *message);
void sk_vm_callstack_push(SkVM *vm, SkObject *ctx);
SkObject *sk_vm_bool_to_skelde(SkVM *vm, _Bool boolean);
_Bool sk_vm_skelde_to_bool(SkVM *vm, SkObject *value);

#define sk_vm_callstack_pop(VM) \
    cvector_pop((VM)->callstack)
#define sk_vm_callstack_top(VM) \
    objlist_top((VM)->callstack)

#endif
