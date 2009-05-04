#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#include "object.h"
#include "vm.h"

typedef struct _SkThreadData {
    pthread_t thread;
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
#define sk_thread_get_message(SELF) sk_object_get_slot_recursive(SELF, "message");

#endif
