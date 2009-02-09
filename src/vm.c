#include <stdio.h>

#include "mem.h"
#include "object.h"
#include "vm.h"

SkVM *sk_vm_new() {
    SkVM *vm = sk_malloc(sizeof(SkVM));
    SkObject *uberproto = sk_object_create_proto(vm);
    /* the lobby is actually an Object clone, too. */
    vm->lobby = sk_object_clone(uberproto);
    sk_vm_add_proto(vm, "Object", uberproto);
    /* nil is none. */
    vm->nil = sk_object_new(vm);
    cvector_create(&vm->callstack, sizeof(SkObject *), 10);
    sk_vm_callstack_push(vm, vm->lobby);
    return vm;
}

void sk_vm_add_proto(SkVM *vm, const char *name, SkObject *obj) {
    sk_object_put_slot(vm->lobby, name, obj);    
}

SkObject *sk_vm_get_proto(SkVM *vm, const char *name) {
    return sk_object_get_slot_lazy(vm->lobby, name); 
}

