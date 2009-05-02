#include <stdio.h>

#include "mem.h"
#include "object.h"
#include "vm.h"
#include "object.h"
#include "objlist.h"
#include "number.h"
#include "list.h"
#include "message.h"
#include "skstring.h"

SkVM *sk_vm_new() {
    SkVM *vm = sk_malloc(sizeof(SkVM));
    /* The lobby is not a real object ... ok? */
    vm->lobby = sk_object_new(vm);
    SkObject *uberproto = sk_object_create_proto(vm);
    sk_object_set_slot(vm->lobby, "proto", uberproto);
    /* make the protos */
    sk_vm_add_proto(vm, "Object", uberproto);
    sk_vm_add_proto(vm, "Number", sk_number_create_proto(vm));
    sk_vm_add_proto(vm, "List", sk_list_create_proto(vm));
    sk_vm_add_proto(vm, "Message", sk_message_create_proto(vm));
    sk_vm_add_proto(vm, "String", sk_string_create_proto(vm));
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

/* try at the top of the callstack, then go to 0. If no receiver found, return NULL */
SkObject *sk_vm_dispatch_message(SkVM *vm, SkObject *message) {
    int i = cvector_size(vm->callstack) - 1;
    while(i >= 0) {
        SkObject *ctx = objlist_get_at(vm->callstack, i); 
        SkObject *result = sk_object_dispatch_message(ctx, message);
        if(result != NULL) {
            return result;
        }
        i--;
    }
    return NULL;
}

void sk_vm_callstack_push(SkVM *vm, SkObject *ctx) {
    // pass the pointer to a value, that means a pointer to a pointer to a SkObject.
    cvector_push(vm->callstack, &ctx); 
}
