#ifndef VM_H_INCLUDED
#define VM_H_INCLUDED

#include "object.h"
#include "cvector.h"
#include "exc.h"

typedef struct _SkVM {
    SkObject *lobby;
    SkObject *nil, *true, *false; // TODO: oh. it's rather a slot.
    pthread_key_t callstack_key, jmp_ctx_key, exc_key;
    pthread_attr_t tattr;
} SkVM;

SkVM *sk_vm_new();
void sk_vm_add_proto(SkVM *vm, const char *name, SkObject *obj);
SkObject *sk_vm_get_proto(SkVM *vm, const char *name);
void sk_vm_callstack_push(SkVM *vm, SkObject *ctx);
SkObject *sk_vm_bool_to_skelde(SkVM *vm, _Bool boolean);
_Bool sk_vm_skelde_to_bool(SkVM *vm, SkObject *value);
SkJumpContext *sk_vm_push_jmp_context(SkVM *vm);
SkJumpContext *sk_vm_pop_jmp_context(SkVM *vm);
void sk_vm_handle_root_exception(SkVM *vm, SkJumpCode code);
void sk_vm_setup_thread(SkVM *vm);
void sk_vm_kill_thread(SkVM *vm);

#define sk_vm_callstack_pop(VM) \
    cvector_pop(sk_vm_callstack(VM))
#define sk_vm_callstack_top(VM) \
    objlist_top(sk_vm_callstack(VM))
#define sk_vm_callstack(VM) \
    ((CVector *)pthread_getspecific((VM)->callstack_key))
#define sk_vm_set_callstack(VM, VALUE) \
    (pthread_setspecific((VM)->callstack_key, (void *)VALUE))
#define sk_vm_jmp_ctx(VM) \
    ((SkJumpContext *)pthread_getspecific((VM)->jmp_ctx_key))
#define sk_vm_set_jmp_ctx(VM, VALUE) \
    (pthread_setspecific((VM)->jmp_ctx_key, (void *)VALUE))
#define sk_vm_exc(VM) \
    ((SkObject *)pthread_getspecific((VM)->exc_key))
#define sk_vm_set_exc(VM, VALUE) \
    (pthread_setspecific((VM)->exc_key, (void *)VALUE))
#endif
