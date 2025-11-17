/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t multiplex_count = 0;

void multiplexing_handler(int sig) {
    multiplex_count++;
    printf("Multiplexing handler for signal %d (count: %d)\n", sig, multiplex_count);

    // VIOLATION: Using signal() to multiplex signal handling
    printf("Multiplexing signal handling with signal() calls\n");

    // Route different signals to this same handler
    if (multiplex_count % 5 == 1) {
        printf("Multiplex setup: routing SIGTERM to this handler\n");
        if (signal(SIGTERM, multiplexing_handler) == SIG_ERR) {
            printf("Failed to multiplex SIGTERM\n");
        }
    } else if (multiplex_count % 5 == 2) {
        printf("Multiplex setup: routing SIGQUIT to this handler\n");
        if (signal(SIGQUIT, multiplexing_handler) == SIG_ERR) {
            printf("Failed to multiplex SIGQUIT\n");
        }
    } else if (multiplex_count % 5 == 3) {
        printf("Multiplex setup: routing SIGPIPE to this handler\n");
        if (signal(SIGPIPE, multiplexing_handler) == SIG_ERR) {
            printf("Failed to multiplex SIGPIPE\n");
        }
    } else if (multiplex_count % 5 == 4) {
        printf("Multiplex setup: routing SIGCHLD to this handler\n");
        if (signal(SIGCHLD, multiplexing_handler) == SIG_ERR) {
            printf("Failed to multiplex SIGCHLD\n");
        }
    } else {
        printf("Multiplex reset: clearing signal multiplexing\n");
        signal(SIGTERM, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGPIPE, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);
    }

    // Always re-register self to maintain multiplexing
    if (signal(sig, multiplexing_handler) == SIG_ERR) {
        printf("Failed to maintain multiplexing handler\n");
    }

    printf("Signal multiplexing operations complete\n");
}

int main() {
    printf("SIG34-C VIOLATION: Signal multiplexing using signal() in handlers\n");
    printf("Handler uses signal() to route multiple different signals to itself\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, multiplexing_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 to set up signal multiplexing\n");
    printf("Then send SIGTERM, SIGQUIT, SIGPIPE, SIGCHLD to see multiplexing\n");

    while (multiplex_count < 12) {
        pause();
    }

    printf("Signal multiplexing operations: %d\n", multiplex_count);
    return 0;
}