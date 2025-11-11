/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t signal_count = 0;

void reliable_handler(int sig) {
    signal_count++;
    printf("Reliable signal %d received (count: %d)\n", sig, signal_count);
}

int main() {
    struct sigaction sa;
    printf("PASS: Platform-aware signal handling using sigaction\n");

    /* Use sigaction for reliable, portable signal handling */
    sa.sa_handler = reliable_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;  /* No special flags needed for basic reliability */

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\n", getpid());
    printf("Signal handler registered with sigaction for reliability\n");

#ifdef __linux__
    printf("Running on Linux - sigaction provides reliable semantics\n");
#elif defined(__APPLE__)
    printf("Running on macOS - sigaction ensures BSD-style reliability\n");
#elif defined(_WIN32)
    printf("Running on Windows - using most portable signal approach\n");
#else
    printf("Unknown platform - sigaction provides maximum portability\n");
#endif

    /* Test reliable signal handling */
    printf("Send SIGUSR1 multiple times to verify reliable handling\n");

    while (signal_count < 5) {
        pause();
    }

    printf("Successfully received %d signals reliably\n", signal_count);
    return 0;
}