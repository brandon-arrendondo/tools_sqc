/*
 * Rule: POS50-C
 * Source: testcases
 * Status: PASS - Safe data sharing with pthread_create
 */

#include <pthread.h>
#include <stdlib.h>

void *thread_func(void *arg) {
    return arg;
}

/* Global variable — not local, safe */
int g_data = 42;
void global_to_pthread(void) {
    pthread_t t;
    pthread_create(&t, NULL, thread_func, &g_data);
    pthread_join(t, NULL);
}

/* NULL to pthread — safe */
void null_to_pthread(void) {
    pthread_t t;
    pthread_create(&t, NULL, thread_func, NULL);
    pthread_join(t, NULL);
}

/* Heap pointer directly (no &) — safe */
void heap_to_pthread(void) {
    int *x = (int *)malloc(sizeof(int));
    *x = 42;
    pthread_t t;
    pthread_create(&t, NULL, thread_func, x);
    pthread_join(t, NULL);
    free(x);
}
