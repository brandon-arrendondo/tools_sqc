/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t exception_mask = 0;

void multi_exception_handler(int sig) {
    switch(sig) {
        case SIGFPE:
            exception_mask |= 0x01;
            printf("SIGFPE handled, continuing (violates SIG35-C)\n");
            break;
        case SIGSEGV:
            exception_mask |= 0x02;
            printf("SIGSEGV handled, continuing (violates SIG35-C)\n");
            break;
        case SIGILL:
            exception_mask |= 0x04;
            printf("SIGILL handled, continuing (violates SIG35-C)\n");
            break;
    }
    printf("Exception mask: 0x%02x\n", exception_mask);
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing multiple exception handlers that return\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, multi_exception_handler);
    signal(SIGSEGV, multi_exception_handler);
    signal(SIGILL, multi_exception_handler);

    printf("Triggering FPE...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;

    printf("Undefined behavior in multi-exception scenario\n");
    return 0;
}