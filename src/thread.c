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
    sk_vm_setup_thread(SK_VM);
    sk_thread_set_running(self, TRUE);
    sk_message_dispatch_avalanche(sk_thread_get_message(self));
    sk_thread_set_running(self, FALSE);
//    sk_vm_kill_thread(SK_VM);
    return NULL;
}

void sk_thread_start(SkObject *self) {
    SkThreadData *data = sk_thread_get_data(self);
    pthread_create(&data->thread, &SK_VM->tattr, &_sk_thread_task, (void *)self);
}

SkObject *sk_thread_join(SkObject *self) {
    SkThreadData *data = sk_thread_get_data(self);
    if(sk_thread_get_running(self)) {
        pthread_join(data->thread, NULL);
    }
    return self;
}

SkObject *sk_thread__start(SkObject *slot, SkObject *self, SkObject *msg) {
    sk_thread_start(self);
    return self;
}

SkObject *sk_thread__join(SkObject *slot, SkObject *self, SkObject *msg) {
    return sk_thread_join(self);
}
