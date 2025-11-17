/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void cleanup_function1(void) {
    printf("Quick exit cleanup function 1 called\n");
}

void cleanup_function2(void) {
    printf("Quick exit cleanup function 2 called\n");
}

void safe_quick_exit_handler(int sig) {
    printf("COMPUTATIONAL EXCEPTION: Signal %d received\n", sig);
    printf("Performing safe termination with quick_exit()\n");

    /* quick_exit() will call registered cleanup functions */
    quick_exit(EXIT_FAILURE); /* COMPLIANT: Never returns from computational exception handler */
}

int main() {
    printf("Demonstrating safe termination with quick_exit() from computational exceptions\n");
    printf("PID: %d\n", getpid());

    /* Register cleanup functions for quick_exit */
    if (at_quick_exit(cleanup_function1) != 0) {
        printf("Failed to register cleanup function 1\n");
    }
    if (at_quick_exit(cleanup_function2) != 0) {
        printf("Failed to register cleanup function 2\n");
    }

    signal(SIGFPE, safe_quick_exit_handler);
    signal(SIGSEGV, safe_quick_exit_handler);

    printf("Triggering floating point exception...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;

    printf("This line should never be reached\n");
    return 0;
}