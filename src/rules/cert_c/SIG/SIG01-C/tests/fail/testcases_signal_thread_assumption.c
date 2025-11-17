/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

volatile sig_atomic_t thread_signals = 0;

void thread_handler(int sig) {
    thread_signals++;
    printf("Thread signal %d in thread %lu\n", thread_signals, pthread_self());
}

void* worker_thread(void* arg) {
    printf("Worker thread %lu started\n", pthread_self());

    /* Assumes signal handler works consistently in threads */
    sleep(2);

    /* Send signal from thread */
    raise(SIGUSR1);

    printf("Worker thread %lu signaled\n", pthread_self());
    return NULL;
}

int main() {
    printf("FAIL: Signal handling in multithreaded context assumption\n");

    signal(SIGUSR1, thread_handler);

    printf("Main thread PID: %d, TID: %lu\n", getpid(), pthread_self());

    /* Create worker threads */
    pthread_t threads[3];
    int i;
    for (i = 0; i < 3; i++) {
        if (pthread_create(&threads[i], NULL, worker_thread, NULL) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    /* Wait for threads */
    for (i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    sleep(1);

    printf("Thread signals received: %d\n", thread_signals);
    printf("Assumes consistent signal behavior across threads\n");

    return 0;
}