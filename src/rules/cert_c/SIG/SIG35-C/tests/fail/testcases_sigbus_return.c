/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

void bus_handler(int sig) {
    printf("SIGBUS handler: Bus error detected\n");
    printf("Attempting to continue despite bus error (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing SIGBUS handler return violation\n");
    printf("PID: %d\n", getpid());

    signal(SIGBUS, bus_handler);

    /* Trigger bus error by accessing unmapped memory */
    printf("Creating memory mapping and then accessing beyond it...\n");
    void *ptr = mmap(NULL, 4096, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr != MAP_FAILED) {
        munmap(ptr, 4096);
        /* Access unmapped memory to trigger SIGBUS */
        volatile int value = *((int*)ptr);
        printf("Value: %d (undefined behavior if printed)\n", value);
    }

    printf("This represents undefined behavior\n");
    return 0;
}