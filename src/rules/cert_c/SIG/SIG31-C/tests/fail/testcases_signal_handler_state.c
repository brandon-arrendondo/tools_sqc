/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Global signal handler state beyond sig_atomic_t - rule violation */
typedef struct {
    int signal_history[100];
    int history_index;
    char signal_names[32][16];
    int signal_counts[32];
    double signal_timestamps[100];
    char last_signal_context[256];
    int nested_signal_depth;
    int signal_processing_errors;
} signal_state_t;

typedef struct {
    struct sigaction old_handlers[32];
    int handlers_installed[32];
    char handler_descriptions[32][64];
    int handler_call_counts[32];
    int signal_mask_changes;
    char signal_configuration[512];
} signal_management_t;

signal_state_t global_signal_state = {0};
signal_management_t global_signal_mgmt = {0};

void get_signal_name(int sig, char *name) {
    switch (sig) {
        case SIGUSR1: strcpy(name, "SIGUSR1"); break;
        case SIGUSR2: strcpy(name, "SIGUSR2"); break;
        case SIGTERM: strcpy(name, "SIGTERM"); break;
        case SIGINT: strcpy(name, "SIGINT"); break;
        default: sprintf(name, "SIG_%d", sig); break;
    }
}

void unsafe_handler(int sig) {
    /* Violation: Accessing global signal handler state beyond sig_atomic_t */

    /* Track signal in history */
    global_signal_state.signal_history[global_signal_state.history_index] = sig;
    global_signal_state.signal_timestamps[global_signal_state.history_index] =
        (double)clock() / CLOCKS_PER_SEC;
    global_signal_state.history_index = (global_signal_state.history_index + 1) % 100;

    /* Update signal counts */
    if (sig >= 0 && sig < 32) {
        global_signal_state.signal_counts[sig]++;
        get_signal_name(sig, global_signal_state.signal_names[sig]);
        global_signal_mgmt.handler_call_counts[sig]++;
    }

    /* Track nested signals */
    global_signal_state.nested_signal_depth++;
    if (global_signal_state.nested_signal_depth > 1) {
        sprintf(global_signal_state.last_signal_context,
                "Nested signal %d at depth %d", sig, global_signal_state.nested_signal_depth);
        global_signal_state.signal_processing_errors++;
    } else {
        sprintf(global_signal_state.last_signal_context,
                "Normal signal %d processing", sig);
    }

    /* Update signal management state */
    global_signal_mgmt.signal_mask_changes++;
    sprintf(global_signal_mgmt.signal_configuration,
            "Handler for %s called %d times",
            global_signal_state.signal_names[sig],
            global_signal_state.signal_counts[sig]);

    printf("Handler: sig=%d, count=%d, depth=%d, errors=%d, context=%s\n",
           sig, global_signal_state.signal_counts[sig],
           global_signal_state.nested_signal_depth,
           global_signal_state.signal_processing_errors,
           global_signal_state.last_signal_context);

    global_signal_state.nested_signal_depth--;
}

int main() {
    printf("Demonstrating unsafe signal handler state access in signal handler\n");
    printf("PID: %d\n", getpid());

    /* Initialize signal state */
    memset(&global_signal_state, 0, sizeof(global_signal_state));
    memset(&global_signal_mgmt, 0, sizeof(global_signal_mgmt));

    /* Set up signal handlers and track them */
    struct sigaction sa;
    sa.sa_handler = unsafe_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, &global_signal_mgmt.old_handlers[SIGUSR1]);
    global_signal_mgmt.handlers_installed[SIGUSR1] = 1;
    strcpy(global_signal_mgmt.handler_descriptions[SIGUSR1], "Debug signal handler");

    sigaction(SIGUSR2, &sa, &global_signal_mgmt.old_handlers[SIGUSR2]);
    global_signal_mgmt.handlers_installed[SIGUSR2] = 1;
    strcpy(global_signal_mgmt.handler_descriptions[SIGUSR2], "Status signal handler");

    sigaction(SIGTERM, &sa, &global_signal_mgmt.old_handlers[SIGTERM]);
    global_signal_mgmt.handlers_installed[SIGTERM] = 1;
    strcpy(global_signal_mgmt.handler_descriptions[SIGTERM], "Termination signal handler");

    for (int i = 0; i < 30; i++) {
        /* Main program also modifies signal state */
        global_signal_state.signal_processing_errors = i / 10;
        global_signal_state.nested_signal_depth = 0;
        sprintf(global_signal_state.last_signal_context,
                "Main program iteration %d", i);

        /* Update signal management configuration */
        sprintf(global_signal_mgmt.signal_configuration,
                "Main: iteration %d, mask_changes=%d",
                i, global_signal_mgmt.signal_mask_changes);

        /* Simulate signal mask operations */
        if (i % 5 == 4) {
            sigset_t mask;
            sigemptyset(&mask);
            sigaddset(&mask, SIGUSR1);
            sigprocmask(SIG_BLOCK, &mask, NULL);
            global_signal_mgmt.signal_mask_changes++;

            usleep(50000);

            sigprocmask(SIG_UNBLOCK, &mask, NULL);
            global_signal_mgmt.signal_mask_changes++;
        }

        /* Show current signal statistics */
        int total_signals = 0;
        for (int j = 0; j < 32; j++) {
            total_signals += global_signal_state.signal_counts[j];
        }

        printf("Main: iter=%d, total_signals=%d, mask_changes=%d, errors=%d\n",
               i, total_signals, global_signal_mgmt.signal_mask_changes,
               global_signal_state.signal_processing_errors);

        usleep(100000);
    }

    return 0;
}