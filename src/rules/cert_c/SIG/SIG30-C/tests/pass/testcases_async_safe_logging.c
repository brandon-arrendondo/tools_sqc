/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Pre-formatted messages to avoid sprintf in signal handler
static const char msg_sigusr1[] = "SIGUSR1 received\n";
static const char msg_sigusr2[] = "SIGUSR2 received\n";
static const char msg_sigterm[] = "SIGTERM received\n";
static const char msg_unknown[] = "Unknown signal\n";

void safe_logging_handler(int sig) {
    const char *message;
    size_t msg_len;

    // SAFE: Simple assignment and comparison operations
    switch (sig) {
        case SIGUSR1:
            message = msg_sigusr1;
            msg_len = sizeof(msg_sigusr1) - 1;
            break;
        case SIGUSR2:
            message = msg_sigusr2;
            msg_len = sizeof(msg_sigusr2) - 1;
            break;
        case SIGTERM:
            message = msg_sigterm;
            msg_len = sizeof(msg_sigterm) - 1;
            break;
        default:
            message = msg_unknown;
            msg_len = sizeof(msg_unknown) - 1;
            break;
    }

    // SAFE: write() is async-safe
    ssize_t result = write(STDERR_FILENO, message, msg_len);

    // Ignore result to keep handler simple
    (void)result;
}

int main() {
    printf("Demonstrating async-safe logging in signal handlers\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, safe_logging_handler);
    signal(SIGUSR2, safe_logging_handler);
    signal(SIGTERM, safe_logging_handler);

    printf("Send SIGUSR1, SIGUSR2, or SIGTERM\n");
    printf("Signal handler will log safely using write() system call\n");

    while (1) {
        pause();
    }

    return 0;
}