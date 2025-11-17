/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t cleanup_flag = 0;
volatile sig_atomic_t resource_count = 0;

void safe_cleanup_handler(int sig) {
    /* Set cleanup flag using async-signal-safe assignment */
    cleanup_flag = 1;

    const char msg1[] = "COMPUTATIONAL EXCEPTION: Performing safe cleanup\n";
    const char msg2[] = "Cleanup operations limited to async-signal-safe functions\n";
    const char msg3[] = "Resource cleanup completed - terminating\n";

    write(STDERR_FILENO, msg1, sizeof(msg1) - 1);

    /* Only perform async-signal-safe cleanup operations */
    /* Reset signal-atomic counters */
    resource_count = 0;

    write(STDERR_FILENO, msg2, sizeof(msg2) - 1);

    /* Minimal cleanup completed */
    write(STDERR_FILENO, msg3, sizeof(msg3) - 1);

    abort(); /* COMPLIANT: Terminate after safe cleanup, never returns */
}

int main() {
    printf("Demonstrating safe cleanup before termination\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, safe_cleanup_handler);

    /* Initialize some resources */
    resource_count = 5;

    printf("Resource count: %d\n", resource_count);
    printf("Cleanup flag: %d\n", cleanup_flag);

    printf("Triggering floating point exception...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;

    printf("This line should never be reached\n");
    return 0;
}