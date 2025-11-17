/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t exception_count = 0;
volatile sig_atomic_t max_exceptions = 3;

void conditional_exception_handler(int sig) {
    exception_count++;
    printf("Exception #%d: Signal %d received\n", exception_count, sig);

    if (exception_count < max_exceptions) {
        printf("Exception count below threshold, returning (violates SIG35-C)\n");
        return; /* VIOLATION: conditional return from computational exception handler */
    } else {
        printf("Too many exceptions, would abort but still violates rule\n");
        /* Even this path violates SIG35-C because the handler CAN return */
        abort();
    }
}

int main() {
    printf("Testing conditional return from exception handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, conditional_exception_handler);

    printf("Triggering multiple exceptions...\n");

    for (int i = 0; i < 5; i++) {
        printf("Attempt %d: Division by zero\n", i + 1);
        volatile int zero = 0;
        volatile int result = 1 / zero;
        printf("Undefined behavior if this prints\n");
    }

    return 0;
}