/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef enum {
    STATE_INIT,
    STATE_ACTIVE,
    STATE_PROCESSING,
    STATE_CLEANUP,
    STATE_ERROR
} handler_state_t;

volatile sig_atomic_t current_state = STATE_INIT;
volatile sig_atomic_t state_transitions = 0;

void complex_state_handler(int sig) {
    state_transitions++;
    printf("State handler called for signal %d (transition %d, state %d)\n",
           sig, state_transitions, current_state);

    // VIOLATION: Complex signal state management using signal() in handlers
    switch (current_state) {
        case STATE_INIT:
            printf("INIT state: setting up signal handlers\n");
            current_state = STATE_ACTIVE;

            if (signal(SIGUSR2, complex_state_handler) == SIG_ERR) {
                printf("Failed to register SIGUSR2 in INIT state\n");
                current_state = STATE_ERROR;
            }
            if (signal(SIGTERM, complex_state_handler) == SIG_ERR) {
                printf("Failed to register SIGTERM in INIT state\n");
                current_state = STATE_ERROR;
            }
            break;

        case STATE_ACTIVE:
            printf("ACTIVE state: transitioning to processing\n");
            current_state = STATE_PROCESSING;

            if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
                printf("Failed to ignore SIGPIPE in ACTIVE state\n");
                current_state = STATE_ERROR;
            }
            if (signal(sig, complex_state_handler) == SIG_ERR) {
                printf("Failed to re-register handler in ACTIVE state\n");
                current_state = STATE_ERROR;
            }
            break;

        case STATE_PROCESSING:
            printf("PROCESSING state: transitioning to cleanup\n");
            current_state = STATE_CLEANUP;

            if (signal(SIGCHLD, SIG_DFL) == SIG_ERR) {
                printf("Failed to reset SIGCHLD in PROCESSING state\n");
                current_state = STATE_ERROR;
            }
            break;

        case STATE_CLEANUP:
            printf("CLEANUP state: returning to active\n");
            current_state = STATE_ACTIVE;

            if (signal(SIGUSR1, SIG_DFL) == SIG_ERR) {
                printf("Failed to reset SIGUSR1 in CLEANUP state\n");
                current_state = STATE_ERROR;
            }
            if (signal(SIGUSR2, SIG_DFL) == SIG_ERR) {
                printf("Failed to reset SIGUSR2 in CLEANUP state\n");
                current_state = STATE_ERROR;
            }
            break;

        case STATE_ERROR:
            printf("ERROR state: attempting recovery\n");
            current_state = STATE_INIT;

            if (signal(sig, complex_state_handler) == SIG_ERR) {
                printf("Failed to recover in ERROR state\n");
            }
            break;
    }

    printf("State management signal() operations complete (new state: %d)\n", current_state);
}

int main() {
    printf("SIG34-C VIOLATION: Complex signal state management using signal()\n");
    printf("Handler maintains state machine and uses signal() for state transitions\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, complex_state_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1, SIGUSR2, SIGTERM to see complex state management\n");

    while (state_transitions < 12) {
        pause();
    }

    printf("State transitions completed: %d (final state: %d)\n", state_transitions, current_state);
    return 0;
}