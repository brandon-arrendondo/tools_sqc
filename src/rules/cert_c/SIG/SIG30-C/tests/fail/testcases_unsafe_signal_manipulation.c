/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void signal_handler(int sig) {
    // VIOLATION: sigaction() is not async-safe
    struct sigaction new_action;
    new_action.sa_handler = SIG_DFL;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction(SIGUSR2, &new_action, NULL);

    // VIOLATION: sigprocmask() is not async-safe
    sigset_t new_mask, old_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);

    // VIOLATION: sigpending() is not async-safe
    sigset_t pending_signals;
    sigpending(&pending_signals);

    // VIOLATION: sigsuspend() is not async-safe in signal handler
    sigset_t wait_mask;
    sigemptyset(&wait_mask);
    // sigsuspend(&wait_mask);  // Would block indefinitely

    // VIOLATION: signal() inside handler can cause issues
    signal(SIGUSR2, SIG_IGN);
}

int main() {
    printf("Demonstrating unsafe signal manipulation in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, signal_handler);

    printf("Send SIGUSR1 to trigger unsafe signal operations\n");

    while (1) {
        pause();
    }

    return 0;
}