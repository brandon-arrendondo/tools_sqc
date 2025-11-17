/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ucontext.h>

void program_counter_handler(int sig, siginfo_t *info, void *context) {
    printf("Exception handler: Attempting to manipulate program counter\n");

    ucontext_t *uc = (ucontext_t *)context;

    if (uc != NULL) {
        printf("Signal info: signal=%d, address=%p\n", sig, info->si_addr);

        /* Dangerous attempt to modify execution context */
#ifdef __x86_64__
        printf("Original RIP: 0x%llx\n", uc->uc_mcontext.gregs[REG_RIP]);

        /* Attempt to skip the faulting instruction (very dangerous!) */
        uc->uc_mcontext.gregs[REG_RIP] += 4;

        printf("Modified RIP: 0x%llx\n", uc->uc_mcontext.gregs[REG_RIP]);
#endif

        printf("Program counter manipulation attempted\n");
    }

    printf("Context modification complete, returning (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing program counter manipulation with return\n");
    printf("PID: %d\n", getpid());

    struct sigaction sa;
    sa.sa_sigaction = program_counter_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGSEGV, &sa, NULL);

    printf("Dereferencing null pointer...\n");
    volatile int *null_ptr = NULL;
    volatile int value = *null_ptr;

    printf("This represents undefined behavior\n");
    return 0;
}