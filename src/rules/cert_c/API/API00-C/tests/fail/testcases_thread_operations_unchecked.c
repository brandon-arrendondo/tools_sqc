/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: thread_operations_unchecked.c
 *
 * This case demonstrates violations where thread-related functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

/* NON-COMPLIANT: No validation of thread ID pointer */
void create_worker_thread(pthread_t *thread_id, void *(*start_routine)(void *), void *arg) {
    /* No validation of thread_id or start_routine */
    pthread_create(thread_id, NULL, start_routine, arg);  /* thread_id could be NULL */
}

/* NON-COMPLIANT: No validation of mutex pointer */
void lock_mutex(pthread_mutex_t *mutex) {
    /* No validation of mutex */
    pthread_mutex_lock(mutex);  /* mutex could be NULL or uninitialized */
}

/* NON-COMPLIANT: No validation of condition variable */
void wait_on_condition(pthread_cond_t *cond, pthread_mutex_t *mutex) {
    /* No validation of condition or mutex */
    pthread_cond_wait(cond, mutex);  /* Either could be NULL */
}

/* NON-COMPLIANT: No validation of thread attributes */
void create_detached_thread(pthread_t *thread_id, pthread_attr_t *attr,
                           void *(*start_routine)(void *), void *arg) {
    /* No validation of attr */
    pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED);  /* attr could be NULL */
    pthread_create(thread_id, attr, start_routine, arg);
}

/* NON-COMPLIANT: No validation of thread-specific data key */
void set_thread_data(pthread_key_t key, void *data) {
    /* No validation of key validity */
    pthread_setspecific(key, data);  /* key might be invalid */
}

/* NON-COMPLIANT: No validation of barrier parameters */
void wait_at_barrier(pthread_barrier_t *barrier) {
    /* No validation of barrier */
    pthread_barrier_wait(barrier);  /* barrier could be NULL or uninitialized */
}

/* NON-COMPLIANT: No validation of rwlock pointer */
void read_lock(pthread_rwlock_t *rwlock) {
    /* No validation of rwlock */
    pthread_rwlock_rdlock(rwlock);  /* rwlock could be NULL */
}

/* NON-COMPLIANT: No validation of timeout parameter */
void timed_lock_mutex(pthread_mutex_t *mutex, struct timespec *timeout) {
    /* No validation of timeout */
    pthread_mutex_timedlock(mutex, timeout);  /* timeout could be NULL or invalid */
}

/* NON-COMPLIANT: No validation of spin lock */
void spin_lock_acquire(pthread_spinlock_t *lock) {
    /* No validation of lock */
    pthread_spin_lock(lock);  /* lock could be NULL or uninitialized */
}

/* NON-COMPLIANT: No validation of thread cancellation parameters */
void set_thread_cancellation(int state, int type) {
    /* No validation of state or type values */
    pthread_setcancelstate(state, NULL);  /* state could be invalid */
    pthread_setcanceltype(type, NULL);  /* type could be invalid */
}

int main(void) {
    pthread_t *null_thread = NULL;
    pthread_mutex_t *null_mutex = NULL;
    pthread_cond_t *null_cond = NULL;

    /* Examples of dangerous thread operations */
    // create_worker_thread(null_thread, NULL, NULL);  /* NULL parameters */
    // lock_mutex(null_mutex);  /* NULL mutex */
    // wait_on_condition(null_cond, null_mutex);  /* NULL parameters */
    // set_thread_data((pthread_key_t)-1, NULL);  /* Invalid key */
    // wait_at_barrier(NULL);  /* NULL barrier */
    // read_lock(NULL);  /* NULL rwlock */
    // timed_lock_mutex(null_mutex, NULL);  /* NULL parameters */
    // spin_lock_acquire(NULL);  /* NULL spinlock */
    // set_thread_cancellation(999, 888);  /* Invalid values */

    printf("Thread functions compiled but lack parameter validation\n");
    return 0;
}