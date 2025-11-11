/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>

static int signal_pipe[2];

void safe_pipe_handler(int sig) {
    // SAFE: write() is async-safe
    char signal_byte = (char)sig;
    ssize_t result = write(signal_pipe[1], &signal_byte, 1);

    // Ignore write result to avoid complex error handling
    (void)result;  // Suppress unused variable warning
}

int main() {
    printf("Demonstrating self-pipe trick for safe signal handling\n");
    printf("PID: %d\n", getpid());

    // Create pipe for signal communication
    if (pipe(signal_pipe) == -1) {
        perror("pipe");
        exit(1);
    }

    // Install signal handler
    signal(SIGUSR1, safe_pipe_handler);
    signal(SIGUSR2, safe_pipe_handler);
    signal(SIGTERM, safe_pipe_handler);

    printf("Send SIGUSR1, SIGUSR2, or SIGTERM to trigger safe signal handling\n");
    printf("The signal handler only writes to a pipe, all processing is in main loop\n");

    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(signal_pipe[0], &readfds);

        // Wait for signal notification via pipe
        int result = select(signal_pipe[0] + 1, &readfds, NULL, NULL, NULL);

        if (result > 0 && FD_ISSET(signal_pipe[0], &readfds)) {
            char signal_byte;
            if (read(signal_pipe[0], &signal_byte, 1) == 1) {
                // Safe to do complex processing here in main context
                printf("Received signal %d safely via self-pipe trick\n", (int)signal_byte);

                switch (signal_byte) {
                    case SIGUSR1:
                        printf("Processing SIGUSR1 in main context - safe!\n");
                        break;
                    case SIGUSR2:
                        printf("Processing SIGUSR2 in main context - safe!\n");
                        break;
                    case SIGTERM:
                        printf("Terminating safely...\n");
                        close(signal_pipe[0]);
                        close(signal_pipe[1]);
                        exit(0);
                    default:
                        printf("Unknown signal received\n");
                        break;
                }
            }
        }
    }

    return 0;
}