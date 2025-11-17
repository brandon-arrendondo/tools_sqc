/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>

void library_calling_handler(int sig) {
    printf("Exception handler: Calling various library functions\n");

    /* Call non-async-safe library functions */
    time_t current_time = time(NULL);
    char *time_str = ctime(&current_time);

    printf("Current time: %s", time_str);

    /* Call math library functions */
    double result = sqrt(100.0);
    printf("Square root calculation: %f\n", result);

    /* String manipulation */
    char buffer[100];
    strcpy(buffer, "Exception handled");
    strcat(buffer, " with library calls");
    printf("String result: %s\n", buffer);

    printf("Library functions completed, returning (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing library function calls in exception handler with return\n");
    printf("PID: %d\n", getpid());

    signal(SIGSEGV, library_calling_handler);

    printf("Dereferencing null pointer...\n");
    volatile int *null_ptr = NULL;
    volatile int value = *null_ptr;

    printf("This represents undefined behavior\n");
    return 0;
}