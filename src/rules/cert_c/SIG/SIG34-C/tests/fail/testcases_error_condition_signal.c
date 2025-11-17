/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

volatile sig_atomic_t error_signals = 0;
volatile sig_atomic_t error_count = 0;

void error_handling_signal_handler(int sig) {
    error_signals++;
    printf("Signal %d received in error condition (signal count: %d)\n", sig, error_signals);

    // Simulate various error conditions
    if (error_signals % 4 == 1) {
        error_count++;
        printf("Error condition %d: attempting signal() recovery\n", error_count);

        // VIOLATION: Calling signal() in error conditions within handler
        if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
            printf("Failed to ignore SIGPIPE during error recovery\n");
        } else {
            printf("Ignored SIGPIPE as error recovery\n");
        }
    } else if (error_signals % 4 == 2) {
        error_count++;
        printf("Error condition %d: resetting signal handlers\n", error_count);

        // VIOLATION: Error recovery using signal() calls
        if (signal(SIGTERM, SIG_DFL) == SIG_ERR) {
            printf("Failed to reset SIGTERM during error\n");
        } else {
            printf("Reset SIGTERM as error recovery\n");
        }
    } else if (error_signals % 4 == 3) {
        error_count++;
        printf("Error condition %d: establishing backup handlers\n", error_count);

        // VIOLATION: Setting up backup handlers with signal()
        if (signal(SIGQUIT, error_handling_signal_handler) == SIG_ERR) {
            printf("Failed to establish backup SIGQUIT handler\n");
        } else {
            printf("Established backup SIGQUIT handler\n");
        }
    } else {
        error_count++;
        printf("Error condition %d: panic signal() calls\n", error_count);

        // VIOLATION: Panic mode signal() calls
        if (signal(sig, SIG_IGN) == SIG_ERR) {
            printf("Panic: failed to ignore current signal\n");
        } else {
            printf("Panic: ignored current signal\n");
        }
    }

    printf("Error condition signal() handling complete\n");
}

int main() {
    printf("SIG34-C VIOLATION: signal() calls in error conditions within handlers\n");
    printf("Handler attempts error recovery using signal() calls\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, error_handling_signal_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 to trigger error condition signal() calls\n");

    while (error_count < 8) {
        pause();
    }

    printf("Error condition handling completed: %d errors\n", error_count);
    return 0;
}