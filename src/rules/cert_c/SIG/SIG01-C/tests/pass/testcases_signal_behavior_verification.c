/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t verification_count = 0;

void verification_handler(int sig) {
    verification_count++;
}

int test_signal_persistence(void) {
    struct sigaction sa;
    int initial_count, final_count;

    printf("Testing signal handler persistence behavior...\n");

    /* Set up handler */
    sa.sa_handler = verification_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return -1;
    }

    initial_count = verification_count;

    /* Send first signal */
    raise(SIGUSR1);
    usleep(100000);  /* Allow signal processing */

    /* Send second signal to test persistence */
    raise(SIGUSR1);
    usleep(100000);  /* Allow signal processing */

    final_count = verification_count;

    printf("Initial count: %d, Final count: %d\n", initial_count, final_count);

    /* With sigaction, we should get both signals */
    if (final_count >= initial_count + 2) {
        printf("Signal handler persisted correctly\n");
        return 1;  /* Handler persisted */
    } else {
        printf("Signal handler did not persist (unexpected with sigaction)\n");
        return 0;  /* Handler did not persist */
    }
}

int main() {
    printf("PASS: Signal behavior verification before relying on persistence\n");

    printf("PID: %d\n", getpid());

    /* Test and verify signal behavior before depending on it */
    int persistence_result = test_signal_persistence();

    if (persistence_result == 1) {
        printf("Signal persistence verified - safe to rely on handler\n");

        /* Now we can safely use the signal handler knowing it persists */
        printf("Send SIGUSR1 to test verified persistent handler\n");

        while (verification_count < 5) {
            pause();
        }

        printf("Successfully handled %d signals with verified persistence\n",
               verification_count);
    } else if (persistence_result == 0) {
        printf("Signal persistence not guaranteed - using alternative approach\n");
        /* Could implement alternative approach here */
    } else {
        printf("Could not verify signal behavior\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}