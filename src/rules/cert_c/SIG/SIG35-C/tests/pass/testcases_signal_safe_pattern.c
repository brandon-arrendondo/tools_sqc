/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t termination_code = 0;

void signal_safe_pattern_handler(int sig) {
    /* Follow proper signal-safe termination pattern */

    /* Step 1: Record termination reason (async-signal-safe) */
    termination_code = sig;

    /* Step 2: Write minimal error message using async-safe functions */
    const char error_msg[] = "FATAL: Computational exception - terminating immediately\n";
    write(STDERR_FILENO, error_msg, sizeof(error_msg) - 1);

    /* Step 3: Perform only essential async-signal-safe operations */
    /* No file I/O, no malloc, no mutex operations, no non-reentrant functions */

    /* Step 4: Use appropriate termination function */
    switch (sig) {
        case SIGFPE:
        case SIGSEGV:
        case SIGILL:
        case SIGBUS:
            /* For computational exceptions, use abort() */
            abort();
            break;
        default:
            /* For other signals, use _Exit() */
            _Exit(EXIT_FAILURE);
            break;
    }

    /* COMPLIANT: All code paths lead to termination, never returns */
}

int main() {
    printf("Demonstrating proper signal-safe termination pattern\n");
    printf("PID: %d\n", getpid());

    /* Register the same safe handler for all computational exceptions */
    signal(SIGFPE, signal_safe_pattern_handler);
    signal(SIGSEGV, signal_safe_pattern_handler);
    signal(SIGILL, signal_safe_pattern_handler);
    signal(SIGBUS, signal_safe_pattern_handler);

    printf("Termination code: %d\n", termination_code);

    printf("Triggering bus error...\n");
    /* Create a scenario that might trigger SIGBUS */
    /* This is platform-specific and may not work on all systems */
    volatile int *misaligned_ptr = (volatile int *)((char *)&termination_code + 1);
    volatile int value = *misaligned_ptr;

    printf("If bus error didn't occur, trigger FPE instead...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;

    printf("This line should never be reached\n");
    return 0;
}