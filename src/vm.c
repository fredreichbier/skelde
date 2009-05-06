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
#include "thread.h"
#include "exception.h"
#include "call.h"
#include "mutex.h"

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
    pthread_key_create(&vm->callstack_key, NULL);
    pthread_key_create(&vm->jmp_ctx_key, NULL);
    pthread_key_create(&vm->exc_key, NULL);
    pthread_attr_init(&vm->tattr);
    pthread_attr_setdetachstate(&vm->tattr, PTHREAD_CREATE_JOINABLE);
    vm->nil = sk_object_new(vm);
    vm->true = sk_object_new(vm);
    vm->false = sk_object_new(vm);
    sk_vm_set_exc(vm, NULL);
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
    sk_vm_add_proto(vm, "Thread", sk_thread_create_proto(vm));
    sk_vm_add_proto(vm, "Mutex", sk_mutex_create_proto(vm));

    sk_exception_load_protos(sk_vm_get_proto(vm, "Exception"), vm->lobby);

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
    _set_proto_tag("Thread");
    _set_proto_tag("Mutex");

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

void sk_vm_callstack_push(SkVM *vm, SkObject *ctx) {
    // pass the pointer to a value, that means a pointer to a pointer to a SkObject.
    cvector_push(sk_vm_callstack(vm), &ctx); 
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
        sk_printf("Cannot convert Object to _Bool.\n"); /* TODO */
        return TRUE;
    }
}

SkJumpContext *sk_vm_push_jmp_context(SkVM *vm) {
    SkJumpContext *new = sk_malloc(sizeof(SkJumpContext));
    if(!sk_vm_jmp_ctx(vm)) {
        new->next = NULL;
    } else {
        new->next = sk_vm_jmp_ctx(vm);
    }
    sk_vm_set_jmp_ctx(vm, new);
    new->callstack = cvector_copy(sk_vm_callstack(vm));
    return new;
}

SkJumpContext *sk_vm_pop_jmp_context(SkVM *vm) {
    SkJumpContext *old = sk_vm_jmp_ctx(vm);
    assert(old != NULL);
    sk_vm_set_jmp_ctx(vm, old->next);
    return old;
}

void sk_vm_handle_root_exception(SkVM *vm, SkJumpCode code) {
    switch(code) {
        case SK_JUMP_CODE_EXCEPTION:
            sk_printf("[skelde] Unhandled Exception: %s\n",
                    bstr2cstr(
                        sk_string_get_bstring(
                            sk_object_get_slot_lazy( /* TODO: exception's message HAS to be a String */
                                sk_vm_exc(vm),
                                "message"
                                )
                            ),
                        '\\'
                        )
                  );
            break;
        default:
            sk_printf("[skelde] Strange jump code detected: %d. That should never happen.",
                    code);
    }
    abort();
}

void sk_vm_setup_thread(SkVM *vm) {
    CVector *callstack;
    cvector_create(&callstack, sizeof(SkObject *), 4);
    pthread_setspecific(vm->callstack_key, (void *)callstack);
    sk_vm_callstack_push(vm, vm->lobby);
}

void sk_vm_kill_thread(SkVM *vm) {
    pthread_setspecific(vm->callstack_key, NULL);
    pthread_setspecific(vm->jmp_ctx_key, NULL);
    pthread_setspecific(vm->exc_key, NULL);
}
