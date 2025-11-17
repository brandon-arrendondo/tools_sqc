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

void dangerous_exception_handler(int sig) {
    exception_count++;
    printf("Computational exception %d caught (count: %d)\n", sig, exception_count);

    printf("WARNING: Returning from computational exception handler!\n");
    printf("This causes undefined behavior according to C standard\n");

}

void trigger_fpe() {
    volatile int zero = 0;
    volatile int result = 1 / zero;
    printf("Result: %d\n", result);
}

void trigger_segv() {
    volatile int *null_ptr = NULL;
    volatile int value = *null_ptr;
    printf("Value: %d\n", value);
}

int main() {
    printf("Demonstrating dangerous return from computational exception handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, dangerous_exception_handler);
    signal(SIGSEGV, dangerous_exception_handler);
    signal(SIGILL, dangerous_exception_handler);

    printf("Triggering floating point exception...\n");
    trigger_fpe();

    printf("This line should not be reached due to undefined behavior\n");

    printf("Triggering segmentation fault...\n");
    trigger_segv();

    printf("Program should have terminated by now\n");
    return 0;
}