/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t nesting_level = 0;

void nested_handler(int sig);
void alternate_handler(int sig);

void nested_handler(int sig) {
    nesting_level++;
    printf("Nested handler called (level %d) for signal %d\n", nesting_level, sig);

    if (nesting_level < 3) {
        // VIOLATION: Nested signal() calls within handlers
        if (signal(sig, alternate_handler) == SIG_ERR) {
            printf("Failed to register alternate handler\n");
        }
        printf("Switched to alternate handler at level %d\n", nesting_level);
    }

    nesting_level--;
}

void alternate_handler(int sig) {
    nesting_level++;
    printf("Alternate handler called (level %d) for signal %d\n", nesting_level, sig);

    if (nesting_level < 3) {
        // VIOLATION: Nested signal() calls within handlers
        if (signal(sig, nested_handler) == SIG_ERR) {
            printf("Failed to register nested handler\n");
        }
        printf("Switched back to nested handler at level %d\n", nesting_level);
    }

    nesting_level--;
}

int main() {
    printf("SIG34-C VIOLATION: Nested signal() calls in handlers\n");
    printf("Handlers switch between each other using signal()\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGTERM, nested_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGTERM to trigger nested signal() calls\n");

    for (int i = 0; i < 10; i++) {
        sleep(1);
    }

    return 0;
}