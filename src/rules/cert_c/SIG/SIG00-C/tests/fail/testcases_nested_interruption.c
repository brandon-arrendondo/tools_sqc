/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t depth = 0;
volatile sig_atomic_t max_depth = 0;

void nested_handler(int sig) {
    depth++;
    if (depth > max_depth) {
        max_depth = depth;
    }

    printf("Handler depth: %d (signal %d)\n", depth, sig);

    sleep(1);

    depth--;
    printf("Handler exiting, depth now: %d\n", depth);
}

int main() {
    signal(SIGUSR1, nested_handler);
    signal(SIGUSR2, nested_handler);

    printf("PID: %d\n", getpid());
    printf("Send signals rapidly to create nested interruptions\n");

    while (max_depth < 3) {
        pause();
    }

    printf("Maximum nesting depth reached: %d\n", max_depth);
    return 0;
}