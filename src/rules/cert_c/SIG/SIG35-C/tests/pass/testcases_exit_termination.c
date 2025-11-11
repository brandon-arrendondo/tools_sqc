/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void safe_exit_handler(int sig) {
    const char *signal_name;
    switch (sig) {
        case SIGFPE:
            signal_name = "SIGFPE (Floating Point Exception)";
            break;
        case SIGSEGV:
            signal_name = "SIGSEGV (Segmentation Fault)";
            break;
        case SIGILL:
            signal_name = "SIGILL (Illegal Instruction)";
            break;
        default:
            signal_name = "Unknown computational exception";
            break;
    }

    printf("FATAL ERROR: %s detected\n", signal_name);
    printf("Terminating program safely with _Exit()\n");

    _Exit(EXIT_FAILURE); /* COMPLIANT: Never returns from computational exception handler */
}

void trigger_exception() {
    printf("Triggering division by zero...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;
    printf("Result: %d (this should not print)\n", result);
}

int main() {
    printf("Demonstrating safe termination with _Exit() from computational exceptions\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, safe_exit_handler);
    signal(SIGSEGV, safe_exit_handler);
    signal(SIGILL, safe_exit_handler);

    printf("Exception handlers will terminate program safely with _Exit()\n");

    trigger_exception();

    printf("This line should never be reached\n");
    return 0;
}