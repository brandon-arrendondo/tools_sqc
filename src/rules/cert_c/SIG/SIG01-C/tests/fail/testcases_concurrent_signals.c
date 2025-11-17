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

volatile sig_atomic_t signal_received = 0;

void concurrent_handler(int sig) {
    signal_received++;
    printf("Signal %d received by thread\n", signal_received);
}

void* signal_sender(void* arg) {
    int i;
    for (i = 0; i < 5; i++) {
        sleep(1);
        raise(SIGUSR1);
        printf("Signal %d sent\n", i + 1);
    }
    return NULL;
}

int main() {
    printf("FAIL: Concurrent signals assuming handler persistence\n");

    signal(SIGUSR1, concurrent_handler);

    printf("PID: %d\n", getpid());
    printf("Testing concurrent signal delivery\n");

    pthread_t sender_thread;
    if (pthread_create(&sender_thread, NULL, signal_sender, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    /* Main thread waits for signals */
    while (signal_received < 5) {
        pause();
    }

    pthread_join(sender_thread, NULL);

    printf("Signals received: %d\n", signal_received);
    printf("Assumes handler persists across concurrent access\n");

    return 0;
}