#include <string.h>

#include "mem.h"
#include "object.h"
#include "vm.h"

SkVM *sk_vm_new() {
    SkVM *vm = sk_malloc(sizeof(SkVM));
    vm->lobby = sk_object_new();
    vm->nil = sk_object_new();
    return vm;
}

void sk_vm_add_proto(SkVM *vm, const char *name, SkObject *obj) {
    sk_object_put_slot(vm->lobby, name, obj);    
}
