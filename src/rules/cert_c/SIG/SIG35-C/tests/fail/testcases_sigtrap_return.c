/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void trap_handler(int sig) {
    printf("SIGTRAP handler: Trace/breakpoint trap caught\n");
    printf("Continuing execution from trap (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing SIGTRAP handler return violation\n");
    printf("PID: %d\n", getpid());

    signal(SIGTRAP, trap_handler);

    printf("Triggering breakpoint instruction...\n");
    /* Trigger SIGTRAP with breakpoint instruction */
    __asm__("int3"); /* x86/x64 breakpoint instruction */

    printf("Undefined behavior if this executes after trap\n");
    return 0;
}