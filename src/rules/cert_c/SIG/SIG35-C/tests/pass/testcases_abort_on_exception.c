/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void safe_exception_handler(int sig) {
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
    printf("Terminating program safely with abort()\n");

    abort();
}

void trigger_exception() {
    printf("Triggering division by zero...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;
    printf("Result: %d (this should not print)\n", result);
}

int main() {
    printf("Demonstrating safe handling of computational exceptions\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, safe_exception_handler);
    signal(SIGSEGV, safe_exception_handler);
    signal(SIGILL, safe_exception_handler);

    printf("Exception handlers will terminate program safely\n");

    trigger_exception();

    printf("This line should never be reached\n");
    return 0;
}