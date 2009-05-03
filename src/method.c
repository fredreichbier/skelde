#include "method.h"
#include "message.h"
#include "list.h"
#include "exception.h"
#include "call.h"

/* init: clone a method, but don't set stuff from proto.
 * and method clones are activatable!
 */
void sk_method_init(SkObject *self) {
    sk_method_set_message(self, SK_NIL);
    sk_object_set_activatable(self, TRUE);
    sk_object_set_init_func(self, &sk_method_init2);
}

/* init2: clone a method clone -> do nothing */
void sk_method_init2(SkObject *self) {

}

SkObject *sk_method_create_proto(SkVM *vm) {
    SkObject *self = sk_object_clone(sk_vm_get_proto(vm, "Object"));
    sk_object_set_init_func(self, &sk_method_init);
    sk_object_set_clone_func(self, &sk_method_clone);
    sk_object_set_call_func(self, &sk_method_call);
    /* methods */
    sk_object_bind_method(self, "clone", &sk_method__clone);
    return self;
}

SkObject *sk_method__clone(SkObject *slot, SkObject *self, SkObject *msg) {
    sk_message_check_argcount(msg, "Method clone", 1);
    SkObject *cloned = sk_object_clone(self);
    sk_method_set_message(cloned, sk_message_arg_at(msg, 0));
    /* TODO: argument names */
    return cloned;
}

SkObject *sk_method_call(SkObject *self, SkObject *parentctx, SkObject *call_msg) {
    SkObject *msg = sk_method_get_message(self);
    SkObject *ctx = sk_object_create_child_context(parentctx);
    SkObject *call = sk_object_clone(sk_vm_get_proto(SK_VM, "Call"));
    sk_call_set_message(call, call_msg);
    sk_object_set_slot(ctx, "call", call);

    sk_vm_callstack_push(SK_VM, ctx);
    SkObject *result = sk_message_dispatch_avalanche(msg);
    sk_vm_callstack_pop(SK_VM);
    return result;
}

DEFINE_LAZY_CLONE_FUNC(sk_method_clone);
