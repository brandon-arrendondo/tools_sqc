/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t shared_mutex = PTHREAD_MUTEX_INITIALIZER;
volatile sig_atomic_t resource_value = 0;

void mutex_handler(int sig) {
    printf("Handler: Signal %d attempting to acquire mutex\n", sig);

    // Violation: Attempting to acquire mutex in signal handler
    // without proper signal masking can cause deadlock
    int result = pthread_mutex_trylock(&shared_mutex);

    if (result == 0) {
        printf("Handler: Mutex acquired, modifying resource\n");

        resource_value++;
        printf("Handler: Resource value = %d\n", resource_value);

        // Hold mutex for extended time
        sleep(2);

        pthread_mutex_unlock(&shared_mutex);
        printf("Handler: Mutex released\n");
    } else {
        printf("Handler: Failed to acquire mutex (deadlock avoided)\n");

        // Try to access resource without mutex (race condition)
        resource_value++;
        printf("Handler: Unsafe access, resource = %d\n", resource_value);
    }
}

int main() {
    struct sigaction sa;

    // Install handler without masking
    sa.sa_handler = mutex_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: Signal can interrupt mutex operations causing deadlock
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Send SIGUSR1 while main thread holds mutex to cause deadlock\n");

    while (1) {
        printf("Main: Acquiring mutex...\n");
        pthread_mutex_lock(&shared_mutex);

        printf("Main: Mutex acquired, working with resource\n");
        resource_value += 10;

        // Hold mutex while vulnerable to signals
        for (int i = 0; i < 5; i++) {
            printf("Main: Working... resource = %d\n", resource_value);
            sleep(1); // Signal can arrive here
        }

        pthread_mutex_unlock(&shared_mutex);
        printf("Main: Mutex released\n\n");

        sleep(1);
    }

    return 0;
}