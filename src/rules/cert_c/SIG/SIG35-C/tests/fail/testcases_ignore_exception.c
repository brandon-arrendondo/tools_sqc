/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void bad_segv_handler(int sig) {
    printf("Segmentation fault caught, attempting to continue...\n");
    printf("This return will cause undefined behavior\n");
}

void bad_fpe_handler(int sig) {
    printf("Floating point exception caught, ignoring...\n");
    printf("Returning from FPE handler is undefined behavior\n");
}

int main() {
    printf("Demonstrating unsafe return from multiple exception types\n");

    signal(SIGSEGV, bad_segv_handler);
    signal(SIGFPE, bad_fpe_handler);
    signal(SIGILL, bad_segv_handler);

    printf("Creating array access violation...\n");
    volatile int *bad_ptr = (int *)0x12345678;
    printf("Attempting to access invalid memory: %d\n", *bad_ptr);

    printf("If you see this, undefined behavior occurred\n");

    printf("Creating division by zero...\n");
    volatile int zero = 0;
    volatile int result = 100 / zero;
    printf("Division result: %d\n", result);

    printf("Program should not reach this point\n");
    return 0;
}