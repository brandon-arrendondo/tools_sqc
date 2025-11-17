/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t error_flag = 0;
volatile sig_atomic_t fpe_flag = 0;
volatile sig_atomic_t segv_flag = 0;

void flag_setting_handler(int sig) {
    error_flag = 1;

    switch(sig) {
        case SIGFPE:
            fpe_flag = 1;
            printf("SIGFPE: Setting FPE flag and returning\n");
            break;
        case SIGSEGV:
            segv_flag = 1;
            printf("SIGSEGV: Setting SEGV flag and returning\n");
            break;
    }

    printf("Error flags set, returning to main (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing exception handler that sets flags and returns\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, flag_setting_handler);
    signal(SIGSEGV, flag_setting_handler);

    printf("Initial flags - error: %d, fpe: %d, segv: %d\n",
           error_flag, fpe_flag, segv_flag);

    printf("Triggering division by zero...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;

    printf("Flags after exception - error: %d, fpe: %d, segv: %d\n",
           error_flag, fpe_flag, segv_flag);
    printf("This output represents undefined behavior\n");

    return 0;
}