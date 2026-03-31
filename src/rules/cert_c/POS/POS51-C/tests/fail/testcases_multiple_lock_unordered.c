/*
 * Rule: POS51-C
 * Source: testcases
 * Status: FAIL - Should trigger POS51-C violation
 *
 * Multiple pthread_mutex_lock without predefined ordering
 */

#include <pthread.h>
#include <stdlib.h>

typedef struct {
    int balance;
    pthread_mutex_t balance_mutex;
} account;

void *transfer(void *ptr) {
    account *from = ((account **)ptr)[0];
    account *to = ((account **)ptr)[1];

    /* VIOLATION: locks in arbitrary order without ordering guarantee */
    if (pthread_mutex_lock(&from->balance_mutex) != 0) {
        return NULL;
    }
    if (pthread_mutex_lock(&to->balance_mutex) != 0) {
        return NULL;
    }

    from->balance -= 100;
    to->balance += 100;

    pthread_mutex_unlock(&from->balance_mutex);
    pthread_mutex_unlock(&to->balance_mutex);

    return NULL;
}
