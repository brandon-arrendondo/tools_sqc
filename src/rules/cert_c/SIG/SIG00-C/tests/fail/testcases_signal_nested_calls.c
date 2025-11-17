/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t nested_level = 0;
volatile sig_atomic_t max_nesting = 0;

void nested_handler(int sig) {
    nested_level++;

    if (nested_level > max_nesting) {
        max_nesting = nested_level;
    }

    printf("Handler: Signal %d, nesting level %d\n", sig, nested_level);

    if (nested_level > 5) {
        printf("Handler: DANGER - Deep nesting detected!\n");
        exit(1);
    }

    // Violation: Handler can be interrupted by same or different signals
    // without proper masking, leading to unbounded nesting
    for (int i = 0; i < 3; i++) {
        printf("Handler: Processing step %d at level %d\n", i + 1, nested_level);
        sleep(1); // Large vulnerability window
    }

    printf("Handler: Exiting level %d\n", nested_level);
    nested_level--;
}

int main() {
    struct sigaction sa;

    // Install handler without masking
    sa.sa_handler = nested_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: No masking allows unlimited signal nesting
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Send multiple signals rapidly to cause deep nesting\n");
    printf("Program will exit if nesting exceeds 5 levels\n");

    while (1) {
        printf("Main: Current nesting = %d, max seen = %d\n",
               nested_level, max_nesting);

        if (max_nesting >= 3) {
            printf("Main: WARNING - Dangerous nesting levels detected!\n");
        }

        sleep(2);
    }

    return 0;
}