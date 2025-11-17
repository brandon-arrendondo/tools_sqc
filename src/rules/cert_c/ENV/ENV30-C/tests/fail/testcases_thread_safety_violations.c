/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: thread_safety_violations.c
 *
 * This case demonstrates violations in multithreaded contexts.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

/* Global variables for thread communication */
static char *shared_env_ptr = NULL;
static char *shared_time_ptr = NULL;

/* NON-COMPLIANT: Thread modifying shared return value */
void *unsafe_thread_function(void *arg) {
    int thread_id = *(int*)arg;

    /* Get environment variable */
    char *env_value = getenv("SHARED_VAR");
    shared_env_ptr = env_value;

    if (env_value != NULL) {
        /* VIOLATION: Modifying in thread context */
        env_value[0] = '0' + thread_id;  /* Undefined behavior */
        printf("Thread %d modified env: %s\n", thread_id, env_value);
    }

    return NULL;
}

/* NON-COMPLIANT: Multiple threads accessing time functions */
void *unsafe_time_thread(void *arg) {
    int thread_id = *(int*)arg;
    time_t now = time(NULL);

    /* Get time string */
    char *time_str = ctime(&now);
    shared_time_ptr = time_str;

    if (time_str != NULL) {
        /* VIOLATION: Modifying time string in thread */
        time_str[0] = 'T';
        time_str[1] = '0' + thread_id;  /* Undefined behavior */
        printf("Thread %d modified time: %s", thread_id, time_str);
    }

    return NULL;
}

void unsafe_multithreaded_modification(void) {
    pthread_t threads[3];
    int thread_ids[3] = {1, 2, 3};

    /* Set up environment variable */
    setenv("SHARED_VAR", "original", 1);

    /* Create threads that modify return values */
    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, unsafe_thread_function, &thread_ids[i]);
    }

    /* Wait for threads */
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Final shared env pointer: %s\n", shared_env_ptr ?: "(null)");
}

void unsafe_multithreaded_time(void) {
    pthread_t threads[2];
    int thread_ids[2] = {1, 2};

    /* Create threads that modify time strings */
    for (int i = 0; i < 2; i++) {
        pthread_create(&threads[i], NULL, unsafe_time_thread, &thread_ids[i]);
    }

    /* Wait for threads */
    for (int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Final shared time pointer: %s", shared_time_ptr ?: "(null)\n");
}

int main(void) {
    printf("=== ENV30-C Thread Safety Violations ===\n");

    printf("\n1. Unsafe multithreaded modification:\n");
    unsafe_multithreaded_modification();

    printf("\n2. Unsafe multithreaded time:\n");
    unsafe_multithreaded_time();

    return 0;
}