#include "mutex.h"

void sk_mutex_init(SkObject *self) {
    pthread_mutex_t *mutex = sk_malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex, NULL);
    sk_object_set_data(self, (void *)mutex);
}

SkObject *sk_mutex_create_proto(SkVM *vm) {
    SkObject *self = sk_object_clone(sk_vm_get_proto(vm, "Object"));
    sk_object_set_init_func(self, &sk_mutex_init);
    sk_object_set_clone_func(self, &sk_mutex_clone);
    /* methods */
    sk_object_bind_method(self, "lock", &sk_mutex__lock);
    sk_object_bind_method(self, "unlock", &sk_mutex__unlock);
    return self;
}

DEFINE_LAZY_CLONE_FUNC(sk_mutex_clone);

SkObject *sk_mutex__lock(SkObject *slot, SkObject *self, SkObject *msg) {
    pthread_mutex_lock((pthread_mutex_t *)sk_object_get_data(self));
    return self;
}

SkObject *sk_mutex__unlock(SkObject *slot, SkObject *self, SkObject *msg) {
    pthread_mutex_unlock((pthread_mutex_t *)sk_object_get_data(self));
    return self;
}
