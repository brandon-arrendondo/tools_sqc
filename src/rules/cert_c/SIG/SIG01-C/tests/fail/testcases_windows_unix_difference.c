/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t operations_count = 0;

void operation_handler(int sig) {
    operations_count++;
    printf("Operation %d completed\n", operations_count);
}

int main() {
    printf("FAIL: Code assumes UNIX signal behavior on Windows\n");

#ifdef _WIN32
    printf("Running on Windows - signal() behavior differs from UNIX\n");
#else
    printf("Running on UNIX - but code written with Windows assumptions\n");
#endif

    /* Assumes signal() works the same on Windows and UNIX */
    signal(SIGTERM, operation_handler);

    printf("PID: %d\n", getpid());
    printf("Expecting persistent handler behavior across platforms\n");

    /* This loop assumes handler will persist consistently */
    int i;
    for (i = 0; i < 5; i++) {
        raise(SIGTERM);
        sleep(1);
    }

    printf("Operations completed: %d (expected 5)\n", operations_count);
    printf("May fail on platforms where signal() resets handler\n");

    return 0;
}