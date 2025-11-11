/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t global_state = 0;
volatile sig_atomic_t error_code = 0;
volatile sig_atomic_t system_status = 1; /* 1 = running, 0 = error */

void global_state_handler(int sig) {
    printf("Exception handler: Modifying global state\n");

    /* Modify various global state variables */
    global_state = sig;
    error_code = 100 + sig;
    system_status = 0; /* Set to error state */

    printf("Global state updated: state=%d, error=%d, status=%d\n",
           global_state, error_code, system_status);

    printf("State modification complete, returning (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing global state modification with return\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, global_state_handler);

    printf("Initial state: state=%d, error=%d, status=%d\n",
           global_state, error_code, system_status);

    printf("Triggering floating point exception...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;

    printf("Final state: state=%d, error=%d, status=%d\n",
           global_state, error_code, system_status);
    printf("This represents undefined behavior\n");

    return 0;
}