/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int shared_counter = 0;
static char shared_buffer[256];

void unsafe_handler(int sig) {
    /* Violation: Accessing static variables in signal handler */
    shared_counter++;
    sprintf(shared_buffer, "Signal %d count: %d", sig, shared_counter);
    printf("Handler: %s\n", shared_buffer);
}

int main() {
    printf("Demonstrating unsafe static variable access in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, unsafe_handler);

    for (int i = 0; i < 50; i++) {
        shared_counter = i * 2;
        sprintf(shared_buffer, "Main iteration %d", i);
        printf("Main: %s (counter=%d)\n", shared_buffer, shared_counter);
        usleep(50000);
    }

    return 0;
}