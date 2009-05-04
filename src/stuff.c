#include "stuff.h"

int sk_printf(const char *format, ...) {
    static pthread_mutex_t stdout_mutex = PTHREAD_MUTEX_INITIALIZER;
    int ret;
    va_list args;
    va_start(args, format);
    pthread_mutex_lock(&stdout_mutex);
    ret = vprintf(format, args);
    pthread_mutex_unlock(&stdout_mutex);
    va_end(args);
    return ret;
}
