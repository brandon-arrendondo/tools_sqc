/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t program_state = 0;
volatile sig_atomic_t error_location = 0;
volatile sig_atomic_t last_operation = 0;

void state_saving_handler(int sig) {
    const char msg1[] = "COMPUTATIONAL EXCEPTION: Saving program state\n";
    const char msg2[] = "State saved using async-signal-safe operations\n";
    const char msg3[] = "Terminating after state preservation\n";

    write(STDERR_FILENO, msg1, sizeof(msg1) - 1);

    /* Save critical program state using only async-signal-safe operations */
    error_location = sig;         /* Record which signal occurred */
    program_state = 99;           /* Mark as terminated state */
    last_operation = 1;           /* Mark last operation as signal handling */

    write(STDERR_FILENO, msg2, sizeof(msg2) - 1);

    /* State preservation complete */
    write(STDERR_FILENO, msg3, sizeof(msg3) - 1);

    abort(); /* COMPLIANT: Terminate after saving state, never returns */
}

int main() {
    printf("Demonstrating state preservation before termination\n");
    printf("PID: %d\n", getpid());

    signal(SIGSEGV, state_saving_handler);
    signal(SIGFPE, state_saving_handler);

    /* Initialize program state */
    program_state = 1;    /* Running state */
    error_location = 0;   /* No error yet */
    last_operation = 0;   /* No operations yet */

    printf("Initial state: program=%d, error=%d, operation=%d\n",
           program_state, error_location, last_operation);

    printf("Performing operation that will cause segmentation fault...\n");
    last_operation = 5;   /* Mark current operation */

    volatile int *null_ptr = NULL;
    volatile int value = *null_ptr;

    printf("This line should never be reached\n");
    return 0;
}