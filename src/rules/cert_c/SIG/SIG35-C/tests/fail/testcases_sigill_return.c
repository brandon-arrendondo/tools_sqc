/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void ill_handler(int sig) {
    printf("SIGILL handler: Illegal instruction detected\n");
    printf("Trying to ignore and continue (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

void trigger_illegal_instruction() {
    /* This attempts to execute an illegal instruction */
    /* Implementation-specific way to trigger SIGILL */
    printf("Attempting to trigger illegal instruction...\n");
    __asm__("ud2"); /* x86/x64 undefined instruction */
}

int main() {
    printf("Testing SIGILL handler return violation\n");
    printf("PID: %d\n", getpid());

    signal(SIGILL, ill_handler);

    trigger_illegal_instruction();

    printf("This line represents undefined behavior\n");
    return 0;
}