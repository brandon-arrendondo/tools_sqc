/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

volatile sig_atomic_t reliable_count = 0;

void reliable_handler(int sig) {
    reliable_count++;
}

/* Reliable signal handler installation with error checking */
int install_reliable_handler(int sig, void (*handler)(int)) {
    struct sigaction sa, old_sa;

    /* Clear the structure */
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    /* Add signal to its own mask to prevent recursive calls */
    sigaddset(&sa.sa_mask, sig);

#ifdef SA_RESTART
    /* Restart interrupted system calls for reliability */
    sa.sa_flags |= SA_RESTART;
#endif

    /* Save old handler for potential restoration */
    if (sigaction(sig, NULL, &old_sa) == -1) {
        return -1;
    }

    /* Install new handler */
    if (sigaction(sig, &sa, NULL) == -1) {
        return -1;
    }

    /* Verify installation was successful */
    struct sigaction verify_sa;
    if (sigaction(sig, NULL, &verify_sa) == -1) {
        /* Restore old handler on verification failure */
        sigaction(sig, &old_sa, NULL);
        return -1;
    }

    if (verify_sa.sa_handler != handler) {
        /* Handler not installed correctly, restore old one */
        sigaction(sig, &old_sa, NULL);
        errno = EINVAL;
        return -1;
    }

    return 0;
}

/* Test signal handler reliability */
int test_handler_reliability(int sig, int test_count) {
    int initial_count = reliable_count;
    int i;

    printf("Testing handler reliability with %d signals...\n", test_count);

    for (i = 0; i < test_count; i++) {
        if (raise(sig) != 0) {
            perror("raise");
            return -1;
        }
        usleep(50000);  /* Small delay between signals */
    }

    /* Allow time for all signals to be processed */
    sleep(1);

    int signals_handled = reliable_count - initial_count;
    printf("Sent %d signals, handled %d\n", test_count, signals_handled);

    /* With sigaction, we should handle all signals reliably */
    return (signals_handled == test_count) ? 1 : 0;
}

int main() {
    printf("PASS: Reliable signal handling with sigaction\n");

    printf("PID: %d\n", getpid());

    /* Install reliable handler */
    if (install_reliable_handler(SIGUSR1, reliable_handler) == -1) {
        perror("install_reliable_handler");
        exit(EXIT_FAILURE);
    }

    printf("Reliable signal handler installed successfully\n");

    /* Test reliability with multiple signals */
    if (test_handler_reliability(SIGUSR1, 5) == 1) {
        printf("Signal handler demonstrated reliable behavior\n");
    } else {
        printf("WARNING: Signal handler reliability test failed\n");
    }

    printf("Send SIGUSR1 for additional testing\n");

    /* Additional interactive test */
    while (reliable_count < 10) {
        pause();
    }

    printf("Reliable handling completed with %d signals\n", reliable_count);
    printf("sigaction provides reliable signal handler persistence\n");

    return 0;
}