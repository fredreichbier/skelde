#ifndef VM_H_INCLUDED
#define VM_H_INCLUDED

#include "object.h"
#include "hashmap.h"

typedef struct _SkVM {
    SkObject *lobby;
    SkObject *nil; // TODO: oh. it's rather a slot.
} SkVM;

SkVM *sk_vm_new();
void sk_vm_add_proto(SkVM *vm, const char *name, SkObject *obj);

#endif
