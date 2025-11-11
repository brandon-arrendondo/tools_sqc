/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t debug_signals = 0;
volatile sig_atomic_t debug_enabled = 1;

void debug_logging_handler(int sig) {
    debug_signals++;
    printf("Debug handler for signal %d (count: %d, debug: %s)\n",
           sig, debug_signals, debug_enabled ? "ON" : "OFF");

    // VIOLATION: Using signal() for debugging/logging signal management
    if (debug_enabled) {
        printf("Debug mode: registering additional logging signals\n");

        // Register debug handlers for other signals
        if (signal(SIGQUIT, debug_logging_handler) == SIG_ERR) {
            printf("Debug: failed to register SIGQUIT handler\n");
        } else {
            printf("Debug: registered SIGQUIT for logging\n");
        }

        if (signal(SIGPIPE, debug_logging_handler) == SIG_ERR) {
            printf("Debug: failed to register SIGPIPE handler\n");
        } else {
            printf("Debug: registered SIGPIPE for logging\n");
        }

        // Toggle debug mode every few signals
        if (debug_signals % 3 == 0) {
            debug_enabled = 0;
            printf("Debug: disabling debug mode\n");
        }
    } else {
        printf("Debug mode disabled: removing debug handlers\n");

        // Remove debug handlers
        if (signal(SIGQUIT, SIG_DFL) == SIG_ERR) {
            printf("Debug: failed to remove SIGQUIT handler\n");
        } else {
            printf("Debug: removed SIGQUIT handler\n");
        }

        if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
            printf("Debug: failed to ignore SIGPIPE\n");
        } else {
            printf("Debug: ignored SIGPIPE\n");
        }

        // Re-enable debug mode
        if (debug_signals % 5 == 0) {
            debug_enabled = 1;
            printf("Debug: re-enabling debug mode\n");
        }
    }

    // Always re-register this handler for continued logging
    if (signal(sig, debug_logging_handler) == SIG_ERR) {
        printf("Debug: failed to re-register main handler\n");
    } else {
        printf("Debug: re-registered main handler\n");
    }

    printf("Debug signal() operations complete\n");
}

int main() {
    printf("SIG34-C VIOLATION: signal() calls for debugging/logging signals\n");
    printf("Handler dynamically manages debug signal handlers using signal()\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, debug_logging_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1, SIGQUIT, SIGPIPE to see debug signal() management\n");

    while (debug_signals < 10) {
        pause();
    }

    printf("Debug signal operations completed: %d\n", debug_signals);
    return 0;
}