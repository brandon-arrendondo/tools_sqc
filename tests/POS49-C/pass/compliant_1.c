// POS49-C: Compliant - protect bitfield access with mutex
#include <pthread.h>

struct flags {
    unsigned int flag1 : 1;
    unsigned int flag2 : 1;
};

struct flags shared_flags;
pthread_mutex_t flags_mutex = PTHREAD_MUTEX_INITIALIZER;

void* thread_func(void* arg) {
    // OK: Use mutex to protect bitfield access
    pthread_mutex_lock(&flags_mutex);
    shared_flags.flag1 = 1;
    pthread_mutex_unlock(&flags_mutex);
    return NULL;
}

void test_pos49c_pass() {
    pthread_t thread;
    pthread_create(&thread, NULL, thread_func, NULL);
    pthread_mutex_lock(&flags_mutex);
    shared_flags.flag2 = 1;  // OK: Protected by mutex
    pthread_mutex_unlock(&flags_mutex);
    pthread_join(thread, NULL);
}
