/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: threading_shared_flex.c
 *
 * This case demonstrates a violation of MEM33-C by improperly sharing
 * structures with flexible array members between threads without proper
 * synchronization and memory management consideration.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

/* VIOLATION: Global pointer to flexible array structure without proper handling */
struct flex_array_struct *shared_flex;

void *thread_function(void *arg) {
    int thread_id = *(int *)arg;

    /* VIOLATION: Accessing shared flexible array structure without synchronization */
    if (shared_flex != NULL) {
        printf("Thread %d accessing shared flex struct\n", thread_id);

        /* VIOLATION: Modifying flexible array data without protection */
        for (size_t i = 0; i < shared_flex->num; i++) {
            shared_flex->data[i] = thread_id * 100 + (int)i;  /* Race condition */
        }

        /* VIOLATION: Direct assignment in thread context */
        struct flex_array_struct local_copy = *shared_flex;  /* Only copies fixed members */

        printf("Thread %d copied num: %zu\n", thread_id, local_copy.num);
        if (local_copy.num > 0) {
            printf("Thread %d copied data[0]: %d (garbage)\n", thread_id, local_copy.data[0]);
        }
    }

    return NULL;
}

int main(void) {
    pthread_t threads[2];
    int thread_ids[2] = {1, 2};
    size_t array_size = 3;

    /* Allocate shared structure */
    shared_flex = malloc(sizeof(struct flex_array_struct) + sizeof(int) * array_size);
    if (shared_flex == NULL) return 1;

    shared_flex->num = array_size;
    for (size_t i = 0; i < array_size; i++) {
        shared_flex->data[i] = (int)i;
    }

    /* Create threads that will improperly access the flexible array structure */
    for (int i = 0; i < 2; i++) {
        pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]);
    }

    /* Wait for threads */
    for (int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Check final state (race conditions may have corrupted data) */
    printf("Final shared data: ");
    for (size_t i = 0; i < shared_flex->num; i++) {
        printf("%d ", shared_flex->data[i]);
    }
    printf("\n");

    free(shared_flex);
    return 0;
}