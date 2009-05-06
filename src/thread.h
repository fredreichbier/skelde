#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#include "object.h"
#include "vm.h"

typedef struct _SkThreadData {
    pthread_t thread;
    CVector *parent_callstack;
} SkThreadData;

void sk_thread_init(SkObject *self);
SkObject *sk_thread_create_proto(SkVM *vm);
void *_sk_thread_task(void *self_);
void sk_thread_start(SkObject *self);
SkObject *sk_thread_join(SkObject *self);

DECLARE_LAZY_CLONE_FUNC(sk_thread_clone);

SkObject *sk_thread__start(SkObject *slot, SkObject *self, SkObject *msg);
SkObject *sk_thread__join(SkObject *slot, SkObject *self, SkObject *msg);

#define sk_thread_get_data(SELF) ((SkThreadData *)sk_object_get_data(SELF))
#define sk_thread_get_message(SELF) sk_object_get_slot_recursive(SELF, "message")
#define sk_thread_set_running(SELF, VALUE) sk_object_set_slot(SELF, "running", sk_vm_bool_to_skelde(SELF->vm, VALUE))
#define sk_thread_get_running(SELF) sk_vm_skelde_to_bool(SELF->vm, sk_object_get_slot_recursive(SELF, "running"))

#endif
