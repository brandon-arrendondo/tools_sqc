/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t exception_type = 0;

void exit_family_handler(int sig) {
    exception_type = sig;

    printf("COMPUTATIONAL EXCEPTION: Signal %d detected\n", sig);

    /* Only use functions from the exit family - no other operations */
    switch (sig) {
        case SIGFPE:
            printf("FPE detected: Using exit()\n");
            exit(EXIT_FAILURE);
            break;

        case SIGSEGV:
            printf("SEGV detected: Using _Exit()\n");
            _Exit(EXIT_FAILURE);
            break;

        case SIGILL:
            printf("ILL detected: Using abort()\n");
            abort();
            break;

        default:
            printf("Other exception: Using quick_exit()\n");
            quick_exit(EXIT_FAILURE);
            break;
    }

    /* COMPLIANT: All paths lead to termination, never returns */
}

int main() {
    printf("Demonstrating exclusive use of exit family functions\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, exit_family_handler);
    signal(SIGSEGV, exit_family_handler);
    signal(SIGILL, exit_family_handler);

    printf("Exception type: %d\n", exception_type);

    printf("Triggering illegal instruction...\n");
    /* Platform-specific illegal instruction */
    __asm__("ud2");

    printf("This line should never be reached\n");
    return 0;
}