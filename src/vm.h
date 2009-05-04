#ifndef VM_H_INCLUDED
#define VM_H_INCLUDED

#include "object.h"
#include "cvector.h"
#include "hashmap.h"
#include "exc.h"

typedef struct _SkVM {
    SkObject *lobby;
    SkObject *nil, *true, *false; // TODO: oh. it's rather a slot.
    SkObject *exc; /* the current exception. TODO: use a stack? */
    SkJumpContext *jmp_ctx;
    CVector *callstack;
    pthread_mutex_t stdout_mutex;
} SkVM;

SkVM *sk_vm_new();
void sk_vm_add_proto(SkVM *vm, const char *name, SkObject *obj);
SkObject *sk_vm_get_proto(SkVM *vm, const char *name);
SkObject *sk_vm_dispatch_message(SkVM *vm, SkObject *message);
void sk_vm_callstack_push(SkVM *vm, SkObject *ctx);
SkObject *sk_vm_bool_to_skelde(SkVM *vm, _Bool boolean);
_Bool sk_vm_skelde_to_bool(SkVM *vm, SkObject *value);
SkJumpContext *sk_vm_push_jmp_context(SkVM *vm);
SkJumpContext *sk_vm_pop_jmp_context(SkVM *vm);
void sk_vm_handle_root_exception(SkVM *vm, SkJumpCode code);

void sk_vm_printf(SkVM *vm, const char *format, ...);

#define sk_vm_callstack_pop(VM) \
    cvector_pop((VM)->callstack)
#define sk_vm_callstack_top(VM) \
    objlist_top((VM)->callstack)
#define sk_vm_exc(VM) \
    ((VM)->exc)

#endif
