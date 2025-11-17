/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t recursion_depth = 0;
volatile sig_atomic_t max_depth = 0;

void recursive_signal_handler(int sig) {
    recursion_depth++;
    if (recursion_depth > max_depth) {
        max_depth = recursion_depth;
    }

    printf("Recursive handler depth %d for signal %d (max: %d)\n", recursion_depth, sig, max_depth);

    if (recursion_depth < 5) {
        // VIOLATION: Recursive signal() calls in nested handlers
        printf("Depth %d: Making recursive signal() call\n", recursion_depth);

        if (recursion_depth % 2 == 1) {
            // Odd depth: register for different signal
            if (signal(SIGUSR2, recursive_signal_handler) == SIG_ERR) {
                printf("Failed to register SIGUSR2 at depth %d\n", recursion_depth);
            } else {
                printf("Registered SIGUSR2 at depth %d\n", recursion_depth);
            }
        } else {
            // Even depth: re-register current signal
            if (signal(sig, recursive_signal_handler) == SIG_ERR) {
                printf("Failed to re-register signal %d at depth %d\n", sig, recursion_depth);
            } else {
                printf("Re-registered signal %d at depth %d\n", sig, recursion_depth);
            }
        }

        // Simulate nested signal delivery
        if (recursion_depth < 3) {
            printf("Depth %d: Triggering nested signal\n", recursion_depth);
            raise(sig); // This can cause recursive calls
        }
    } else {
        printf("Maximum recursion depth reached, stopping\n");
    }

    recursion_depth--;
    printf("Exiting handler at depth %d\n", recursion_depth + 1);
}

int main() {
    printf("SIG34-C VIOLATION: Recursive signal() calls in nested handlers\n");
    printf("Handler makes signal() calls that can trigger recursive invocations\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, recursive_signal_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 to trigger recursive signal() calls\n");

    // Trigger initial signal
    raise(SIGUSR1);

    sleep(2); // Allow recursive calls to complete

    printf("Maximum recursion depth reached: %d\n", max_depth);
    return 0;
}