/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void implementation_defined_handler(int sig) {
    const char *signal_name = "UNKNOWN";

    switch(sig) {
        case SIGFPE:
            signal_name = "SIGFPE";
            break;
        case SIGSEGV:
            signal_name = "SIGSEGV";
            break;
        case SIGILL:
            signal_name = "SIGILL";
            break;
        case SIGBUS:
            signal_name = "SIGBUS";
            break;
#ifdef SIGSYS
        case SIGSYS:
            signal_name = "SIGSYS (bad system call)";
            break;
#endif
#ifdef SIGEMT
        case SIGEMT:
            signal_name = "SIGEMT (emulator trap)";
            break;
#endif
    }

    printf("Implementation-defined exception handler: %s (%d)\n", signal_name, sig);
    printf("Handling implementation-specific computational exception\n");

    /* This handler attempts to return from any computational exception */
    printf("Attempting to continue from %s (violates SIG35-C)\n", signal_name);
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing implementation-defined computational exception handling\n");
    printf("PID: %d\n", getpid());

    /* Register handler for multiple computational exceptions */
    signal(SIGFPE, implementation_defined_handler);
    signal(SIGSEGV, implementation_defined_handler);
    signal(SIGILL, implementation_defined_handler);
    signal(SIGBUS, implementation_defined_handler);
#ifdef SIGSYS
    signal(SIGSYS, implementation_defined_handler);
#endif
#ifdef SIGEMT
    signal(SIGEMT, implementation_defined_handler);
#endif

    printf("Triggering floating point exception...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;

    printf("This represents undefined behavior\n");
    return 0;
}