/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t cleanup_calls = 0;

void cleanup_signal_handler(int sig) {
    cleanup_calls++;
    printf("Cleanup handler called for signal %d (call %d)\n", sig, cleanup_calls);

    // VIOLATION: Using signal() for cleanup operations within handler
    printf("Performing cleanup signal() operations\n");

    // Cleanup: reset various signal handlers
    if (signal(SIGPIPE, SIG_DFL) == SIG_ERR) {
        printf("Cleanup failed: couldn't reset SIGPIPE\n");
    } else {
        printf("Cleanup: reset SIGPIPE to default\n");
    }

    if (signal(SIGCHLD, SIG_DFL) == SIG_ERR) {
        printf("Cleanup failed: couldn't reset SIGCHLD\n");
    } else {
        printf("Cleanup: reset SIGCHLD to default\n");
    }

    // Cleanup: ignore certain signals
    if (signal(SIGTERM, SIG_IGN) == SIG_ERR) {
        printf("Cleanup failed: couldn't ignore SIGTERM\n");
    } else {
        printf("Cleanup: ignored SIGTERM\n");
    }

    // Cleanup: re-establish this handler for persistence
    if (signal(sig, cleanup_signal_handler) == SIG_ERR) {
        printf("Cleanup failed: couldn't re-establish handler\n");
    } else {
        printf("Cleanup: re-established this handler\n");
    }

    // Final cleanup step
    if (cleanup_calls >= 5) {
        printf("Final cleanup: resetting all to default\n");
        if (signal(SIGUSR1, SIG_DFL) == SIG_ERR) {
            printf("Final cleanup failed for SIGUSR1\n");
        }
        if (signal(SIGUSR2, SIG_DFL) == SIG_ERR) {
            printf("Final cleanup failed for SIGUSR2\n");
        }
    }

    printf("Cleanup signal() operations complete\n");
}

int main() {
    printf("SIG34-C VIOLATION: Using signal() for cleanup operations in handlers\n");
    printf("Handler attempts cleanup by modifying signal dispositions\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, cleanup_signal_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 to trigger cleanup signal() operations\n");

    while (cleanup_calls < 8) {
        pause();
    }

    printf("Cleanup operations completed: %d\n", cleanup_calls);
    return 0;
}