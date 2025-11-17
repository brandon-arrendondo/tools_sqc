/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    volatile sig_atomic_t exception_type;
    volatile sig_atomic_t severity_level;
    volatile sig_atomic_t recovery_strategy;
    volatile sig_atomic_t attempt_count;
} exception_context_t;

exception_context_t exc_context = {0, 0, 0, 0};

void complex_logic_handler(int sig) {
    printf("Exception handler: Executing complex logic\n");

    /* Complex decision tree */
    exc_context.exception_type = sig;
    exc_context.attempt_count++;

    if (sig == SIGFPE) {
        exc_context.severity_level = 3;
        exc_context.recovery_strategy = 1;
        printf("FPE detected: High severity, strategy 1\n");
    } else if (sig == SIGSEGV) {
        exc_context.severity_level = 5;
        exc_context.recovery_strategy = 2;
        printf("SEGV detected: Critical severity, strategy 2\n");
    } else {
        exc_context.severity_level = 1;
        exc_context.recovery_strategy = 0;
        printf("Other exception: Low severity, no strategy\n");
    }

    /* Complex recovery logic */
    if (exc_context.severity_level > 2 && exc_context.attempt_count < 3) {
        printf("Attempting recovery: severity=%d, attempts=%d\n",
               exc_context.severity_level, exc_context.attempt_count);

        /* Simulate complex recovery procedure */
        for (volatile int i = 0; i < exc_context.recovery_strategy * 1000; i++) {
            /* Busy wait simulation */
        }

        printf("Recovery procedure completed\n");
    }

    printf("Complex logic complete, returning (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing complex logic in exception handler with return\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, complex_logic_handler);
    signal(SIGSEGV, complex_logic_handler);

    printf("Initial context: type=%d, severity=%d, strategy=%d, attempts=%d\n",
           exc_context.exception_type, exc_context.severity_level,
           exc_context.recovery_strategy, exc_context.attempt_count);

    printf("Triggering floating point exception...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;

    printf("Final context: type=%d, severity=%d, strategy=%d, attempts=%d\n",
           exc_context.exception_type, exc_context.severity_level,
           exc_context.recovery_strategy, exc_context.attempt_count);
    printf("This represents undefined behavior\n");

    return 0;
}