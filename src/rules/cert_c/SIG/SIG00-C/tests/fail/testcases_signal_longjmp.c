/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>

jmp_buf jump_buffer;
volatile sig_atomic_t in_handler = 0;

void longjmp_handler(int sig) {
    printf("Handler: Signal %d received\n", sig);

    if (in_handler) {
        printf("Handler: Already in handler, potential corruption!\n");
        exit(1);
    }

    in_handler = 1;

    // Violation: longjmp from signal handler without proper masking
    // can corrupt program state and stack
    printf("Handler: Performing longjmp...\n");
    longjmp(jump_buffer, sig);
}

int main() {
    struct sigaction sa;
    int signal_value;

    // Install handler without masking
    sa.sa_handler = longjmp_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: longjmp can corrupt stack if handler is interrupted
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Send signals to trigger longjmp from handler\n");

    while (1) {
        signal_value = setjmp(jump_buffer);

        if (signal_value == 0) {
            printf("Main: Starting normal execution\n");
            in_handler = 0;

            // Simulate work
            for (int i = 0; i < 10; i++) {
                printf("Main: Working... %d\n", i);
                sleep(1);
            }
        } else {
            printf("Main: Jumped from signal handler (signal %d)\n", signal_value);
            in_handler = 0;

            // Reset and continue
            sleep(2);
        }
    }

    return 0;
}