/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t segv_count = 0;

void segv_handler(int sig) {
    segv_count++;
    printf("SIGSEGV handler: Segmentation fault #%d\n", segv_count);
    printf("Attempting to recover by returning (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing SIGSEGV handler return violation\n");
    printf("PID: %d\n", getpid());

    signal(SIGSEGV, segv_handler);

    printf("Dereferencing null pointer...\n");
    volatile int *null_ptr = NULL;
    volatile int value = *null_ptr;

    printf("Undefined behavior occurs if this executes\n");
    return 0;
}