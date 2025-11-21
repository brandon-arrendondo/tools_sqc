/*
 * Rule: POS49-C
 * Source: wiki
 * Status: FAIL - Should trigger POS49-C violation
 */

#include <pthread.h>

struct flags {
    unsigned int flag1 : 1;
    unsigned int flag2 : 1;
};

struct flags global_flags;

void* thread1_func(void* arg) {
    // VIOLATION: Access bitfield without mutex
    global_flags.flag1 = 1;
    return NULL;
}

void* thread2_func(void* arg) {
    // VIOLATION: Access bitfield without mutex
    global_flags.flag2 = 1;
    return NULL;
}

int main(void) {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread1_func, NULL);
    pthread_create(&t2, NULL, thread2_func, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}