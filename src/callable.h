#ifndef CALLABLE_H_INCLUDED
#define CALLABLE_H_INCLUDED

#include "object.h"
#include "block.h"
#include "vm.h"

typedef SkObject* (*SkCFunction)(SkObject *, SkObject *);

enum SkCallableType {
    SK_CALLABLE_TYPE_CFUNCTION,
    SK_CALLABLE_TYPE_BLOCK
};

typedef struct _SkCallableData {
    int type;
    union {
        SkCFunction c_function;
        SkObject *block;
    };
} SkCallableData;

void sk_callable_init(SkObject *self);
SkObject *sk_callable_create_proto(SkVM *vm);
void sk_callable_set_callable(SkObject *self, SkCFunction callable);
SkObject *sk_callable_from_cfunction(SkVM *vm, SkCFunction func);
SkObject *sk_callable_call(SkObject *self, SkObject *message);
DECLARE_LAZY_CLONE_FUNC(sk_callable_clone);

#define sk_callable_get_data(self) \
    ((SkCallableData *)(self->data))
#define sk_is_callable(self) \
    (((self)->clone_func) == sk_callable_clone)

#endif
