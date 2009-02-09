#ifndef VM_H_INCLUDED
#define VM_H_INCLUDED

#include "object.h"
#include "cvector.h"
#include "hashmap.h"

typedef struct _SkVM {
    SkObject *lobby;
    SkObject *nil; // TODO: oh. it's rather a slot.
    CVector *callstack;
} SkVM;

SkVM *sk_vm_new();
void sk_vm_add_proto(SkVM *vm, const char *name, SkObject *obj);
SkObject *sk_vm_get_proto(SkVM *vm, const char *name);

#define sk_vm_callstack_push(VM, CTX) \
    cvector_push((VM)->callstack, (CTX))
#define sk_vm_callstack_pop(VM) \
    cvector_pop((VM)->callstack)
#define sk_vm_callstack_top(VM) \
    cvector_top((VM)->callstack)

#endif
