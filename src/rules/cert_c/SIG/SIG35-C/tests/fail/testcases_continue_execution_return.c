/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t execution_mode = 1; /* 1 = normal, 0 = recovery */

void continue_execution_handler(int sig) {
    printf("Exception handler: Attempting to continue normal execution\n");

    /* Switch to recovery mode */
    execution_mode = 0;

    printf("Switched to recovery mode, continuing execution (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

void risky_operation() {
    if (execution_mode == 1) {
        printf("Performing risky division...\n");
        volatile int zero = 0;
        volatile int result = 1 / zero;
        printf("Result: %d\n", result);
    } else {
        printf("In recovery mode, skipping risky operation\n");
    }
}

int main() {
    printf("Testing attempt to continue execution after exception\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, continue_execution_handler);

    printf("Execution mode: %d\n", execution_mode);
    risky_operation();

    printf("Execution mode after exception: %d\n", execution_mode);
    printf("This output represents undefined behavior\n");

    return 0;
}