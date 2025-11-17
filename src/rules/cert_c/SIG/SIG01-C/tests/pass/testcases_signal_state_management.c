/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t state_count = 0;
volatile sig_atomic_t handler_installed = 0;

void state_managed_handler(int sig) {
    state_count++;
}

/* Save and restore signal state safely */
struct signal_state {
    struct sigaction old_action;
    int signal_number;
    int was_installed;
};

int save_signal_state(int sig, struct signal_state* state) {
    state->signal_number = sig;

    if (sigaction(sig, NULL, &state->old_action) == -1) {
        return -1;
    }

    state->was_installed = (state->old_action.sa_handler != SIG_DFL &&
                           state->old_action.sa_handler != SIG_IGN);
    return 0;
}

int restore_signal_state(const struct signal_state* state) {
    return sigaction(state->signal_number, &state->old_action, NULL);
}

int install_managed_handler(int sig, void (*handler)(int), struct signal_state* state) {
    struct sigaction sa;

    /* Save current state */
    if (save_signal_state(sig, state) == -1) {
        return -1;
    }

    /* Install new handler */
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(sig, &sa, NULL) == -1) {
        return -1;
    }

    return 0;
}

int main() {
    struct signal_state saved_state;
    printf("PASS: Proper signal state management\n");

    printf("PID: %d\n", getpid());

    /* Install handler with state management */
    if (install_managed_handler(SIGUSR2, state_managed_handler, &saved_state) == -1) {
        perror("install_managed_handler");
        exit(EXIT_FAILURE);
    }

    handler_installed = 1;
    printf("Signal handler installed with state management\n");

    printf("Send SIGUSR2 to test managed handler\n");

    /* Test the handler */
    raise(SIGUSR2);
    raise(SIGUSR2);
    sleep(1);

    printf("State managed signals: %d\n", state_count);

    /* Restore original signal state */
    if (restore_signal_state(&saved_state) == -1) {
        perror("restore_signal_state");
        exit(EXIT_FAILURE);
    }

    handler_installed = 0;
    printf("Original signal state restored\n");

    /* Verify restoration */
    printf("Signal state management completed successfully\n");

    return 0;
}