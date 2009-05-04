#include "thread.h"
#include "message.h"
#include "mem.h"

void sk_thread_init(SkObject *self) {
    sk_object_set_data(self, (void *)sk_malloc(sizeof(SkThreadData)));
}

SkObject *sk_thread_create_proto(SkVM *vm) {
    SkObject *self = sk_object_clone(sk_vm_get_proto(vm, "Object"));
    sk_object_set_clone_func(self, &sk_thread_clone);
    sk_object_set_init_func(self, &sk_thread_init);
    sk_thread_init(self);
    /* methods */
    sk_object_bind_method(self, "start", &sk_thread__start);
    sk_object_bind_method(self, "join", &sk_thread__join);
    return self;
}

DEFINE_LAZY_CLONE_FUNC(sk_thread_clone);

void *_sk_thread_task(void *self_) {
    SkObject *self = (SkObject *)self_;
    SkObject *message = sk_thread_get_message(self);
    return (void *)sk_message_dispatch_avalanche(message);
}

void sk_thread_start(SkObject *self) {
    SkThreadData *data = sk_thread_get_data(self);
    GC_pthread_create(&data->thread, NULL, &_sk_thread_task, (void *)self);
}

SkObject *sk_thread_join(SkObject *self) {
    SkThreadData *data = sk_thread_get_data(self);
    SkObject *result;
    GC_pthread_join(data->thread, (void **)&result);
    return result;
}

SkObject *sk_thread__start(SkObject *slot, SkObject *self, SkObject *msg) {
    sk_thread_start(self);
    return self;
}

SkObject *sk_thread__join(SkObject *slot, SkObject *self, SkObject *msg) {
    return sk_thread_join(self);
}
