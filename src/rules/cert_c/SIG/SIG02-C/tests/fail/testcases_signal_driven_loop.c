/*
 * Rule: SIG02-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG02-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t continue_processing = 1;
volatile sig_atomic_t iteration_count = 0;

void loop_control_handler(int sig) {
    if (sig == SIGUSR1) {
        iteration_count++;
        printf("Signal-driven loop iteration %d\n", iteration_count);
    } else if (sig == SIGUSR2) {
        continue_processing = 0;
        printf("Signal to stop loop received\n");
    }
}

int main() {
    printf("Using signals for normal processing loop control (BAD)\n");

    signal(SIGUSR1, loop_control_handler);
    signal(SIGUSR2, loop_control_handler);

    pid_t controller = fork();
    if (controller == 0) {
        printf("Controller: Starting to drive processing loop\n");

        for (int i = 0; i < 8; i++) {
            sleep(1);
            printf("Controller: Triggering loop iteration %d\n", i + 1);
            kill(getppid(), SIGUSR1);
        }

        sleep(1);
        printf("Controller: Sending stop signal\n");
        kill(getppid(), SIGUSR2);
        exit(0);
    } else {
        printf("Worker: Starting signal-driven processing loop\n");

        while (continue_processing) {
            pause();  // Wait for signal

            if (iteration_count > 0 && continue_processing) {
                printf("Worker: Processing iteration %d\n", iteration_count);
                printf("Worker: Performing normal business logic...\n");
                // Simulate normal processing work
                for (int i = 0; i < 1000000; i++) {
                    // Busy work
                }
            }
        }

        printf("Worker: Loop terminated by signal\n");
        wait(NULL);
    }

    return 0;
}