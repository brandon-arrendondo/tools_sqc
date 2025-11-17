/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t terminate_flag = 0;

void safe_infinite_loop_handler(int sig) {
    printf("COMPUTATIONAL EXCEPTION: Signal %d received\n", sig);
    printf("Entering infinite loop - will never return\n");

    terminate_flag = 1;

    /* COMPLIANT: Infinite loop ensures handler never returns */
    while (1) {
        /* Check for termination condition periodically */
        if (terminate_flag) {
            /* Could perform minimal cleanup here */
            printf("Termination requested, calling abort()\n");
            abort();
        }

        /* Small delay to prevent excessive CPU usage */
        sleep(1);
    }

    /* This line is never reached */
}

int main() {
    printf("Demonstrating safe infinite loop termination from computational exceptions\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, safe_infinite_loop_handler);
    signal(SIGSEGV, safe_infinite_loop_handler);

    printf("Exception handlers will enter infinite loops (never return)\n");

    printf("Triggering segmentation fault...\n");
    volatile int *null_ptr = NULL;
    volatile int value = *null_ptr;

    printf("This line should never be reached\n");
    return 0;
}