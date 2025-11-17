/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t portable_count = 0;

void portable_handler(int sig) {
    portable_count++;
}

/* Portable signal handler installation */
int install_signal_handler(int sig, void (*handler)(int)) {
    struct sigaction sa, old_sa;

    /* Save old handler */
    if (sigaction(sig, NULL, &old_sa) == -1) {
        return -1;
    }

    /* Set up new handler with portable flags */
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);

    /* Use flags for maximum portability */
#ifdef SA_RESTART
    sa.sa_flags = SA_RESTART;  /* Restart interrupted system calls */
#else
    sa.sa_flags = 0;
#endif

    /* Install handler */
    if (sigaction(sig, &sa, NULL) == -1) {
        return -1;
    }

    return 0;
}

/* Portable signal handler removal */
int remove_signal_handler(int sig) {
    struct sigaction sa;

    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    return sigaction(sig, &sa, NULL);
}

int main() {
    printf("PASS: Portable signal handling patterns\n");

    printf("PID: %d\n", getpid());

    /* Use portable installation function */
    if (install_signal_handler(SIGUSR1, portable_handler) == -1) {
        perror("install_signal_handler");
        exit(EXIT_FAILURE);
    }

    printf("Portable signal handler installed\n");

#ifdef SA_RESTART
    printf("Using SA_RESTART for system call restart compatibility\n");
#else
    printf("SA_RESTART not available, using basic setup\n");
#endif

    printf("Send SIGUSR1 to test portable handler\n");

    /* Test portable handler */
    int test_signals = 3;
    int i;
    for (i = 0; i < test_signals; i++) {
        raise(SIGUSR1);
        usleep(200000);
    }

    printf("Portable signals handled: %d/%d\n", portable_count, test_signals);

    /* Clean up using portable removal */
    if (remove_signal_handler(SIGUSR1) == -1) {
        perror("remove_signal_handler");
        exit(EXIT_FAILURE);
    }

    printf("Signal handler removed portably\n");
    return 0;
}