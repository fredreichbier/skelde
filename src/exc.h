#ifndef EXC_H_INCLUDED
#define EXC_H_INCLUDED

#include <setjmp.h>

typedef enum _SkJumpCode {
    SK_JUMP_CODE_EXCEPTION = 1,
    SK_JUMP_CODE_BREAK,
    SK_JUMP_CODE_CONTINUE,
    SK_JUMP_CODE_RETURN,
    SK_JUMP_CODE_SERIOUS
} SkJumpCode;

typedef struct _SkJumpContext {
    struct _SkJumpContext *next;
    jmp_buf jmp;
    CVector *callstack;
} SkJumpContext;

#define sk_exc_raise(VM, EXC) \
    sk_exc_set(VM, EXC); \
    sk_exc_jump(VM, SK_JUMP_CODE_EXCEPTION);

#define sk_exc_jump(VM, CODE) \
    if(!VM->jmp_ctx) { \
        sk_vm_handle_root_exception(VM, CODE); \
    } else { \
        longjmp(VM->jmp_ctx->jmp, CODE); \
    }

#define sk_exc_set(VM, EXC) \
    VM->exc = EXC;
#define sk_exc_try(VM) \
    do { SkJumpContext *__jmp_ctx = sk_vm_push_jmp_context(VM); \
        int __jmp_code; \
        switch(__jmp_code = setjmp(__jmp_ctx->jmp)) { \
            case 0:
#define sk_exc_except(X) break; case X:
#define sk_exc_else break; default:

/* we can do that here: if we jump, the current execution context
 * is invalidated: sk_exc_end_try will never be called. So we
 * have no double-pop. */
#define sk_exc_pass(VM) \
                VM->callstack = __jmp_ctx->callstack; \
                sk_vm_pop_jmp_context(VM); \
                sk_exc_jump(VM, __jmp_code)

#define sk_exc_end_try(VM) \
        } \
        VM->callstack = __jmp_ctx->callstack; \
        sk_vm_pop_jmp_context(VM); \
        } while(0); \

#endif
