/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t global_semaphore;

void thread_handler(int sig) {
    // VIOLATION: pthread_mutex_lock/unlock are not async-safe
    if (pthread_mutex_lock(&global_mutex) == 0) {
        // Critical section
        static int counter = 0;
        counter++;
        pthread_mutex_unlock(&global_mutex);
    }

    // VIOLATION: semaphore operations are not async-safe
    sem_wait(&global_semaphore);
    // Do work
    sem_post(&global_semaphore);

    // VIOLATION: pthread_cond_signal is not async-safe
    static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    pthread_cond_signal(&cond);
}

int main() {
    printf("Demonstrating unsafe thread synchronization in signal handler\n");
    printf("PID: %d\n", getpid());

    sem_init(&global_semaphore, 0, 1);
    signal(SIGUSR1, thread_handler);

    printf("Send SIGUSR1 to trigger unsafe thread operations\n");

    while (1) {
        pause();
    }

    return 0;
}