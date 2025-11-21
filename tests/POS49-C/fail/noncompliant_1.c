// POS49-C: Noncompliant - access bitfields from threads without mutex
#include <pthread.h>

struct flags {
    unsigned int flag1 : 1;
    unsigned int flag2 : 1;
};

struct flags shared_flags;

void* thread_func(void* arg) {
    // VIOLATION: Access bitfield without mutex protection
    shared_flags.flag1 = 1;
    return NULL;
}

void test_pos49c_fail() {
    pthread_t thread;
    pthread_create(&thread, NULL, thread_func, NULL);
    shared_flags.flag2 = 1;  // VIOLATION: Concurrent access
    pthread_join(thread, NULL);
}
