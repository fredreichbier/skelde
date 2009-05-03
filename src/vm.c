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
#include "method.h"
#include "exception.h"
#include "call.h"

static SkObject *_sk_vm_to_bool_false(SkObject *slot, SkObject *self, SkObject *msg) {
    return self->vm->false;
}

static void _sk_vm_setup_true(SkObject *self) {
    sk_object_set_activatable(self, FALSE);
    sk_object_set_proto(self, sk_vm_get_proto(self->vm, "Object"));
}

static void _sk_vm_setup_false(SkObject *self) {
    sk_object_set_activatable(self, FALSE);
    sk_object_set_proto(self, sk_vm_get_proto(self->vm, "Object"));
    sk_object_bind_method(self, "to_bool", &_sk_vm_to_bool_false);
}

static void _sk_vm_setup_nil(SkObject *self) {
    sk_object_set_activatable(self, FALSE);
    sk_object_set_proto(self, sk_vm_get_proto(self->vm, "Object"));
    sk_object_bind_method(self, "to_bool", &_sk_vm_to_bool_false);
}

SkVM *sk_vm_new() {
    SkVM *vm = sk_malloc(sizeof(SkVM));
    vm->nil = sk_object_new(vm);
    vm->true = sk_object_new(vm);
    vm->false = sk_object_new(vm);
    vm->exc = NULL;
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
    sk_vm_add_proto(vm, "Exception", sk_exception_create_proto(vm));
    sk_vm_add_proto(vm, "Method", sk_method_create_proto(vm));
    sk_vm_add_proto(vm, "Call", sk_call_create_proto(vm));

    sk_exception_load_protos(sk_vm_get_proto(vm, "Exception"), vm->lobby);
    cvector_create(&vm->callstack, sizeof(SkObject *), 10);
    sk_vm_callstack_push(vm, vm->lobby);

    sk_object_set_activatable(uberproto, FALSE);
    _sk_vm_setup_true(vm->true);
    sk_vm_add_proto(vm, "true", vm->true);
    _sk_vm_setup_false(vm->false);
    sk_vm_add_proto(vm, "false", vm->false);
    _sk_vm_setup_nil(vm->nil);
    sk_vm_add_proto(vm, "nil", vm->nil);

#define _set_proto_tag(PROTO) sk_object_set_tag(sk_vm_get_proto(vm, PROTO), bfromcstr(PROTO))
    sk_object_set_tag(uberproto, bfromcstr("Object"));
    _set_proto_tag("Number");
    _set_proto_tag("List");
    _set_proto_tag("Message");
    _set_proto_tag("String");
    _set_proto_tag("Exception");
    _set_proto_tag("Method");
    _set_proto_tag("Call");

    sk_object_set_tag(vm->true, bfromcstr("true"));
    sk_object_set_tag(vm->false, bfromcstr("false"));
    sk_object_set_tag(vm->nil, bfromcstr("nil"));
#undef _set_proto_tag

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

SkObject *sk_vm_bool_to_skelde(SkVM *vm, _Bool boolean) {
    return boolean ? vm->true : vm->false;
}

_Bool sk_vm_skelde_to_bool(SkVM *vm, SkObject *value) {
    if(value == vm->true) {
        return TRUE;
    } else if(value == vm->false) {
        return FALSE;
    } else {
        printf("Cannot convert Object to _Bool.\n"); /* TODO */
        return TRUE;
    }
}

SkJumpContext *sk_vm_push_jmp_context(SkVM *vm) {
     if(!vm->jmp_ctx) {
        vm->jmp_ctx = sk_malloc(sizeof(SkJumpContext));
        vm->jmp_ctx->next = NULL;
     } else {
        SkJumpContext *new = sk_malloc(sizeof(SkJumpContext));
        new->next = vm->jmp_ctx;
        vm->jmp_ctx = new;
     }
     vm->jmp_ctx->callstack = cvector_copy(vm->callstack);
     return vm->jmp_ctx;
}

SkJumpContext *sk_vm_pop_jmp_context(SkVM *vm) {
    assert(vm->jmp_ctx != NULL);
    SkJumpContext *old = vm->jmp_ctx;
    vm->jmp_ctx = vm->jmp_ctx->next;
    return old;
}

void sk_vm_handle_root_exception(SkVM *vm, SkJumpCode code) {
    switch(code) {
        case SK_JUMP_CODE_EXCEPTION:
            printf("[skelde] Unhandled Exception: %s\n",
                    bstr2cstr(
                        sk_string_get_bstring(
                            sk_object_get_slot_lazy( /* TODO: exception's message HAS to be a String */
                                vm->exc,
                                "message"
                                )
                            ),
                        '\\'
                        )
                  );
            break;
        default:
            printf("[skelde] Strange jump code detected: %d. That should never happen.",
                    code);
    }
    abort();
}
