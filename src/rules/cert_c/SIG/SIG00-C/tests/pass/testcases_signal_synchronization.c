/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t processing_complete = 0;
volatile sig_atomic_t data_ready = 0;

void data_ready_handler(int sig) {
    // Compliant: Simple atomic flag setting
    data_ready = 1;

    // Async-safe output
    char msg[] = "Data ready signal received\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

void processing_done_handler(int sig) {
    // Compliant: Simple atomic flag setting
    processing_complete = 1;

    // Async-safe output
    char msg[] = "Processing complete signal received\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

int main() {
    struct sigaction sa1, sa2;

    // Setup first handler
    sa1.sa_handler = data_ready_handler;
    sigemptyset(&sa1.sa_mask);
    // Compliant: Mask both signals during execution of either handler
    sigaddset(&sa1.sa_mask, SIGUSR1);
    sigaddset(&sa1.sa_mask, SIGUSR2);
    sa1.sa_flags = 0;

    // Setup second handler
    sa2.sa_handler = processing_done_handler;
    sigemptyset(&sa2.sa_mask);
    // Compliant: Same masking for coordinated signal handling
    sigaddset(&sa2.sa_mask, SIGUSR1);
    sigaddset(&sa2.sa_mask, SIGUSR2);
    sa2.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa1, NULL) == -1) {
        perror("sigaction SIGUSR1");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGUSR2, &sa2, NULL) == -1) {
        perror("sigaction SIGUSR2");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\n", getpid());
    printf("Send SIGUSR1 for data ready, SIGUSR2 for processing complete\n");
    printf("Signals are properly synchronized\n");

    while (1) {
        if (data_ready && !processing_complete) {
            printf("Data is ready, processing...\n");

            // Simulate data processing
            for (int i = 0; i < 5; i++) {
                printf("  Processing step %d/5\n", i + 1);
                sleep(1);
            }

            printf("Processing finished, waiting for completion signal\n");
        }

        if (data_ready && processing_complete) {
            printf("Both data ready and processing complete!\n");
            printf("Resetting flags for next cycle\n");

            // Reset for next cycle
            data_ready = 0;
            processing_complete = 0;
        }

        printf("Status: data_ready=%d, processing_complete=%d\n",
               data_ready, processing_complete);

        sleep(1);
    }

    return 0;
}