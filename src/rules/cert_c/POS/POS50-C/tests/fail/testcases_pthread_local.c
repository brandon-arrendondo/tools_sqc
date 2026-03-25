/*
 * Rule: POS50-C
 * Source: testcases
 * Status: FAIL - Passing local storage to pthread_create
 */

#include <pthread.h>

void *thread_func(void *arg) {
    return arg;
}

/* Pass address of local int to pthread_create */
void local_int_to_pthread(void) {
    int x = 42;
    pthread_t t;
    pthread_create(&t, NULL, thread_func, &x);
    pthread_join(t, NULL);
}

/* Pass local array to pthread_create */
void local_array_to_pthread(void) {
    int data[10];
    data[0] = 1;
    pthread_t t;
    pthread_create(&t, NULL, thread_func, data);
    pthread_join(t, NULL);
}
